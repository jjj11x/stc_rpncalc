#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# set GITHUBWORKSPACE if building locally
if [[ -z "${GITHUB_WORKSPACE}" ]]; then
    export GITHUB_WORKSPACE="$SCRIPT_DIR/.."
fi
echo "GITHUB_WORKSPACE: ${GITHUB_WORKSPACE}"

# build
cd $SCRIPT_DIR

if [ "$#" -eq 0 ]; then
    # start shell
    docker-compose run build_tools bash
else
    docker-compose run build_tools /code/steps/$1
fi
