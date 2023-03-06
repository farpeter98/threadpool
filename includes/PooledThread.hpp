#ifndef POOLEDTHREAD_HPP
#define POOLEDTHREAD_HPP

#include <chrono>
#include <condition_variable>
#include <functional>
#include <optional>
#include <mutex>
#include <thread>


namespace ThreadPool {

class PooledThread {
    using MaybeTask = std::optional<std::function<void ()>>;

    std::condition_variable cv;
    bool                    doRun;
    MaybeTask               task;
    std::thread             thread;

public:
    PooledThread ()
        : doRun     (true)
        , thread    (&PooledThread::ThreadTask, this)
    {}


    ~PooledThread ()
    {
        doRun = false;
        AddTask ([] () { return; });
        thread.join ();
    }


    inline bool IsFree () const
    {
        return !task.has_value ();
    }


    inline std::thread::id GetId () const
    {
        return thread.get_id ();
    }


    void AddTask (std::function<void ()>&& func)
    {
        using namespace std::chrono_literals;

        while (true) {
            if (task.has_value ()) {
                std::this_thread::sleep_for (10ms);
                continue;
            }

            task.emplace (std::move (func));
            cv.notify_one ();
            return;
        }
    }

private:
    void ThreadTask ()
    {
        std::mutex m;
        std::unique_lock<std::mutex> lock (m);
        while (doRun) {
            cv.wait (lock, [this] { return task.has_value (); });
            std::invoke (*task);
            task.reset ();
        }
    }
};

}; // namespace ThreadPool


#endif // POOLEDTHREAD_HPP
