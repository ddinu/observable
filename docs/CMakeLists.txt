add_subdirectory(doxygen)
add_subdirectory(sphinx)
add_subdirectory(example_tests)

add_custom_target(ALL_DOCS)

if(TARGET doxygen_docs)
    add_dependencies(ALL_DOCS doxygen_docs)
endif()

if(TARGET sphinx_docs)
    add_dependencies(ALL_DOCS sphinx_docs)
endif()