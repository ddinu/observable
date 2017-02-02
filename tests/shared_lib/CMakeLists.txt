include(GenerateExportHeader)
include(compile_flags)
include(default_source_groups)

set(SOURCE_FILES include/shared/shared_lib.h
                 src/shared_lib.cpp)

add_library(shared SHARED ${SOURCE_FILES})

generate_export_header(shared)
configure_compiler(shared)
target_include_directories(shared PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(shared observable)
