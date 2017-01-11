project(observable)
cmake_minimum_required(VERSION 3.0)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(default_source_groups)

set(SOURCE_FILES include/observable/subject.hpp
                 include/observable/subscription.hpp
                 include/observable/value.hpp
                 include/observable/detail/collection.hpp
                 include/observable/detail/expression.hpp
                 include/observable/detail/expression_tree.hpp
                 include/observable/detail/type_traits.hpp)

source_group(include\\expression REGULAR_EXPRESSION .*/expression/.*)

add_custom_target(observable SOURCES ${SOURCE_FILES})

add_subdirectory(external)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(docs)
