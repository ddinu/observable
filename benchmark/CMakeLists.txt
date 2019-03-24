include(compile_flags)
include(default_source_groups)
include(setup_qt)

# Function benchmark target.
add_executable(bench_function src/utility.h
                              src/function.cpp)
set_cpp_standard(bench_function)
set_target_properties(bench_function PROPERTIES FOLDER benchmarks)
target_link_libraries(bench_function observable)

# Virtual function benchmark target.
add_executable(bench_virtual src/utility.h
                             src/virtual.cpp)
set_cpp_standard(bench_virtual)
set_target_properties(bench_virtual PROPERTIES FOLDER benchmarks)
target_link_libraries(bench_virtual observable)

# Expressions benchmark target
add_executable(bench_expressions src/utility.h
                                 src/expressions.cpp)
set_cpp_standard(bench_expressions)
set_target_properties(bench_expressions PROPERTIES FOLDER benchmarks)
target_link_libraries(bench_expressions observable)

# Qt signal-slot benchmark target.
setup_qt()
find_package(Qt5 QUIET OPTIONAL_COMPONENTS Core)

if(Qt5_FOUND)
    message(STATUS "Creating 'bench_qt' target. Using Qt5 from: ${QT_ROOT}")

    add_executable(bench_qt src/utility.h
                            src/qt.h
                            src/qt.cpp)
    set_cpp_standard(bench_qt)
    set_target_properties(bench_qt PROPERTIES FOLDER benchmarks)
    target_link_libraries(bench_qt observable
                                   Qt5::Core)
endif()
