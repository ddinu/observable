include(compile_flags)
include(observable_includes)
include(default_source_groups)

set(SOURCE_FILES src/utility.h
                 src/getting_started.cpp
                 src/index.cpp
                 src/readme.cpp)

add_executable(docs_example_tests ${SOURCE_FILES})

configure_compiler(docs_example_tests)
add_observable_includes(docs_example_tests)
target_include_directories(docs_example_tests PRIVATE src)
target_include_directories(docs_example_tests
                           SYSTEM
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/gtest/include)
target_link_libraries(docs_example_tests gtest gtest_main)

add_test(NAME docs_example_tests
         COMMAND docs_example_tests
         WORKING_DIRECTORY $<TARGET_FILE_DIR:docs_example_tests>)
