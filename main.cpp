#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>
#include <vector>

#include "includes/ThreadDispatcher.hpp"


void DelayedTask (int i) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for (1s);
    std::cout << i << " delayed task over" << std::endl;
}


int main ()
{
    ThreadPool::ThreadDispatcher<4> dispatcher;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        std::function<void ()> func = [i] () { DelayedTask (i); };
        futures.push_back (dispatcher.QueueTask (std::move (func)));
    }
    for (const std::future<void>& f : futures)
        f.wait ();
    return 0;
}