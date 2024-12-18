#ifndef THREADDISPATCHER_HPP
#define THREADDISPATCHER_HPP

#include <array>
#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>

#include "PooledThread.hpp"
#include "TaskEntry.hpp"


namespace ThreadPool {


template<typename ResultType>
class ThreadResult {
private:
    std::shared_ptr<TaskEntry<ResultType>> taskEntry;

public:
    ThreadResult (const std::shared_ptr<TaskEntry<ResultType>>& taskEntry)
        : taskEntry { taskEntry }
    {}

    std::future<ResultType> GetFuture () const
    {
        return taskEntry->promise.get_future ();
    }
};

template<std::size_t poolSize>
class ThreadDispatcher {
public:
    ThreadDispatcher ()
        : isRunning         (true)
        , dispatcherThread  (&ThreadDispatcher::DispatcherTask, this)
    {}


    ~ThreadDispatcher ()
    {
        isRunning = false;
        dispatcherThread.join ();
    }


    template<typename ReturnType, typename... Ts, typename... Args>
    ThreadResult<ReturnType> QueueTask (std::function<ReturnType (Ts...)>&& task, Args&&... args)
    {
        using EntryType = TaskEntry<ReturnType>;

        std::shared_ptr<EntryType> entry = std::make_shared<EntryType> ();
        std::function<void ()> voidTask = [task = std::move (task), &args..., entry] () -> void {
            if constexpr (std::is_void_v <ReturnType>) {
                task (std::forward <Args> (args)...);
                entry->promise.set_value ();
            }
            else {
                entry->promise.set_value (task (std::forward <Args> (args)...));
            }
        };
        entry->task = std::move (voidTask);
        tasks.emplace_back (entry);
        return { entry };
    }

private:
    std::deque<std::shared_ptr<TaskEntryBase>>  tasks;
    // declare isRunning before dispatcherThread so that they can be initialized in the member initializer list
    std::atomic_bool                            isRunning;
    std::thread                                 dispatcherThread;
    std::array<PooledThread, poolSize>          threadPool;


    void DispatcherTask ()
    {
        using namespace std::chrono_literals;

        while (isRunning) {
            if (tasks.empty ()) {
                std::this_thread::sleep_for (10ms);
                continue;
            }

            auto it = tasks.begin ();
            const auto end = tasks.end ();
            while (it != end) {
                for (PooledThread& thread : threadPool) {
                    if (it == end)
                        break;

                    if (thread.IsFree ()) {
                        std::this_thread::sleep_for (10ms);
                        thread.AddTask (std::move ((*it)->task));
                        ++it;
                        tasks.pop_front ();
                    }
                }
            }
        }
    }
};

} // namespace ThreadPool

#endif // THREADDISPATCHER_HPP
