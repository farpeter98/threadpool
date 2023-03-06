#ifndef THREADDISPATCHER_HPP
#define THREADDISPATCHER_HPP

#include <array>
#include <deque>
#include <memory>

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
        : dispatcherThread  (&ThreadDispatcher::DispatcherTask, this)
        , isRunning         (true)
    {}


    ~ThreadDispatcher ()
    {
        isRunning = false;
        dispatcherThread.join ();
    }


    template<typename TaskType, typename... Args>
    auto QueueTask (TaskType&& task, Args&&... args)
    {
        using ResultType    = decltype (task (args...));
        using EntryType     = TaskEntry<ResultType>;

        std::shared_ptr<EntryType> entry = std::make_shared<EntryType> ();
        auto voidTask = [task = std::move (task), &args..., entry] () -> void {
            if constexpr (std::is_void_v <ResultType>) {
                task (std::forward <Args> (args)...);
                entry->promise.set_value ();
            }
            else {
                entry->promise.set_value (task (std::forward <Args> (args)...));
            }
        };
        entry->task = std::move (voidTask);
        tasks.emplace_back (entry);
        return ThreadResult<ResultType> { entry };
    }

private:
    std::array<PooledThread, poolSize>          threadPool;
    std::deque<std::shared_ptr<TaskEntryBase>>  tasks;
    std::thread                                 dispatcherThread;
    bool                                        isRunning;


    void DispatcherTask ()
    {
        using namespace std::chrono_literals;

        while (isRunning) {
            if (tasks.empty ()) {
                std::this_thread::sleep_for (10ms);
                continue;
            }

            typename decltype (tasks)::iterator it = tasks.begin ();
            typename decltype (tasks)::iterator end = tasks.end ();
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
