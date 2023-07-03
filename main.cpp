#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>
#include <vector>

#include "includes/ThreadDispatcher.hpp"


int DelayedTask (int i) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for (1s);
    std::cout << i << " delayed task over" << std::endl;
    return i;
}


void DelayedTaskVoid () {
    using namespace std::chrono_literals;
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for (1s);
        std::cout << "sleep #" << i << std::endl;
    }
}


int main ()
{
    using ResultT = ThreadPool::ThreadResult<int>;

    ThreadPool::ThreadDispatcher<5> dispatcher;
    std::vector<ResultT> results;
    for (int i = 0; i < 5; ++i) {
        std::function<int (int)> func = [] (int param) { return DelayedTask (param); };
        results.push_back (dispatcher.QueueTask (std::move (func), i));
    }

    ThreadPool::ThreadResult<void> result = dispatcher.QueueTask (std::function<void ()> { DelayedTaskVoid });
    result.GetFuture ().wait ();

    for (const ResultT& result : results)
        std::cout << result.GetFuture ().get () << std::endl;
    return 0;
}
