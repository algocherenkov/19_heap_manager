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
    //map with the standard allocator by default
    std::map<int, int> myMapStdAlloc;
    for(int i = 0; i < 10; i++)
        myMapStdAlloc.emplace(i, tgamma(i + 1));

    //map with my allocator. Here is function less<int> which uses for compare elements. And here is my allocator.
    //Since the map's element is std::pair my allocator stores std::pair<int, int>. Also chunk size was mentioned
    std::map<int, int, std::less<int>, UserAllocator<std::pair<int,int>, CHUNK_SIZE>> MapUserAlloc;
    for(int i = 0; i < 10; i++)
        MapUserAlloc.emplace(i, tgamma(i + 1));

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


    UserContainer<int> myContainerStdAlloc;
    for(int i = 0; i < 10; i++)
        myContainerStdAlloc.emplace_back(i);

    UserContainer<int, UserAllocator<int, CHUNK_SIZE>> myContainerUserAlloc;
    for(int i = 0; i < 10; i++)
        myContainerUserAlloc.emplace_back(i);

    int k = 0;
    for(auto i = myContainerUserAlloc.begin(); i != myContainerUserAlloc.end(); i++, k++)
        BOOST_CHECK_MESSAGE(*i == k, "wrong result size");
}
BOOST_AUTO_TEST_SUITE_END()
