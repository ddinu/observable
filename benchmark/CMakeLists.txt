include(compile_flags)
include(observable_includes)
include(source_groups)

# Function benchmark target.
add_executable(bench_function src/utility.h
                              src/function.cpp)
add_observable_includes(bench_function)
set_cpp_standard(bench_function)

# Virtual function benchmark target.
add_executable(bench_virtual src/utility.h
                             src/virtual.cpp)
add_observable_includes(bench_virtual)
set_cpp_standard(bench_virtual)

# Qt signal-slot benchmark target.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # Search for 64bit Qt
    set(QT_ROOT $ENV{QT_ROOT_64})
else()
    # Search for 32bit Qt
    set(QT_ROOT $ENV{QT_ROOT_32})
endif()

list(APPEND CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake/Qt5)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 QUIET OPTIONAL_COMPONENTS Core)

if(Qt5_FOUND)
    message(STATUS "Building Qt benchmark. Qt5 found: ${QT_ROOT}")

    add_executable(bench_qt src/utility.h
                            src/qt.h
                            src/qt.cpp)
    add_observable_includes(bench_qt)
    set_cpp_standard(bench_qt)
    target_link_libraries(bench_qt Qt5::Core)
endif()
