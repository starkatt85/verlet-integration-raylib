#!/usr/bin/env bash

rm -rf build
cmake -S . -B build
cd build
make
./app
cd ..
