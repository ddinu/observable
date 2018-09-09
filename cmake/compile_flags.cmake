# Enable strict warnings in both debug and release modes. Treat warnings as
# errors in debug mode.
#
# - target_name Name of the configured target.
function(enable_strict_warnings target_name)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        target_compile_options(${target_name}
            PRIVATE
                /Wall /wd4820 /wd4514 /wd4625 /wd4626 /wd5026 /wd5027 /wd4710
                /wd4571 /wd5039 /wd4623 /wd4774 /wd4548 /wd4711 /wd4868 /wd5045
                /wd4628
                $<$<CONFIG:Debug>:/WX>
        )
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        target_compile_options(${target_name}
            PRIVATE
                -Wall -Wextra -pedantic -Wshadow -Wabi
                $<$<CONFIG:Debug>:-Werror>
        )
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        target_compile_options(${target_name}
            PRIVATE
                -Weverything -Wno-system-headers -Wabi
                -Wno-reserved-id-macro -Wno-padded -Wno-c++98-compat
                -Wno-global-constructors -Wno-missing-prototypes
                -Wno-unused-macros
                $<$<CONFIG:Debug>:-Werror>
        )
    else()
        message(WARNING "Custom compiler flags not set.\n"
                        "Your compiler is not supported.\n"
                        "Detected id is '${CMAKE_CXX_COMPILER_ID}'.")
    endif()
endfunction(enable_strict_warnings)

# Disablee all warnings.
#
# - target_name Name of the configured target.
function(disable_warnings target_name)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        target_compile_options(${target_name} PRIVATE /W0)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        target_compile_options(${target_name} PRIVATE -w)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        target_compile_options(${target_name} PRIVATE -w)
    endif()
endfunction(disable_warnings)

# Set the default compile flags.
#
# - target_name Name of the configured target.
function(set_default_flags target_name)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(flags /MP /GF)

        if(${MSVC_VERSION} GREATER 1900)
            set(flags ${flags} /Zc:__cplusplus)
        endif()
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(flags -fpic)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(flags -fpic)
    else()
        message(WARNING "Custom compiler flags not set.\n"
                        "Your compiler is not supported.\n"
                        "Detected id is '${CMAKE_CXX_COMPILER_ID}'.")
    endif()

    target_compile_options(${target_name} PRIVATE ${flags})
endfunction(set_default_flags)

# Set the required C++ standard version (ie. C++14).
#
# - target_name Name of the configured target.
function(set_cpp_standard target_name)
    get_property(cpp_standard GLOBAL PROPERTY cpp_standard)

    if(NOT CPP_STANDARD AND NOT cpp_standard)
        set(cpp_standard 14)
        message(STATUS "You can set the C++ standard by defining CPP_STANDARD")
    elseif(NOT cpp_standard)
        set(cpp_standard ${CPP_STANDARD})
    endif()
    set_property(GLOBAL PROPERTY cpp_standard ${cpp_standard})

    get_property(cpp_standard_message_printed
        GLOBAL
        PROPERTY cpp_standard_message_printed)
    if(NOT cpp_standard_message_printed)
        message(STATUS "Using the C++${cpp_standard} standard")
        set_property(GLOBAL PROPERTY cpp_standard_message_printed TRUE)
    endif()

    set_property(TARGET ${target_name} PROPERTY CXX_STANDARD ${cpp_standard})
    set_property(TARGET ${target_name} PROPERTY CXX_STANDARD_REQUIRED on)
endfunction(set_cpp_standard)

# Set the default compiler flags.
#
# - target_name Name of the configured target.
function(configure_compiler target_name)
    enable_strict_warnings(${target_name})
    set_cpp_standard(${target_name})
    set_default_flags(${target_name})
endfunction(configure_compiler)