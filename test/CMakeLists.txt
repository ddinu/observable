add_subdirectory(shared_lib)

include(compile_flags)
include(observable_includes)
include(source_groups)

set(SOURCE_FILES src/detail/auto_handle.cpp
                 src/detail/function_collection.cpp
                 src/detail/function_traits.cpp
                 src/detail/handle.cpp
                 src/gtest.h
                 src/shared_lib.cpp
                 src/subject.cpp)

add_executable(test_observable ${SOURCE_FILES})

configure_compiler(test_observable)
add_observable_includes(test_observable)
target_link_libraries(test_observable test_shared gtest gtest_main)
target_include_directories(test_observable PRIVATE src shared_lib)
target_include_directories(test_observable
                           SYSTEM
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/gtest/include)

add_custom_command(
    TARGET test_observable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:test_shared> $<TARGET_FILE_DIR:test_observable>
)

add_test(NAME test_observable
         COMMAND test_observable
         WORKING_DIRECTORY $<TARGET_FILE_DIR:test_observable>)
