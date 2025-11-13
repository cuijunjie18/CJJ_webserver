#!/bin/bash

rm -rf build
cmake -B build -DENABLE_UT_TEST=ON
cmake --build build -j