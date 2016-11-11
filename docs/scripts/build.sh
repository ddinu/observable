#!/usr/bin/env bash
export DOC_SOURCE_DIR=$1
export CODE_SOURCE_DIR=$2
export DOXYGEN_OUTPUT_DIR=$3
export HTML_DIR=$4

source ./py_env/bin/activate
sphinx-build -b html $DOC_SOURCE_DIR $HTML_DIR