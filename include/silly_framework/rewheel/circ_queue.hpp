#pragma once
#ifndef __SFLIB_CIRC_QUEUE__
#define __SFLIB_CIRC_QUEUE__

#include <new>
#include <memory>
#include <utility>
#include <cstddef>
#include <optional>

#include <mutex>
#include <condition_variable>

namespace silly_framework::rewheel {

/**
 * 只分配内存，不执行构造函数，析构时自动释放。
 * 居家造轮子必备良药（大雾）。
 */
template <typename T>
struct noinit_arr {
    unsigned char* data = nullptr; size_t size = 0;
    noinit_arr(size_t n) : size(n) {
        if (n) { data = new alignas(T) unsigned char[(n * sizeof(T))]; }
    }
    ~noinit_arr() { if (data) { delete data; } }
    noinit_arr(const noinit_arr&) = delete;
    noinit_arr(noinit_arr&& other):
        data(std::exchange(other.data, nullptr)), size(std::exchange(other.size, 0)) {}
    T* ptr() { return reinterpret_cast<T*>(data); }
    T* ptr(size_t idx) { return ptr() + idx; }
    T& operator[](size_t idx) { return *std::launder(ptr(idx)); }
};

/**
 * 环形队列。使用数组实现，容量固定，push和pop操作均为O(1)。
 */
template <typename T>
class circ_queue {
protected:
    noinit_arr<T> data;
    size_t start = 0, length = 0;

    T* ptr(size_t idx) { return data.ptr((start + idx) % data.size); }

public:
    circ_queue(size_t capacity) : data(capacity) {}
    circ_queue(const circ_queue<T>&) = delete;
    circ_queue(circ_queue<T>&&) = default;
    ~circ_queue() { while (!empty()) { pop(); } }

    T& operator[](size_t idx) { return data[(start + idx) % data.size]; }
    size_t size() { return length; }
    size_t capacity() { return data.size; }
    T& front() { return (*this)[0]; }
    T& back() { return (*this)[length - 1]; }
    bool empty() { return size() == 0; }
    bool full() { return size() == capacity(); }

    // 入队，不检查队列是否已满。
    template <typename... ArgTs>
    T& emplace(ArgTs&&... args) {
        auto result = std::construct_at(ptr(length), std::forward<ArgTs>(args)...);
        ++length; return *result;
    }

    // 出队，不检查队列是否为空。
    T pop() {
        auto& head = front();
        T rescue = std::move(head);
        std::destroy_at(&head);
        start = (start + 1) % capacity();
        --length; return rescue; // NRVO
    }

    template <typename... ArgTs>
    T* try_emplace(ArgTs&&... args) {
        return full() ? nullptr : &emplace(std::forward<ArgTs>(args)...);
    }

    std::optional<T> try_pop() {
        if (empty()) { return std::nullopt; } else { return pop(); }
    }
};

/**
 * 线程安全队列。API名称参考Python的Queue。
 */
template <typename T>
class concurrent_queue {
protected:
    circ_queue<T> cqueue;
    std::mutex lock;
    std::condition_variable cvar;

    template <typename... ArgTs>
    T& notify_put(ArgTs&&... args) {
        auto& result = cqueue.emplace(std::forward<ArgTs>(args)...);
        cvar.notify_one();
        return result;
    }

    T notify_get() {
        auto result = cqueue.pop();
        cvar.notify_one();
        return result;
    }

public:
    concurrent_queue(size_t cap) : cqueue(cap) {}
    concurrent_queue(const concurrent_queue<T>&) = delete;
    concurrent_queue(concurrent_queue<T>&&) = default;
    ~concurrent_queue() {}


    size_t size() { return cqueue.size(); }
    size_t capacity() { return cqueue.capacity(); }
    bool empty() { return size() == 0; }
    bool full() { return size() == capacity(); }

    template <typename... ArgTs>
    T& put(ArgTs&&... args) {
        std::unique_lock guard(lock);
        cvar.wait(guard, [this] { return !full(); });
        return notify_put(std::forward<ArgTs>(args)...);
    }

    T get() {
        std::unique_lock guard(lock);
        cvar.wait(guard, [this] { return !empty(); });
        return notify_get();
    }

    template <typename Clock, typename Duration, typename... ArgTs>
    T* put_until(const std::chrono::time_point<Clock, Duration>& abs_time, ArgTs&&... args) {
        std::unique_lock guard(lock);
        bool fulfilled = cvar.wait_until(guard, abs_time, [this] { return !full(); });
        if (!fulfilled) { return nullptr; }
        return &notify_put(std::forward<ArgTs>(args)...);
    }

    template <typename Clock, typename Duration>
    std::optional<T> get_until(const std::chrono::time_point<Clock, Duration>& abs_time) {
        std::unique_lock guard(lock);
        bool fulfilled = cvar.wait_until(guard, abs_time, [this] { return !empty(); });
        if (!fulfilled) { return std::nullopt; }
        return notify_get();
    }

    template <typename Rep, typename Period, typename... ArgTs>
    T* put_timeout(const std::chrono::duration<Rep, Period>& rel_time, ArgTs&&... args) {
        std::unique_lock guard(lock);
        bool fulfilled = cvar.wait_for(guard, rel_time, [this] { return !full(); });
        if (!fulfilled) { return nullptr; }
        return &notify_put(std::forward<ArgTs>(args)...);
    }

    template <typename Rep, typename Period>
    std::optional<T> get_timeout(const std::chrono::duration<Rep, Period>& rel_time) {
        std::unique_lock guard(lock);
        bool fulfilled = cvar.wait_for(guard, rel_time, [this] { return !empty(); });
        if (!fulfilled) { return std::nullopt; }
        return notify_get();
    }

    template <typename... ArgTs>
    T* put_nowait(ArgTs&&... args) {
        std::unique_lock guard(lock);
        if (full()) { return nullptr; }
        return &notify_put(std::forward<ArgTs>(args)...);
    }

    std::optional<T> get_nowait() {
        std::unique_lock guard(lock);
        if (empty()) { return std::nullopt; }
        return notify_get();
    }
};

} // namespace silly_framework::rewheel

#endif // __SFLIB_CIRC_QUEUE__  
