include(FindPackageHandleStandardArgs)

find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build
             HINTS
             PATH_SUFFIXES bin
             DOC "Sphinx")

find_package_handle_standard_args(Sphinx DEFAULT_MSG SPHINX_EXECUTABLE)

mark_as_advanced(SPHINX_EXECUTABLE)