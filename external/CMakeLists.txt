set(gtest_force_shared_crt ON CACHE BOOL "Force use of the shared CRT on Windows." FORCE)
add_subdirectory(gtest)

include(compile_flags)

set_cpp_standard(gtest)
disable_warnings(gtest)
target_include_directories(gtest INTERFACE gtest/include)

set_cpp_standard(gtest_main)
disable_warnings(gtest_main)
target_include_directories(gtest_main INTERFACE gtest/include)
