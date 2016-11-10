include(FindPackageHandleStandardArgs)

find_program(VIRTUALENV_EXECUTABLE
             NAMES virtualenv
             HINTS
             $ENV{VIRTUALENV_DIR}
             PATH_SUFFIXES bin
             DOC "Virtualenv")

find_package_handle_standard_args(Virtualenv DEFAULT_MSG VIRTUALENV_EXECUTABLE)

mark_as_advanced(VIRTUALENV_EXECUTABLE)