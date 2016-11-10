#!/usr/bin/env bash
source ./py_env/bin/activate
REQUIREMENTS_FILE=$1
pip install --upgrade -r $REQUIREMENTS_FILE