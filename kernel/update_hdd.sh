#!/bin/bash

# create a loop device that corresponds to the fat16 partiton (file system)
sudo losetup -o 1048576  --sizelimit 32505856 /dev/loop1 ../hdd.img

sudo mount /dev/loop1 ../mount/
sudo cp kernel.elf ../mount
sudo umount ../mount/

# detach the loop device
sudo losetup -d /dev/loop1
