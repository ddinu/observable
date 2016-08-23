set(SOURCE_FILES #observer.cpp
                 subject.cpp
                 detail/function_collection.cpp
                 detail/function_handle.cpp
                 detail/function_traits.cpp)

source_group(source REGULAR_EXPRESSION .*\\.cpp)
source_group(source\\detail REGULAR_EXPRESSION .*/detail/.*\\.cpp)

add_executable(observable_test ${SOURCE_FILES})
target_link_libraries(observable_test gtest gtest_main)
target_include_directories(observable_test
                           SYSTEM
                           PRIVATE ${PROJECT_SOURCE_DIR}/external/gtest/include)

add_test(NAME observable_test COMMAND observable_test)
