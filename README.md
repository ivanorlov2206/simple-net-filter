# simple-net-filter
Just-for-fun simple kernel module for income packets filtering, implemented via netfilter hooks functionality and character device.
Also it uses kernel data structure - double linked list to store allowed ip ranges;

## What is it?
It is a kernel module, which can filter income packets by source ip ranges. IP ranges can be set via ioctl call to the character device "filter"

## How it works
When it is dynamically linked into the kernel, it creates a character device and immediately begins to filter income packets.
If you want to allow packets from some ip range, you can easily set it via ioctl system call (to get example see userspace/test.c).

## How to build
Just call
```
make
```
in the source directory of module.
To build simple userspace program that uses simple-net-filter api:
```
cd userspace
gcc test.c -o test
```

## How to use
Firstly, you need to link module into the kernel:
```
sudo insmod filter.ko
```
Afther that, you can access filter through the simple userspace interface:
```
cd userspace
./test
```
If you don't need this module anymore, remove it from kernel to free system resources:
```
sudo rmmod filter
```
