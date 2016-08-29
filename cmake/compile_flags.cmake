# Enable strict warnings in both debug and release modes. Treat warnings as
# errors in debug mode.
#
# - target_name Name of the configured target.
function(enable_strict_warnings target_name)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(release /Wall /wd4820 /wd4514 /wd4625 /wd4626 /wd4627 /wd5026
                    /wd5027 /wd4710 /wd4668 /wd4711 /wd4548)
        set(debug /WX)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(release -Wall -Wextra -pedantic -Wshadow)
        set(debug -Werror)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(release -Weverything -Wno-system-headers -Wno-c++98-compat
                    -Wno-c++98-compat-pedantic -Wno-exit-time-destructors
                    -Wno-global-constructors -Wno-missing-prototypes
                    -Wno-padded)
        set(debug -Werror)
    else()
        message(WARNING "Custom compiler flags not set.\n"
                        "Your compiler is not supported.\n"
                        "Detected id is '${CMAKE_CXX_COMPILER_ID}'.")
    endif()

    target_compile_options(${target_name} PUBLIC ${release})
    target_compile_options(${target_name} PUBLIC $<$<CONFIG:Debug>:${debug}>)
endfunction(enable_strict_warnings)

# Disablee all warnings.
#
# - target_name Name of the configured target.
function(disable_warnings target_name)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        target_compile_options(${target_name} PUBLIC /W0)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        target_compile_options(${target_name} PUBLIC -w)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        target_compile_options(${target_name} PUBLIC -w)
    endif()
endfunction(disable_warnings)

# Set the required C++ standard version (ie. C++14).
#
# - target_name Name of the configured target.
function(set_cpp_standard target_name)
    set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${target_name} PROPERTY CXX_STANDARD_REQUIRED on)
endfunction(set_cpp_standard)

# Set some common compiler flags.
#
# - target_name Name of the configured target.
function(set_common_compiler_flags target_name)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        target_compile_options(${target_name} PUBLIC /EHsc)
    endif()
endfunction(set_common_compiler_flags)

# Set the default compiler flags.
#
# - target_name Name of the configured target.
function(configure_compiler target_name)
    enable_strict_warnings(${target_name})
    set_common_compiler_flags(${target_name})
    set_cpp_standard(${target_name})
endfunction(configure_compiler)