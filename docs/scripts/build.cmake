set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(HTML_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)

if(WIN32)
    set(SETUP ${DOCS_SOURCE}/scripts/setup.bat)
    set(BUILD ${DOCS_SOURCE}/scripts/build.bat)
else()
    set(SETUP ${DOCS_SOURCE}/scripts/setup.sh)
    set(BUILD ${DOCS_SOURCE}/scripts/build.sh)
endif()

if(NOT EXISTS py_env)
    execute_process(COMMAND ${VIRTUALENV} py_env)
    execute_process(COMMAND ${SETUP} ${REQUIREMENTS})
endif()

execute_process(COMMAND ${BUILD} ${DOCS_SOURCE} ${CODE_SOURCE} ${DOXYGEN_OUTPUT_DIR} ${HTML_DIR})
