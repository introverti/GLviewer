#!/usr/bin/env bash
set -e

CURRENT_PATH=$(cd $(dirname $0) && pwd)
BUILD_PATH="$CURRENT_PATH/build"
INSTALL_PREFIX="$CURRENT_PATH/install"
ARCH=$(uname -m)

FLAGS_UNITTEST=false
FLAGS_COVERAGE=false
COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report

# function _build() {
#     mkdir -p $BUILD_PATH && cd $BUILD_PATH
#     cmake ../modules -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DENABLE_UNITTEST=${FLAGS_UNITTEST} -DENABLE_COVERAGE=${FLAGS_COVERAGE}
#     cd $BUILD_PATH
#     make -j4
#     make install
# }

# function _build_pipeline() {
#     _build
#     exit 0
# }

# function _usage() {
#     echo -e "\n${RED}Usage${NO_COLOR}:
#     .${BOLD}/sl.bash${NO_COLOR} [OPTION]"
#     echo -e "\n${RED}Options${NO_COLOR}:
#     ${BLUE}init${NO_COLOR}\t create mamba environment
#     ${BLUE}update${NO_COLOR}\t update mamba environment
#     ${BLUE}build${NO_COLOR}\t build and install all modules
#     ${BLUE}test${NO_COLOR}\t run all unittests and generate coverage report
#     ${BLUE}usage${NO_COLOR}\t show this message and exit"

#     echo -e "\n${RED}Optionals:
#     ${BLUE}--mode${NO_COLOR}\t [Debug, Release]
#     "
# }

function _export() {
    echo "========== Exporting mamba environment..."
    mamba env export >glviewer.yaml
    echo "========== environment exported."
}

function _init() {
    echo "========== Creating mamba environment..."
    mamba env create -f glviewer.yaml
    echo "========== environment created and dependencies installed."
}

function _update() {
    echo "========== Updating mamba environment..."
    mamba env update -f glviewer.yaml --prune
    echo "========== environment updated."
}

# function _test() {
#     echo "========== Testing coverage..."
#     FLAGS_UNITTEST=true
#     FLAGS_COVERAGE=true
#     _build
#     ctest
#     lcov --rc lcov_branch_coverage=1 -c -d . -o ${COVERAGE_FILE}_tmp
#     lcov --rc lcov_branch_coverage=1 -e ${COVERAGE_FILE}_tmp "*autolabel/modules*" -o ${COVERAGE_FILE}
#     genhtml --rc genhtml_branch_coverage=1 --highlight --legend ${COVERAGE_FILE} -o ${REPORT_FOLDER}
#     echo "========== Test done."
# }

function main() {
    # the number of arguments
    if [ "$#" -eq 0 ]; then
        _usage
        exit 0
    fi
    local cmd="$1"
    shift
    case "${cmd}" in
    export)
        _export $@
        ;;
    init)
        _init $@
        ;;
    update)
        _update $@
        ;;
    build)
        _build $@
        ;;
    test)
        _test $@
        ;;
    -h | --help | usage | *)
        _usage
        ;;
    esac # end case
}

main "$@"
