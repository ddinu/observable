project(observable)
cmake_minimum_required(VERSION 3.0)
enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(default_source_groups)

set(SOURCE_FILES include/observable/observable.hpp
                 include/observable/observe.hpp
                 include/observable/subject.hpp
                 include/observable/subscription.hpp
                 include/observable/value.hpp
                 include/observable/expressions/expression.hpp
                 include/observable/expressions/filters.hpp
                 include/observable/expressions/math.hpp
                 include/observable/expressions/operators.hpp
                 include/observable/expressions/tree.hpp
                 include/observable/expressions/utility.hpp
                 include/observable/detail/collection.hpp
                 include/observable/detail/type_traits.hpp)

add_library(observable INTERFACE)
target_include_directories(observable INTERFACE include)

# Dummy target to get a project in IDEs.
add_custom_target(observable_headers SOURCES ${SOURCE_FILES})

add_subdirectory(external)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(docs)
add_subdirectory(examples)
