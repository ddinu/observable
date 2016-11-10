find_package(PythonInterp)
find_package(Virtualenv)

set(DOXYGEN_SKIP_DOT on)
find_package(Doxygen)

set(SOURCES conf.py
            index.rst
            developers.rst
            getting-started.rst
            reference/index.rst
            reference/property.rst
            reference/shared_subscription.rst
            reference/subject.rst
            reference/unique_subscription.rst)

if(PYTHONINTERP_FOUND AND DOXYGEN_FOUND AND VIRTUALENV_FOUND)
    add_custom_target(docs
                      COMMAND ${CMAKE_COMMAND}
                              -DPYTHON=${PYTHON_EXECUTABLE}
                              -DVIRTUALENV=${VIRTUALENV_EXECUTABLE}
                              -DREQUIREMENTS=${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
                              -DDOCS_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                              -DCODE_SOURCE=${CMAKE_SOURCE_DIR}/include
                              -P ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build.cmake
                      COMMENT "Building the documentation."
                      SOURCES ${SOURCES}
                      BYPRODUCTS html)

    message(STATUS "Python, Virtualenv and Doxygen found. Documentation target is 'docs'.")
else()
    if(NOT PYTHONINTERP_FOUND)
        message(STATUS "Python not found. Will not create documentation target.")
    endif()

    if(NOT VIRTUALENV_FOUND)
        message(STATUS "Virtualenv not found. Will not create documentation target.")
    endif()

    if(NOT DOXYGEN_FOUND)
        message(STATUS "Doxygen not found. Will not create documentation target.")
    endif()
endif()