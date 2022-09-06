#include <iostream>
#include <type_traits>

#include "includes/ThreadDispatcher.hpp"

int main ()
{
    ThreadPool::ThreadDispatcher dispatcher;
    auto task = [] (int a, int b) -> int {
        return a + b;
    };
    int x = 0; 
    auto voidTask = [&] (int y) -> void {
        x = y;
    };
    std::future<int> intResult = dispatcher.QueueTask (std::move (task), 12, 35);
    std::future<void> voidResult = dispatcher.QueueTask (std::move (voidTask), 3);
    dispatcher.ExecuteTasks ();
    voidResult.wait ();
    std::cout << x << std::endl;
    std::cout << intResult.get () << std::endl;
    return 0;
}