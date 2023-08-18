#! /usr/bin/env bash
mamba activate gl

# get the directory of this script
GL_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# creat log directory
GL_LOG_DIR=${GL_SCRIPT_DIR}/data/log
mkdir -p ${GL_LOG_DIR}


# set up the environment variables
export GL_LOG_DIR=${GL_LOG_DIR}