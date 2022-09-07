#ifndef THREADDISPATCHER_HPP
#define THREADDISPATCHER_HPP

#include <array>
#include <future>
#include <list>
#include <memory>

#include "PooledThread.hpp"
#include "TaskEntry.hpp"


namespace ThreadPool {

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
        return entry->promise.get_future ();
    }

private:
    std::array<PooledThread, poolSize>          threadPool;
    std::list<std::shared_ptr<TaskEntryBase>>   tasks;
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

            decltype (tasks)::iterator it = tasks.begin ();
            decltype (tasks)::iterator end = tasks.end ();
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
