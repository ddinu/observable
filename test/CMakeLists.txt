set(SOURCE_FILES observable.cpp
                 observer.cpp)

add_executable(observable_test ${SOURCE_FILES})
target_link_libraries(observable_test gtest)
#target_include_directories(observable_test SYSTEM PRIVATE /external/gtest/include)

add_test(NAME observable_test COMMAND observable_test)
