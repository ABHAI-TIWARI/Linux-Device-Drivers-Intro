# Linux-Device-Drivers-Intro

This project is a beginner-friendly introduction to Linux kernel module development using a **simple character device driver**.

It shows how to:
- register a character device dynamically,
- expose it as a file in `/dev`,
- implement basic file operations (`open`, `read`, `write`, `release`),
- and load/unload the module safely.

---

## Project Goal

In Linux, many devices are accessed through the file interface. This project demonstrates that model by creating a device node named `/dev/simple_char`. User-space programs can then interact with it using normal file commands like `cat` and `echo`.

This is ideal for understanding the core ideas before moving to advanced drivers (ioctl, interrupts, platform drivers, PCI, etc.).

## Files

- `simple_char_driver.c`: Minimal char driver with `open`, `read`, `write`, `release`
- `Makefile`: Kernel module build instructions

---

## How It Works (High Level)

1. Module is loaded with `insmod`.
2. Driver requests a major/minor number using `alloc_chrdev_region`.
3. Driver registers file operations through `cdev`.
4. Driver creates a class and device node (`/dev/simple_char`).
5. User reads/writes through that file.
6. Module is removed with `rmmod`, and resources are cleaned up.

---

## Driver Internals

The driver keeps a small in-kernel message buffer:
- Default data: `"Hello from kernel space!\n"`
- `read`: copies data from kernel buffer to user buffer (`copy_to_user`)
- `write`: copies data from user buffer to kernel buffer (`copy_from_user`)

Implemented callbacks:
- `simple_char_open`
- `simple_char_read`
- `simple_char_write`
- `simple_char_release`

These are registered in a `struct file_operations` table.

## Prerequisites

- Linux system
- Kernel headers for your running kernel
- Build tools (`make`, `gcc`)

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r)
```

If `make` fails with:

`/lib/modules/<kernel-version>/build: No such file or directory`

it means matching kernel headers are missing.

## Build

```bash
make
```

This creates `simple_char_driver.ko`.

---

## Load the module

```bash
sudo insmod simple_char_driver.ko
ls -l /dev/simple_char
sudo dmesg | tail -n 20
```

You should see kernel log lines indicating the allocated major/minor number.

If `/dev/simple_char` is missing (common in some containers/VMs without udev), create it manually:

```bash
cat /sys/class/simple_char_class/simple_char/dev
# example output: 239:0
sudo mknod /dev/simple_char c 239 0
sudo chmod 666 /dev/simple_char
```

---

## Read from the device

```bash
cat /dev/simple_char
```

Expected output (initially):

`Hello from kernel space!`

---

## Write to the device

```bash
echo "Hi kernel" | sudo tee /dev/simple_char >/dev/null
cat /dev/simple_char
```

Now `cat` should print:

`Hi kernel`

---

## Unload the module

```bash
sudo rmmod simple_char_driver
sudo dmesg | tail -n 20
```

---

## Learning Notes

- This example avoids advanced features to keep the core flow clear.
- It is for education, not production use.
- Real drivers usually need stronger error handling, synchronization, and security controls.

## Clean build artifacts

```bash
make clean
```