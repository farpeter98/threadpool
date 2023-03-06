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


int main ()
{
    using ResultT = ThreadPool::ThreadResult<int>;

    ThreadPool::ThreadDispatcher<4> dispatcher;
    std::vector<ResultT> results;
    for (int i = 0; i < 10; ++i) {
        std::function<int ()> func = [i] () { return DelayedTask (i); };
        results.push_back (dispatcher.QueueTask (std::move (func)));
    }
    for (const ResultT& result : results)
        std::cout << result.GetFuture ().get () << std::endl;
    return 0;
}
