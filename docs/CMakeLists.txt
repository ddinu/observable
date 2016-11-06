find_package(Sphinx)

set(SOURCES conf.py
            index.rst
            developers.rst
            getting-started.rst
            reference.rst
            property-reference.rst
            subject-reference.rst)

if(Sphinx_FOUND)
    add_custom_target(docs
                      COMMAND ${SPHINX_EXECUTABLE} -b html
                              "${CMAKE_CURRENT_SOURCE_DIR}"
                               html
                      COMMENT "Building documentation."
                      SOURCES ${SOURCES}
                      BYPRODUCTS html)
    message(STATUS "Sphinx found.")
else()
    message(STATUS "Sphinx not found. Will not create documentation target.")
endif()