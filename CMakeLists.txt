project(observable)
cmake_minimum_required(VERSION 3.0)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(default_source_groups)

set(SOURCE_FILES include/observable/observable.hpp
                 include/observable/observe.hpp
                 include/observable/subject.hpp
                 include/observable/subscription.hpp
                 include/observable/value.hpp
                 include/observable/expression/expression.hpp
                 include/observable/expression/operators.hpp
                 include/observable/expression/tree.hpp
                 include/observable/expression/utility.hpp
                 include/observable/detail/collection.hpp
                 include/observable/detail/type_traits.hpp)

add_custom_target(observable SOURCES ${SOURCE_FILES})

add_subdirectory(external)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(docs)
