include(compile_flags)
include(default_source_groups)

add_executable(tests
    src/main.cpp
    src/detail/collection.cpp
    src/detail/type_traits.cpp
    src/expressions/expression.cpp
    src/expressions/filters.cpp
    src/expressions/math.cpp
    src/expressions/operators.cpp
    src/expressions/tree.cpp
    src/infinite_subscription.cpp
    src/observe.cpp
    src/shared_subscription.cpp
    src/subject.cpp
    src/unique_subscription.cpp
    src/value.cpp
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set_source_files_properties(src/expressions/operators.cpp PROPERTIES
                                COMPILE_FLAGS /bigobj)
endif()

configure_compiler(tests)
target_link_libraries(tests observable catch)
target_include_directories(tests PRIVATE src)

find_package(Threads)
if(THREADS_FOUND)
    target_link_libraries(tests Threads::Threads)
endif()

add_test(NAME tests
         COMMAND tests
         WORKING_DIRECTORY $<TARGET_FILE_DIR:tests>)
