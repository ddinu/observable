include(GenerateExportHeader)
include(compile_flags)
include(observable_includes)
include(source_groups)

set(SOURCE_FILES src/shared_lib.h
                 src/shared_lib.cpp)

add_library(test_shared SHARED ${SOURCE_FILES})

generate_export_header(test_shared)

configure_compiler(test_shared)
add_observable_includes(test_shared)
target_include_directories(test_shared PRIVATE src)
target_include_directories(test_shared PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
