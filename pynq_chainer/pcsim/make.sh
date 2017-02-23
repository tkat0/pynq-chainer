#/bin/bash

g++ ../../src/mmult_accel.cpp -o ./mmult_accel.so -fPIC -shared -undefined dynamic_lookup
