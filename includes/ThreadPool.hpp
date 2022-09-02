#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <future>
#include <list>
#include <memory>


namespace ThreadPool {

class ThreadPool {
    struct TaskEntryBase {
        std::function<void ()> task;
    };

    template<typename ResultType>
    struct TaskEntry : public TaskEntryBase  {
        std::promise<ResultType> promise;
    };

public:
    template<typename TaskType, typename... Args>
    auto QueueTask (TaskType&& task, Args&&... args) {
        using ResultType    = decltype (task (args...));
        using PromiseType   = std::promise<ResultType>;
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


    void ExecuteTasks () {
        for (const std::shared_ptr<TaskEntryBase>& entry : tasks) {
            entry->task ();
        }
    }


private:
    std::list<std::shared_ptr<TaskEntryBase>> tasks;
};

} // namespace ThreadPool


#endif // THREADPOOL_H
