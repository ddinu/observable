include(compile_flags)
include(observable_includes)
include(source_groups)

# Function benchmark target.
add_executable(function_benchmark src/timing.h
                                  src/function.cpp)
add_observable_includes(function_benchmark)
set_cpp_standard(function_benchmark)

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

    add_executable(qt_benchmark src/timing.h
                                src/qt.h
                                src/qt.cpp)
    add_observable_includes(qt_benchmark)
    set_cpp_standard(qt_benchmark)
    target_link_libraries(qt_benchmark Qt5::Core)
endif()