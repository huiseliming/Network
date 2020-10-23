#pragma once

template<typename T>
class ThreadSafeDeque;

template<typename T>
using TSQueue = ThreadSafeDeque<T>;

template<typename T>
class ThreadSafeDeque
{
public:
    ThreadSafeDeque() = default;
    ThreadSafeDeque(const ThreadSafeDeque&) = delete;
    ThreadSafeDeque& operator=(const ThreadSafeDeque&) = delete;
    virtual ~ThreadSafeDeque() { m_deque.clear(); }

    const T& front()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.front();
    }

    const T& back()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.back();
    }

    void push_front(const T& item)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.push_front(item);
    }

    void push_back(const T& item)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.push_back(item);
    }

    void emplace_front(T&& item)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.emplace_front(std::forward<T>(item));
    }

    void emplace_back(T&& item)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.emplace_back(std::forward<T>(item));
    }

    bool empty()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.empty();
    }

    size_t size()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.size();
    }

    void clear()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_deque.clear();
    }

    T pop_front()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        auto front = std::move(m_deque.front());
        m_deque.pop_front();
        return front;
    }

    T pop_back()
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        auto back = std::move(m_deque.back());
        m_deque.pop_back();
        return back;
    }

protected:
    std::deque<T> m_deque;
    std::mutex m_mutex;
};






