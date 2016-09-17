add_subdirectory(shared_lib)

include(compile_flags)
include(observable_includes)
include(source_groups)

set(SOURCE_FILES src/detail/callable_collection.cpp
                 src/detail/unique_subscription.cpp
                 src/detail/shared_subscription.cpp
                 src/gtest.h
                 src/compound_subject.cpp
                 src/property.cpp
                 src/shared_lib.cpp
                 src/subject.cpp)

add_executable(observable_tests ${SOURCE_FILES})

configure_compiler(observable_tests)
add_observable_includes(observable_tests)
target_link_libraries(observable_tests test_shared gtest gtest_main)
target_include_directories(observable_tests PRIVATE src shared_lib)
target_include_directories(observable_tests
                           SYSTEM
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/gtest/include)

add_custom_command(
    TARGET observable_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:test_shared> $<TARGET_FILE_DIR:observable_tests>
)

add_test(NAME observable_tests
         COMMAND observable_tests
         WORKING_DIRECTORY $<TARGET_FILE_DIR:observable_tests>)
