#!/usr/bin/env bash

TARGET=$1

dd if=/dev/zero of=$TARGET bs=512 count=2880
mkfs.fat  -F 12 -f 2880 $TARGET
dd if=${BUILD_DIR}/fst_stg.bin of=$TARGET conv=notrunc

mcopy -i $TARGET ${BUILD_DIR}/sec_stg.bin "::sec_stg.bin"
mcopy -i $TARGET ${BUILD_DIR}/kernel.bin "::kernel.bin"
mcopy -i $TARGET test.txt "::test.txt"

mmd -i $TARGET "::mydir"
mcopy -i $TARGET test.txt "::mydir/test.txt"