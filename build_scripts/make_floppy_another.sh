#!/usr/bin/env bash

TARGET=$1
STAGE1_STAGE2_LOCATION_OFFSET=480

# generate image file
dd if=/dev/zero of=$TARGET bs=512 count=2880 >/dev/null

# determine how many reserved sectors
STAGE2_SIZE=$(stat -f%z ${BUILD_DIR}/sec_stg.bin)
STAGE2_SECTORS=$(( ( ${STAGE2_SIZE} + 511 ) / 512 ))
RESERVED_SECTORS=$(( 1 + ${STAGE2_SECTORS} ))

# create file system
newfs_msdos -F 12 -f 2880 -r ${RESERVED_SECTORS} -v "CLOS" $TARGET
# /usr/sbin/mkfs.fat -F 12 -R ${RESERVED_SECTORS} -n "CLOS" $TARGET 

# install bootloader
dd if=${BUILD_DIR}/fst_stg.bin of=$TARGET conv=notrunc bs=1 count=3 > /dev/null
dd if=${BUILD_DIR}/fst_stg.bin of=$TARGET conv=notrunc bs=1 seek=90 skip=90 > /dev/null
dd if=${BUILD_DIR}/sec_stg.bin of=$TARGET conv=notrunc bs=512 seek=1 >/dev/null

# write lba address of stage2 to bootloader
echo "01 00 00 00" | xxd -r -p | dd of=$TARGET conv=notrunc bs=1 seek=$STAGE1_STAGE2_LOCATION_OFFSET
printf "%x" ${STAGE2_SECTORS} | xxd -r -p | dd of=$TARGET conv=notrunc bs=1 seek=$(( $STAGE1_STAGE2_LOCATION_OFFSET + 4 ))

# copy files
mcopy -i $TARGET ${BUILD_DIR}/kernel.bin "::kernel.bin"
mcopy -i $TARGET test.txt "::test.txt"
