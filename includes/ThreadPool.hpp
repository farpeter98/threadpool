#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <array>
#include <cstddef>
#include <thread>

#include "ThreadEntry.hpp"


namespace ThreadPool {

template<std::size_t ThreadCount>
class ThreadPool {
public:
    ThreadPool () = default;
    ~ThreadPool () {
        for (const ThreadEntry& entry) {
            entry.doRun = false;
            entry.thread.join ();
        }
    }

private:
    std::array<ThreadEntry, ThreadCount> threads;
};

} // namespace ThreadPool


#endif // THREADPOOL_HPP
