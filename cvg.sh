#!/bin/bash

# compile for valgrind <vg.sh> "libs" main

clear && gcc -std=c++11 -w -g -Iinclude src/vec2.cpp src/vec3.cpp $2.cpp $1 -lstdc++ -lm -o $2 && valgrind --leak-check=full ./$2
