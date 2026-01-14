#!/bin/bash

# check the build directory exists
if [ ! -d "build" ]; then
  echo "Build directory does not exist! Building..."
  mkdir build
  ./build.sh
fi

./build/program "$@"