#!/usr/bin/env bash
DOC_SOURCE_DIR=$1
CODE_SOURCE_DIR=$2
DOXYGEN_OUTPUT_DIR=$3
HTML_DIR=$4

source ./py_env/bin/activate
sphinx-build -b html $DOC_SOURCE_DIR $HTML_DIR