#!/bin/bash
# Unix. Task 1. Pachkovsky A.
pwd=$(pwd)

# Copy 512 byte blocks of 1M size from a /dev/zero device to an ext2fsfile.dat file byte by byte
dd if=/dev/zero of=ext2fsfile.dat bs=1M count=512

# Create an ext2 filesystem in a file ext2fsfile.dat
# forced, otherwise it complains that ex2fsfile is not a block device
mke2fs ext2fsfile.dat -F

# Check if filesystem exists
file ext2fsfile.dat

# Mount ext2fsfile.dat file as a loop device. 
# Lets suppose that there are no already mounted loop device,
# so it's ok to strat with zero
mount ext2fsfile.dat /mnt/ext2fsfile -o loop=/dev/loop0

# Ensure that /dev/loop0 device created and ext2fsfile.dat mounted to it
losetup -a | grep loop0
cat /proc/mounts | grep loop0

# Give access to my account
chown -R pachkovsky /mnt/ext2fsfile/

# Create a simple file structure
cd /mnt/ext2fsfile
echo some text > text0.txt
mkdir dir0
cd dir0
echo some another text > text1.txt
cd ..
mkdir dir1
cd dir1
echo some another text again > text2.txt
cd $pwd

tree /mnt/ext2fsfile

# Unmount
umount /mnt/ext2fsfile -l

rm ext2fsfile.dat

# Ensure that /dev/loop0 device is removed and ext2fsfile.dat unmounted
losetup -a | grep loop0
cat /proc/mounts | grep loop0















