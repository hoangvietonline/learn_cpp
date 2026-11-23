#ifndef SDP_THREAD_SAFE_QUEUE_H
#define SDP_THREAD_SAFE_QUEUE_H

#include "LinkedQueue.h"
#include <condition_variable>
#include <mutex>

namespace sdp
{
    template <typename T>
    class ThreadSafeQueue
    {
        ds::LinkedQueue<T> queue;
        std::mutex mutex;
        std::condition_variable ready;
        bool closed = false;

    public:
        void push(T value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (closed) throw Error("Queue is closed");
                queue.push(std::move(value));
            }
            ready.notify_one();
        }

        bool pop(T &value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            ready.wait(lock, [this] { return closed || !queue.empty(); });
            if (queue.empty()) return false;
            value = queue.pop();
            return true;
        }

        void close()
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                closed = true;
            }
            ready.notify_all();
        }
    };
}
#endif
