#include "../includes/ThreadEntry.hpp"

#include <chrono>


namespace ThreadPool {


ThreadEntry::ThreadEntry ()
    : doRun     (true)
    , thread    (&ThreadEntry::ThreadTask, this)
{}


void ThreadEntry::AddTask (std::function<void ()>&& func) {
    using namespace std::chrono_literals;

    task.emplace (std::move (func));
    cv.notify_one ();
    std::this_thread::sleep_for (1ms);
    mutex.lock ();
    mutex.unlock ();
}


void ThreadEntry::ThreadTask () {
    std::unique_lock<std::mutex> lock (mutex);
    while (doRun) {
        cv.wait (lock, [this] { return task.has_value (); });
        std::invoke (*task);
        task.reset ();
    }
}


} // namespace ThreadPool
