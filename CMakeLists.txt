cmake_minimum_required(VERSION 3.15)
project(The_State_Of_The_Art_Linked_List)

set(CMAKE_CXX_STANDARD 98)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
find_package(Threads)

add_executable(The_State_Of_The_Art_Linked_List main.cpp linked_list.h unrolled_node.h
        external-sources/concurrency_factory.cpp external-sources/concurrency_objects.cpp)


target_link_libraries(The_State_Of_The_Art_Linked_List gtest gtest_main pthread)
target_link_libraries(The_State_Of_The_Art_Linked_List ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(The_State_Of_The_Art_Linked_List rt)