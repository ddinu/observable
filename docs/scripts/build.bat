@echo off
set DOC_SOURCE_DIR=%1
set CODE_SOURCE_DIR=%2
set DOXYGEN_OUTPUT_DIR=%3
set HTML_DIR=%4

call py_env\Scripts\activate
sphinx-build -b html %DOC_SOURCE_DIR% %HTML_DIR%
