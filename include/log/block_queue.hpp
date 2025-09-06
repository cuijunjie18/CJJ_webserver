#ifndef BLOCK_QUEUE_HPP
#define BLOCK_QUEUE_HPP

#include <queue>
#include <lock/locker.hpp>
#include <string.h>
#include <string>

class BlockQueue
{
public:
    BlockQueue(int maxn = 100000);
    ~BlockQueue();
    void clear();
    bool full();
    bool empty();
    std::string front();
    int size();
    int max_size();
    bool push(std::string &item); // 核心，生产者写日志到缓冲区
    bool pop(std::string &item); // 消费者取缓冲区

private:
    std::queue<std::string> que; // 队列里的内容是字符串
    locker b_lock;
    cond b_cond;
    int maxn_size;
}; 

#endif