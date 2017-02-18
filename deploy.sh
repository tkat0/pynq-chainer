#!/bin/bash

mkdir -p ./HLS/src

cp SDDebug/_sds/swstubs/*.{c*|h*} ./src/
cp SDDebug/_sds/p0/ipi/pynq.runs/impl_1/bitstream.bit ./
cp SDDebug/_sds/p0/ipi/top.bd.tcl ./bitstream.tcl
