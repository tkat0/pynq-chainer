#!/bin/bash

mkdir -p ./pynq_chainer/HLS/src

cp ./SDDebug/_sds/swstubs/*.h* ./pynq_chainer/HLS/src/
cp ./SDDebug/_sds/swstubs/*.c* ./pynq_chainer/HLS/src/
cp ./src/*.h ./pynq_chainer/HLS/src
cp ./SDDebug/_sds/p0/ipi/pynq.runs/impl_1/bitstream.bit ./pynq_chainer/HLS
cp ./SDDebug/_sds/p0/ipi/top.bd.tcl ./pynq_chainer/HLS/bitstream.tcl
