set(SOURCES index.rst
            developers.rst
            getting-started.rst)

find_package(PythonInterp)
find_package(Virtualenv)

if(PYTHONINTERP_FOUND AND VIRTUALENV_FOUND)

else()
    if(NOT PYTHONINTERP_FOUND)
        message(STATUS "Python not found. Will not create documentation target.")
    endif()

    if(NOT VIRTUALENV_FOUND)
        message(STATUS "Virtualenv not found. Will not create documentation target.")
    endif()
endif()