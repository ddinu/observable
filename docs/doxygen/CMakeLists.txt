set(SOURCES Doxyfile
            DoxygenLayout.xml)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/html/reference)

set(DOXYGEN_SKIP_DOT on)
find_package(Doxygen)

if(DOXYGEN_FOUND)
    configure_file(Doxyfile Doxyfile)

    add_custom_target(doxygen_docs
                      COMMAND ${CMAKE_COMMAND} -E remove_directory "${OUTPUT_DIR}"
                      COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
                      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
                      SOURCES ${SOURCES}
                      COMMENT "Building the Doxygen documentation."
                      BYPRODUCTS ${OUTPUT_DIR})
else()
    message(STATUS "Doxygen not found. Will not create the doxygen_docs target.")
endif()
