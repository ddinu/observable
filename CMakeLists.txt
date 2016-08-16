project(observable)
cmake_minimum_required(VERSION 3.0)
enable_testing()

set(SOURCE_FILES include/observable/observable.hpp
                 include/observable/observer.hpp)

add_custom_target(observable SOURCES ${SOURCE_FILES})
include_directories(include)

add_subdirectory(external)
add_subdirectory(test)