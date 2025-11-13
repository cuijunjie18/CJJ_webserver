#!/bin/bash

rm -rf build
cmake -B build -DENABLE_UT_TEST=OFF
cmake --build build -j