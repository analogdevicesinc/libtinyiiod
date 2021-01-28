#!/bin/bash
set -e

TOP_DIR="$(pwd)"

sudo apt-get update

. ./ci/travis/lib.sh

build_tinyiiod() {
    . ./ci/travis/build_project.sh
}

build_cppcheck() {
    . ./build/cppcheck.sh
}

build_astyle() {
    . ./build/astyle.sh
}

build_${BUILD_TYPE}
