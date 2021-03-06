#!/usr/bin/env bash
#
# Copyright 2019 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -eu

export CC=gcc
export CXX=g++
export DISTRO=ubuntu
export DISTRO_VERSION=18.04

if [[ "${BUILD_NAME+x}" != "x" ]]; then
 echo "The BUILD_NAME is not defined or is empty. Fix the Kokoro .cfg file."
 exit 1
elif [[ "${BUILD_NAME}" = "asan" ]]; then
  export BUILD_TYPE=Debug
  export CC=clang
  export CXX=clang++
  export CMAKE_FLAGS="-DSANITIZE_ADDRESS=yes"
elif [[ "${BUILD_NAME}" = "centos-7" ]]; then
 export DISTRO=centos
 export DISTRO_VERSION=7
elif [[ "${BUILD_NAME}" = "noex" ]]; then
 export DISTRO_VERSION=16.04
 export CMAKE_FLAGS="-DGOOGLE_CLOUD_CPP_ENABLE_CXX_EXCEPTIONS=no"
elif [[ "${BUILD_NAME}" = "ubsan" ]]; then
  export BUILD_TYPE=Debug
  export CC=clang
  export CXX=clang++
  export CMAKE_FLAGS="-DSANITIZE_UNDEFINED=yes"
else
  echo "Unknown BUILD_NAME (${BUILD_NAME}). Fix the Kokoro .cfg file."
  exit 1
fi

if [[ -z "${PROJECT_ROOT+x}" ]]; then
  readonly PROJECT_ROOT="$(cd "$(dirname $0)/../../.."; pwd)"
fi
source "${PROJECT_ROOT}/ci/travis/linux-config.sh"
source "${PROJECT_ROOT}/ci/define-dump-log.sh"

echo "================================================================"
printenv
echo "================================================================"

echo "================================================================"
echo "Updating submodules."
cd "${PROJECT_ROOT}"
git submodule update --init
echo "================================================================"

echo "================================================================"
echo "Creating Docker image with all the development tools."
# We do not want to print the log unless there is an error, so disable the -e
# flag. Later, we will want to print out the emulator(s) logs *only* if there
# is an error, so disabling from this point on is the right choice.
set +e
"${PROJECT_ROOT}/ci/travis/install-linux.sh" \
    >create-build-docker-image.log 2>&1 </dev/null
if [[ "$?" != 0 ]]; then
  dump_log create-build-docker-image.log
  exit 1
fi
echo "================================================================"

echo "================================================================"
echo "Running the full build."
export NEEDS_CCACHE=no
"${PROJECT_ROOT}/ci/travis/build-linux.sh"
exit_status=$?
echo "================================================================"

echo "================================================================"
"${PROJECT_ROOT}/ci/travis/dump-logs.sh"
echo "================================================================"

exit ${exit_status}
