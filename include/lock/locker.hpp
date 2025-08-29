#ifndef LOCKER_HPP
#define LOCKER_HPP

#include <pthread.h>
#include <exception>
#include <semaphore.h>


/*信号量封装实现*/
class sem
{
public:
    sem(){
        int rc = sem_init(&sig, 0, 0); // 不指定初始化为0
        if (rc != 0){
            throw std::exception();
        }
    }

    sem(int num){
        int rc = sem_init(&sig, 0, num);
        if (rc != 0){
            throw std::exception();
        }
    }

    ~sem(){
        sem_destroy(&sig);
    }

    bool post(){
        return sem_post(&sig) == 0;
    }

    bool wait(){
        return sem_wait(&sig) == 0;
    }
    
private:
    sem_t sig;
};

class locker
{
public:
    locker(){
        int rc = pthread_mutex_init(&mutex, nullptr);
        if (rc != 0){
            throw std::exception();
        }
    }

    ~locker(){
        pthread_mutex_destroy(&mutex); // 析构函数不抛出异常
    }

    bool lock(){
        return pthread_mutex_lock(&mutex) == 0;
    }

    bool unlock(){
        return pthread_mutex_unlock(&mutex) == 0;
    }

    pthread_mutex_t *get()
    {
        return &mutex;
    }

private:
    pthread_mutex_t mutex;
};

class cond
{
public:
    cond(){
        int rc = pthread_cond_init(&c_cond, nullptr);
        if (rc != 0){
            throw std::exception();
        }
        // rc = pthread_mutex_init(&c_mutex, nullptr);
        // if (rc != 0){
        //     throw std::exception();
        // }
    }

    ~cond(){
        // pthread_mutex_destroy(&c_mutex);
        pthread_cond_destroy(&c_cond);
    }

    bool wait(pthread_mutex_t* c_mutex){
        return pthread_cond_wait(&c_cond, c_mutex) == 0;
    }

    bool timewait(pthread_mutex_t* c_mutex, struct timespec t){ 
        return pthread_cond_timedwait(&c_cond, c_mutex, &t) == 0;
    }

    bool signal(){
        return pthread_cond_signal(&c_cond) == 0;
    }

    bool broadcast(){
        return pthread_cond_broadcast(&c_cond) == 0;
    }

private:
    pthread_cond_t c_cond;
    // pthread_mutex_t c_mutex; // 自己内部的锁？
};

#endif