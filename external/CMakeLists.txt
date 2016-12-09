set(gtest_force_shared_crt ON CACHE BOOL "Force use of the shared CRT on Windows" FORCE)
add_subdirectory(gtest)

include(compile_flags)

set_cpp_standard(gtest)
disable_warnings(gtest)
set_common_compiler_flags(gtest)

set_cpp_standard(gtest_main)
disable_warnings(gtest_main)
set_common_compiler_flags(gtest_main)
