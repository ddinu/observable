project(observable)
cmake_minimum_required(VERSION 3.5)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(SOURCE_FILES include/observable/subject.hpp
                 include/observable/detail/copy_policy.hpp
                 include/observable/detail/function_collection.hpp
                 include/observable/detail/function_traits.hpp
                 include/observable/detail/handle.hpp)

source_group(include REGULAR_EXPRESSION include/.*)
source_group(include\\detail REGULAR_EXPRESSION include/.*/detail/.*)
source_group(include\\detail\\subject REGULAR_EXPRESSION include/.*/detail/subject/.*)

add_custom_target(observable SOURCES ${SOURCE_FILES})
include_directories(include)

add_subdirectory(external)
add_subdirectory(test)