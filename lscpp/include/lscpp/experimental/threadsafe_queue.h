#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <stop_token>

template <class T> class threadsafe_queue {
  mutable std::mutex m;
  std::queue<T> data;
  std::condition_variable_any cond;

public:
  void push(T const &obj) {
    std::lock_guard lock(m);
    data.push(std::move(obj));
    cond.notify_one();
  }
  std::optional<T> wait_and_pop(std::stop_token s) {
    std::unique_lock lock(m);
    cond.wait(lock, s, [this]() { return !data.empty(); });
    if (s.stop_requested())
      return {};
    auto res = data.front();
    data.pop();
    return res;
  }
};