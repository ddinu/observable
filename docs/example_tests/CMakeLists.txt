include(compile_flags)
include(default_source_groups)

add_executable(docs_example_tests
    src/main.cpp
    src/utility.h
    src/getting_started.cpp
    src/index.cpp
    src/readme.cpp
    src/using_expressions.cpp
)

set_target_properties(docs_example_tests PROPERTIES FOLDER docs)
configure_compiler(docs_example_tests)
target_include_directories(docs_example_tests PRIVATE src)
target_link_libraries(docs_example_tests observable catch)

add_test(NAME docs_example_tests
         COMMAND docs_example_tests
         WORKING_DIRECTORY $<TARGET_FILE_DIR:docs_example_tests>)
