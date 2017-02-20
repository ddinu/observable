# Prepare everything for using FindQt5 and building a Qt5 target.
macro(setup_qt)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # Search for 64bit Qt
        set(QT_ROOT $ENV{QT_ROOT_64})
    else()
        # Search for 32bit Qt
        set(QT_ROOT $ENV{QT_ROOT_32})
    endif()

    list(APPEND CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake/Qt5)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER moc)
    source_group(moc REGULAR_EXPRESSION .*/moc_.+\\.cpp)
endmacro()
