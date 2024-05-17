cmake_minimum_required(VERSION 3.10)
project(cpo)

find_package(Boost REQUIRED COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(multi_thread_mutex src/multi_thread_mutex.cpp)
add_executable(multi_thread_shared_mutex src/multi_thread_shared_mutex.cpp)
add_executable(producer-consumer src/producer-consumer.cpp)
add_executable(Asynchronous_programming src/Asynchronous_programming.cpp)
add_executable(lockfree src/lockfree.cpp)


target_link_libraries(
    multi_thread_mutex -lpthread
)
target_link_libraries(
    multi_thread_shared_mutex -lpthread
)

target_link_libraries(
    multi_thread_shared_mutex ${Boost_LIBRARIES}
)

target_link_libraries(
    producer-consumer -lpthread
)

target_link_libraries(
    Asynchronous_programming -lpthread
)
