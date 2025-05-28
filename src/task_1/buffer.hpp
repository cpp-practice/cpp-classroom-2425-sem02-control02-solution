#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <thread>

namespace control_02 {

class ExecutionQueue {
public:
  ExecutionQueue() {
    thread_ = std::thread{[this]() { spin(); }};
  }

  void post(const std::function<void()> &f) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(f);
    notifier_.notify_one();
  }

  ~ExecutionQueue() {
    stopped_ = true;
    notifier_.notify_all();
    thread_.join();
  }

private:
  void spin() {
    while (true) {
      // stopped
      std::function<void()> func;

      {
        std::unique_lock<std::mutex> queue_lock(mutex_);
        notifier_.wait(queue_lock,
                       [this]() { return !queue_.empty() || stopped_; });
        if (stopped_ && queue_.empty())
          return;
        func = queue_.front();
        queue_.pop_front();
      }
      func();
    }
  }

  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable notifier_;
  std::list<std::function<void()>> queue_;
  bool stopped_ = false;
};

class Buffer : public std::enable_shared_from_this<Buffer> {
public:
  static std::shared_ptr<Buffer>
  make_buffer(const std::optional<size_t> &max_queue_size = std::nullopt) {
    return std::shared_ptr<Buffer>{new Buffer{max_queue_size}};
  }

  void post(const std::function<void()> &f) {
    std::weak_ptr<Buffer> buf = weak_from_this();
    std::lock_guard<std::mutex> lock(queue_mutex_);
    queue_.emplace_back([buf, f]() {
      f();
      if (auto sp = buf.lock()) {
        std::lock_guard<std::mutex> lock(sp->queue_mutex_);
        sp->has_running_task_ = false;
        sp->try_send_to_execution_queue();
      }
    });
    if (max_queue_size_ && queue_.size() > *max_queue_size_) {
      queue_.pop_front();
    }
    try_send_to_execution_queue();
  }

  void move_to_execution_queue(
      const std::shared_ptr<ExecutionQueue> &execution_queue) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    execution_queue_ = execution_queue;
    try_send_to_execution_queue();
  }

  size_t queue_size() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return queue_.size();
  }

private:
  explicit Buffer(const std::optional<size_t> &max_queue_size)
      : max_queue_size_{max_queue_size} {}

  void try_send_to_execution_queue() {
    // Must take the lock before calling this method
    if (!has_running_task_ && execution_queue_ && !queue_.empty()) {
      auto task = queue_.front();
      queue_.pop_front();
      execution_queue_->post(task);
      has_running_task_ = true;
    }
  }

  std::shared_ptr<ExecutionQueue> execution_queue_;
  bool has_running_task_ = false;
  std::list<std::function<void()>> queue_;
  std::optional<size_t> max_queue_size_;
  mutable std::mutex queue_mutex_;
};

} // namespace control_02