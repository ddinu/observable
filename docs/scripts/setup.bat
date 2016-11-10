@echo off
call py_env\Scripts\activate
set REQUIREMENTS_FILE=%1
pip install --upgrade -r %REQUIREMENTS_FILE%
