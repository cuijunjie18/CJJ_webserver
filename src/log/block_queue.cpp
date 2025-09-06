#include <log/block_queue.hpp>
#include <assert.h>

BlockQueue::BlockQueue(int maxn){
    assert(maxn > 0);
    maxn_size = maxn;
}

BlockQueue::~BlockQueue(){
    while (!que.empty()) que.pop();
}

void BlockQueue::clear(){
    b_lock.lock();
    while (!que.empty()) que.pop();
    b_lock.unlock();
}

bool BlockQueue::full(){
    b_lock.lock();
    bool flag = (que.size() >= maxn_size);
    b_lock.unlock();
    return flag;
}

bool BlockQueue::empty(){
    b_lock.lock();
    bool flag = que.empty();
    b_lock.unlock();
    return flag;
}

int BlockQueue::size(){
    b_lock.lock();
    int temp_size = que.size();
    b_lock.unlock();
    return temp_size;
}

int BlockQueue::max_size(){
    return this->maxn_size;
}

std::string BlockQueue::front(){
    b_lock.lock();
    std::string temp = que.front();
    b_lock.unlock();
    return temp;
}

bool BlockQueue::push(std::string &item){
    b_lock.lock();
    if (full()){
        b_cond.broadcast();
        b_lock.unlock();
        return false;
    }
    que.push(item);
    b_cond.broadcast(); // 有就满足条件
    b_lock.unlock();
    return true;
}

bool BlockQueue::pop(std::string &item){
    b_lock.lock();

    // 对于多消费者使用while，因为唤醒后可能条件立刻又不满足了，防止假唤醒
    while (que.empty()){
        if (!b_cond.wait(b_lock.get())){ // 调用失败
            b_lock.unlock();
            return false;
        }
    }

    item = que.front();
    que.pop();

    b_lock.unlock();
    return true;
}