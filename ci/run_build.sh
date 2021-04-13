#!/bin/bash
set -e

TOP_DIR="$(pwd)"

sudo apt-get update

. ./ci/lib.sh

build_tinyiiod() {
    . ./ci/build_project.sh
}

build_cppcheck() {
    . ./build/cppcheck.sh
}

build_astyle() {
    . ./build/astyle.sh
}

build_${BUILD_TYPE}
