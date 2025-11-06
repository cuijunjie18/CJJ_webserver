#pragma once
#include "utils/em_logging.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <vector>
namespace obf {
namespace em {
class ThreadPool {
public:
  explicit ThreadPool(size_t threads) {
    workers_.reserve(threads);
    for (size_t i = 0; i < threads; ++i) {
      workers_.emplace_back([this] {
        for (;;) {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            this->condition_.wait(
                lock, [this] { return this->stop_ || !this->tasks_.empty(); });
            if (this->stop_ && this->tasks_.empty()) {
              return;
            }
            task = std::move(this->tasks_.front());
            this->tasks_.pop();
          }
          try {
            task();
          } catch (...) {
            EM_LOG(ERROR, "task execute throw exception");
          }

          {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            if (tasks_.empty()) {
              condition_.notify_all();
            }
          }
        }
      });
    }
  }

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&... args)
      -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      // don't allow enqueueing after stop_ping the pool
      if (stop_) {
        EM_LOG(WARN, "enqueue on stop_ped ThreadPool");
        return res;
      }

      tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_all();
    return res;
  }
  ~ThreadPool() {
    wait_for_tasks();
    shutdown();
  }

private:
  // 添加用于等待所有任务完成的方法
  void wait_for_tasks() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    condition_.wait(lock, [this] { return tasks_.empty(); });
  }

  // 添加用于立即停止线程池并等待所有线程结束的方法
  void shutdown() {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      stop_ = true;
    }
    condition_.notify_all();
    for (std::thread &worker : workers_) {
      worker.join();
    }
    workers_.clear(); // 清空工作线程列表，确保析构函数不会再次尝试加入它们
  }

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::atomic<bool> stop_{false};
};
} // namespace em
} // namespace obf