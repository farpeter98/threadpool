#ifndef THREADENTRY_HPP
#define THREADENTRY_HPP

#include <condition_variable>
#include <functional>
#include <optional>
#include <mutex>
#include <thread>


namespace ThreadPool {

struct ThreadEntry {
    using Task = std::optional<std::function<void ()>>;

    std::condition_variable cv;
    std::mutex              mutex;
    volatile bool           doRun;
    Task                    task;
    std::thread             thread;

    ThreadEntry ();

    void AddTask (std::function<void ()>&& func);
    void ThreadTask ();
};

};


#endif // THREADENTRY_HPP
