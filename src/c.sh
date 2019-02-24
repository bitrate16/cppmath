#!/bin/bash

# compile <c.sh> "libs" main

clear && gcc -std=c++11 -w -g vec2.cpp vec3.cpp $2.cpp $1 -lstdc++ -lm -o $2 && ./$2