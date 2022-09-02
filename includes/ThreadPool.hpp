#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <future>
#include <memory>
#include <list>


namespace ThreadPool {

class ThreadPool {
public:
    template<typename TaskType, typename... Args>
    auto QueueTask (TaskType&& task, Args&&... args) {
        using ResultType = decltype (task (args...));
        using PromiseType = std::promise<ResultType>;

        std::shared_ptr<PromiseType> promise = std::make_shared<PromiseType> ();
        tasks.emplace_back ([task = std::move (task), &args..., promise] () -> void {
            if constexpr (std::is_void_v <ResultType>) {
                task (std::forward <Args> (args)...);
                promise->set_value ();
            }
            else {
                promise->set_value (task (std::forward <Args> (args)...));
            }
        });
        return promise;
    }


    void ExecuteTasks () {
        for (const std::function<void ()>& task : tasks) {
            task ();
        }
    }


private:
    std::list<std::function<void ()>> tasks;
};

} // namespace ThreadPool


#endif // THREADPOOL_H
