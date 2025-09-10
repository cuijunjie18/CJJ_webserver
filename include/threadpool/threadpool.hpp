#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include "CGImysql/sql_connection_pool.hpp"
#include "common.hpp"

template <typename T>
class ThreadPool
{
public:
    /*thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量*/
    ThreadPool(
        int actor_model, 
        ConnectionPool *connPool, 
        int thread_number = 8, 
        int max_request = 10000);
    ~ThreadPool();
    bool append(T *request, int state);
    bool append_p(T *request);

private:
    /*工作线程运行的函数，它不断从工作队列中取出任务并执行之*/
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;        // 线程池中的线程数
    int m_max_requests;         // 请求队列中允许的最大请求数
    pthread_t *m_threads;       // 描述线程池的数组，其大小为m_thread_number
    std::list<T *> m_workqueue; // 请求队列
    locker m_queuelocker;       // 保护请求队列的互斥锁
    sem m_queuestat;            // 是否有任务需要处理
    ConnectionPool *m_connPool; // 数据库
    int m_actor_model;          // 模型切换
};

/*下面是具体实现*/

// 线程池初始化
template <typename T>
ThreadPool<T>::ThreadPool(
    int actor_model, 
    ConnectionPool *connPool, 
    int thread_number, int max_requests) 
    : 
    m_actor_model(actor_model),
    m_thread_number(thread_number), m_max_requests(max_requests), 
    m_threads(NULL),m_connPool(connPool)
{
    if (thread_number <= 0 || max_requests <= 0)
        throw std::exception();
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
        throw std::exception();
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

// 释放线程ppid
template <typename T>
ThreadPool<T>::~ThreadPool()
{
    delete[] m_threads;
}

// 添加请求到请求队列
template<typename T>
bool ThreadPool<T>::append(T *request, int state){
    m_queuelocker.lock();

    if (m_workqueue.size() > m_max_requests){
        m_queuelocker.unlock();
        return false;
    }

    request->m_state = state;
    m_workqueue.push_back(request);
    
    m_queuelocker.unlock();
    m_queuestat.post();

    return true;
}

template<typename T>
bool ThreadPool<T>::append_p(T *request){
        m_queuelocker.lock();

    if (m_workqueue.size() > m_max_requests){
        m_queuelocker.unlock();
        return false;
    }

    m_workqueue.push_back(request);
    
    m_queuelocker.unlock();
    m_queuestat.post();

    return true;
}

// 工作线程运行函数
template<typename T>
void* ThreadPool<T>::worker(void *arg){
    ThreadPool* pool = (ThreadPool*)arg;
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run(){
    while (true) {
        m_queuestat.wait();
        m_queuelocker.lock();

        if (m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        if (!request) continue;

        // Reactor模式
        if (m_actor_model == Reactor__Mode){
            // 读
            if (request->m_state == 0){
                if (request->read_once()){
                    request->improv = 1;
                    ConnectionRAII mysqlcon(&request->mysql, m_connPool);
                    request->process();
                }else{
                    request->improv = 1;
                    request->timer_flag = 1;
                }
            }else { // 写
                if (request->write()){
                    request->improv = 1;
                }
                else{
                    request->improv = 1;
                    request->timer_flag = 1;
                }
            }
        }
        else if (m_actor_model == Proactor_Mode){ // Proactor模式
            ConnectionRAII mysqlcon(&request->mysql, m_connPool);
            request->process();
        }
    }
}

#endif