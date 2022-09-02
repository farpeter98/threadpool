#include <iostream>
#include <type_traits>

#include "includes/ThreadPool.hpp"

int main ()
{
    ThreadPool::ThreadPool pool;
    auto task = [] (int a, int b) -> int {
        return a + b;
    };
    int x = 0; 
    auto voidTask = [&] (int y) -> void {
        x = y;
    };
    std::future<int> intResult = pool.QueueTask (std::move (task), 12, 35);
    std::future<void> voidResult = pool.QueueTask (std::move (voidTask), 3);
    pool.ExecuteTasks ();
    voidResult.wait ();
    std::cout << x << std::endl;
    std::cout << intResult.get () << std::endl;
    return 0;
}