project(observable)
cmake_minimum_required(VERSION 3.0)
enable_testing()

set(SOURCE_FILES #include/observable/observer.hpp
                 include/observable/subject.hpp
                 include/observable/detail/subject/copy_policy.hpp
                 include/observable/detail/subject/data.hpp
                 include/observable/detail/subject/subscription.hpp
                 include/observable/detail/function_collection.hpp
                 include/observable/detail/function_handle.hpp
                 include/observable/detail/function_traits.hpp)

source_group(include REGULAR_EXPRESSION .*\\.hpp)
source_group(include\\detail REGULAR_EXPRESSION .*/detail/.*\\.hpp)
source_group(include\\detail\\subject REGULAR_EXPRESSION .*/detail/subject/.*\\.hpp)

add_custom_target(observable SOURCES ${SOURCE_FILES})
include_directories(include)

add_subdirectory(external)
add_subdirectory(test)