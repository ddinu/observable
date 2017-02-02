set(SOURCES conf.py
            developers.rst
            getting-started.rst
            index.rst)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/html)

find_package(PythonInterp)
find_package(Sphinx)

if(PYTHONINTERP_FOUND AND SPHINX_FOUND)
    add_custom_target(sphinx_docs
                      COMMAND ${SPHINX_EXECUTABLE} -b html ${CMAKE_CURRENT_SOURCE_DIR} ${OUTPUT_DIR}
                      SOURCES ${SOURCES}
                      COMMENT "Building the Sphinx documentation."
                      BYPRODUCTS ${OUTPUT_DIR})
else()
    if(NOT PYTHONINTERP_FOUND)
        message(STATUS "Python not found. Will not create the sphinx_docs documentation target.")
    endif()

    if(NOT SPHINX_FOUND)
        message(STATUS "Sphinx not found. Will not create the sphinx_docs documentation target.")
    endif()
endif()