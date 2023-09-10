#!/usr/bin/env bash
# Auto exit when bash got error

    ############################
    # !!! AVOID THIS LOOPS !!! #
    #       /dev/sdc           #
    #       /dev/sbd           #
    #       /dev/sda           #
    # !!! DO NOT USE THEM  !!! #
    ############################

set -e

#
# Variables
#

TARGET=$1
SIZE=$2

DISK_SECTOR_COUNT=$(( (${SIZE} + 511 ) / 512 ))
FST_STG_SEC_STG_LOCATION_OFFSET=480

DISK_FST_PART_BEGIN=2048
DISK_FST_PART_END=$(( ${DISK_SECTOR_COUNT} - 1 ))

#
# Variables
#

#
# Script body
#

# generate image file
dd if=/dev/zero of=$TARGET bs=512 count=${DISK_SECTOR_COUNT} >/dev/null
# generate image file

# create partition table
parted -s $TARGET mklabel msdos
parted -s $TARGET mkpart primary ${DISK_FST_PART_BEGIN}s ${DISK_FST_PART_END}s
parted -s $TARGET set 1 boot on
# create partition table

# determine how many reserved sectors
SEC_STG_SIZE=$(stat -c%s ${BUILD_DIR}/sec_stg.bin)
SEC_STG_SECTORS=$(( ( ${SEC_STG_SIZE} + 511 ) / 512 ))

# error if sec stg more than fst stg
if [ ${SEC_STG_SECTORS} \< $(( ${DISK_FST_PART_BEGIN} - 1 )) ]; then
    echo "Sec_stage too big"
    exit 2
fi
# determine how many reserved sectors

# create .raw
dd if=${BUILD_DIR}/sec_stg.bin of=$TARGET conv=notrunc bs=512 seek=1 >/dev/null
# create .raw

# Create loopback device
DEVICE=$(losetup -fP --show ${TARGET})
TARGET_PARTITION="${DEVICE}p1"
# Create loopback device

# create file system
mkfs.fat -n "CLOS" $TARGET_PARTITION >/dev/null
# create file system

# install bootloader
dd if=${BUILD_DIR}/fst_stg.bin of=$TARGET_PARTITION conv=notrunc bs=1 count=3 2>&1 > /dev/null
dd if=${BUILD_DIR}/fst_stg.bin of=$TARGET_PARTITION conv=notrunc bs=1 seek=90 skip=90 2>&1 > /dev/null
# install bootloader

# write lba address of stage2 to bootloader
echo "01 00 00 00" | xxd -r -p | dd of=$TARGET_PARTITION conv=notrunc bs=1 seek=$FST_STG_SEC_STG_LOCATION_OFFSET
printf "%x" ${SEC_STG_SECTORS} | xxd -r -p | dd of=$TARGET_PARTITION conv=notrunc bs=1 seek=$(( $FST_STG_SEC_STG_LOCATION_OFFSET + 4 ))
# write lba address of stage2 to bootloader

# copy files
mkdir -p /tmp/clos
mount ${TARGET_PARTITION} /tmp/clos

cp ${BUILD_DIR}/kernel.bin /tmp/clos
echo "Test" > test.txt
cp test.txt /tmp/clos
mkdir /tmp/clos/mydir
cp test.txt /tmp/clos/mydir

umount /tmp/clos
# copy files

# destroy loop back device
losetup -d ${DEVICE}
# destroy loop back device

#
# Script body
#

                                                    # DISK IMAGE CREATION

                ################################################        (...)   #########################
                ##                            |##                       (...)   ##                 |
                ##  MBR (Master Boot Sector)  |##   SECOND_STAGE        (...)   ##   FIRST_STAGE   |    PARTITION (1)
                ##         *partition table*->|##                       (...)   ##                 |
                ####################################################    (...)   ##########################################

                                                    # DISK IMAGE CREATION