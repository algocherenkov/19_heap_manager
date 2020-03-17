#include "user_allocator.h"
#include "user_container.h"
#include <string>
#include <ctime>
#include <chrono>
#include <random>
#include <iterator>
#include <map>

#define BOOST_TEST_MODULE test_main

#include <boost/test/included/unit_test.hpp>

using namespace boost::unit_test;
BOOST_AUTO_TEST_SUITE(test_suite_main)

//std::tgamma(arg) - is a factorial of arg - 1
constexpr std::size_t CHUNK_SIZE = 10;

//let's compare std::allocator (standard heap manager) and my allocator UserAllocator (my heap manager)
BOOST_AUTO_TEST_CASE(my_allocator_as_a_heap_manager_test)
{
    auto start = std::chrono::high_resolution_clock::now();

    // unsync the I/O of C and C++.
    std::ios_base::sync_with_stdio(false);

    //map with the standard allocator by default
    std::map<int, int> myMapStdAlloc;
    for(int i = 0; i < 10; i++)
        myMapStdAlloc.emplace(i, tgamma(i + 1));

    auto end = std::chrono::high_resolution_clock::now();

    // Calculating total time taken by the BM algorithm.
    double time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    std::cout << "time of work of std::allocator during of 10 emplacements into map: " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
    std::ios_base::sync_with_stdio(true);

    // it's around 0.000057 sec
    //========================================================================================================================

    start = std::chrono::high_resolution_clock::now();

    // unsync the I/O of C and C++.
    std::ios_base::sync_with_stdio(false);

    //map with my allocator. Here is function less<int> which uses for compare elements. And here is my allocator.
    //Since the map's element is std::pair my allocator stores std::pair<int, int>. Also chunk size was mentioned
    std::map<int, int, std::less<int>, UserAllocator<std::pair<int,int>, CHUNK_SIZE>> MapUserAlloc;
    for(int i = 0; i < 10; i++)
        MapUserAlloc.emplace(i, tgamma(i + 1));

    end = std::chrono::high_resolution_clock::now();

    // Calculating total time taken by the BM algorithm.
    time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    std::cout << "time of work of user_allocator during of 10 emplacements into map: " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
    std::ios_base::sync_with_stdio(true);

    //it's around 0.000006617 sec
    //=======================================================================================================================

    BOOST_CHECK_MESSAGE(MapUserAlloc[0] == 1, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[1] == 1, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[2] == 2, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[3] == 6, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[4] == 24, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[5] == 120, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[6] == 719, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[7] == 5039, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[8] == 40319, "wrong value");
    BOOST_CHECK_MESSAGE(MapUserAlloc[9] == 362879, "wrong value");

    //=======================================================================================================================
    start = std::chrono::high_resolution_clock::now();

    // unsync the I/O of C and C++.
    std::ios_base::sync_with_stdio(false);

    UserContainer<int> myContainerStdAlloc;
    for(int i = 0; i < 10; i++)
        myContainerStdAlloc.emplace_back(i);

    end = std::chrono::high_resolution_clock::now();

    // Calculating total time taken by the BM algorithm.
    time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    std::cout << "time of work of std::allocator during of 10 emplacements into user allocator: " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
    std::ios_base::sync_with_stdio(true);

    //it's around 0.000004 sec
    //=======================================================================================================================

    start = std::chrono::high_resolution_clock::now();

    // unsync the I/O of C and C++.
    std::ios_base::sync_with_stdio(false);

    UserContainer<int, UserAllocator<int, CHUNK_SIZE>> myContainerUserAlloc;
    for(int i = 0; i < 10; i++)
        myContainerUserAlloc.emplace_back(i);

    end = std::chrono::high_resolution_clock::now();

    // Calculating total time taken by the BM algorithm.
    time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    std::cout << "time of work of user_allocator of 10 emplacements into user allocator: " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
    std::ios_base::sync_with_stdio(true);

    //it's around 0.000003200 sec
    //=======================================================================================================================

    int k = 0;
    for(auto i = myContainerUserAlloc.begin(); i != myContainerUserAlloc.end(); i++, k++)
        BOOST_CHECK_MESSAGE(*i == k, "wrong result size");
}
//                                      time, sec
//std::map with std::allocator          0.000057
//std::map with user allocator          0.000006617
//user container with std::allocator    0.000004
//user container with user allocator    0.0000032

//User allocator with storage of chunks shows results which is better than standard allocator
//user container with user allocator is slightly better than the same container with std::allocator

BOOST_AUTO_TEST_SUITE_END()
