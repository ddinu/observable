include(compile_flags)

set(SOURCE_FILES include/gtest.h
                 src/observer.cpp
                 src/subject.cpp
                 src/detail/function_collection.cpp
                 src/detail/function_handle.cpp
                 src/detail/function_traits.cpp)

source_group(include REGULAR_EXPRESSION include/.*)
source_group(src REGULAR_EXPRESSION src/.*)
source_group(src\\detail REGULAR_EXPRESSION src/detail/.*)

add_executable(observable_test ${SOURCE_FILES})
enable_strict_warnings(observable_test)
set_cpp_standard(observable_test)
target_link_libraries(observable_test gtest gtest_main)
target_include_directories(observable_test PRIVATE include)
target_include_directories(observable_test
                           SYSTEM
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/gtest/include)

add_test(NAME observable_test COMMAND observable_test)
