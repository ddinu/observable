include(compile_flags)
include(observable_includes)
include(source_groups)

set(SOURCE_FILES src/detail/auto_handle.cpp
                 src/detail/function_collection.cpp
                 src/detail/function_traits.cpp
                 src/detail/handle.cpp
                 src/gtest.h
                 src/subject.cpp)

add_executable(observable_test ${SOURCE_FILES})
configure_compiler(observable_test)
add_observable_includes(observable_test)
target_link_libraries(observable_test gtest gtest_main)
target_include_directories(observable_test PRIVATE src)
target_include_directories(observable_test
                           SYSTEM
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/gtest/include)

add_test(NAME observable_test COMMAND observable_test)
