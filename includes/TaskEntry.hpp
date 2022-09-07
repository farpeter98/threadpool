#ifndef TASKENTRY_HPP
#define TASKENTRY_HPP

#include <functional>
 

namespace ThreadPool {

struct TaskEntryBase {
    std::function<void ()> task;
    virtual ~TaskEntryBase () = default;
};

template<typename ResultType>
struct TaskEntry : public TaskEntryBase  {
    std::promise<ResultType> promise;
    virtual ~TaskEntry () override = default;
};

} // namespace ThreadPool


#endif // TASKENTRY_HPP
