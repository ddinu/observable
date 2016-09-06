project(observable)
cmake_minimum_required(VERSION 3.5)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(source_groups)

set(SOURCE_FILES include/observable/subject.hpp
                 include/observable/detail/function_collection.hpp
                 include/observable/detail/function_traits.hpp
                 include/observable/detail/handle.hpp
                 include/observable/detail/tagged_function.hpp)

add_custom_target(observable SOURCES ${SOURCE_FILES})

add_subdirectory(external)
add_subdirectory(test)
add_subdirectory(benchmark)