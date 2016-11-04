project(observable)
cmake_minimum_required(VERSION 3.0)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(source_groups)

set(SOURCE_FILES include/observable/compound_subject.hpp
                 include/observable/property.hpp
                 include/observable/subject.hpp
                 include/observable/detail/callable_collection.hpp
                 include/observable/detail/compound_subject_utility.hpp
                 include/observable/detail/subscription.hpp)

add_custom_target(observable SOURCES ${SOURCE_FILES})

add_subdirectory(external)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(docs)
