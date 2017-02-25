include(compile_flags)
include(default_source_groups)
include(setup_qt)

set(SOURCES src/game_controller.h
            src/game_controller.cpp
            src/game_model.h
            src/game_model.cpp
            src/game_view.h
            src/game_view.cpp
            src/main.cpp
            README.rst)

setup_qt()
find_package(Qt5 QUIET OPTIONAL_COMPONENTS Core Gui Widgets)

if(Qt5_FOUND)
    message(STATUS "Creating 'capture_the_dot' target. Using Qt5 from: ${QT_ROOT}")

    add_executable(capture_the_dot WIN32 EXCLUDE_FROM_ALL ${SOURCES})
    set_cpp_standard(capture_the_dot)
    set_target_properties(capture_the_dot PROPERTIES FOLDER examples)
    target_link_libraries(capture_the_dot observable
                                          Qt5::Core
                                          Qt5::Gui
                                          Qt5::Widgets)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set_target_properties(capture_the_dot PROPERTIES LINK_FLAGS /ENTRY:mainCRTStartup)
    endif()
endif()
