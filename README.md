# Linux Device Driver Course Guide

This repository is a practical starting point for learning Linux device driver development. This guide explains the core theory, architecture, and learning path you should follow to become strong in Linux drivers.

## What Is a Linux Device Driver?

A Linux device driver is kernel code that allows the operating system to communicate with hardware.

It bridges three layers:

1. Hardware (registers, interrupts, DMA, buses)
2. Linux kernel subsystems (VFS, networking, block layer, input, etc.)
3. User-space applications (through /dev, sysfs, ioctl, netlink, and other interfaces)

## User Space vs Kernel Space

### User Space

- Normal applications and services run here
- Restricted privileges and isolated memory
- Cannot access hardware directly

### Kernel Space

- Full privileges and direct hardware access
- Handles process scheduling, memory, filesystems, networking, and drivers
- A bug here can crash or hang the whole system

### How They Communicate

- Device files in /dev (open, read, write)
- sysfs entries in /sys
- procfs entries in /proc
- ioctl for custom command/control
- poll/select/epoll for event-driven I/O
- mmap for mapped buffers
- netlink for structured kernel-user communication

## Module Basics

### Loadable Module

- Built separately and loaded/unloaded at runtime
- Faster development cycle
- Useful for testing and debugging

### Built-In Driver

- Compiled into the kernel image
- Needed for devices required very early in boot

## Driver Lifecycle: Attach and Detach

Attach and detach are core concepts in all modern Linux driver models.

### Attach (Probe/Bind) Flow

1. Module loads and registers driver with a bus/subsystem
2. Kernel finds matching hardware
3. Probe callback executes
4. Driver allocates and initializes resources:
	- I/O memory mapping
	- Interrupt request (IRQ)
	- DMA buffers
	- clocks, regulators, GPIOs (common in embedded)
5. Driver publishes user-space interfaces
6. Device becomes operational

### Detach (Remove/Unbind) Flow

1. Device disconnects, unbinds, or module unload starts
2. Remove callback executes
3. Driver stops hardware safely
4. Driver releases resources:
	- free IRQ
	- unmap I/O
	- free DMA and memory
	- unregister interfaces
5. Driver exits cleanly

Rule: every resource allocated during attach must be released during detach.

## Major Types of Linux Drivers

There is no single fixed count. Drivers are categorized by function and by bus/subsystem.

### 1. Character Drivers

- Byte-stream access model
- Common for UART, sensors, custom devices
- Core API: open, read, write, ioctl, poll, mmap

### 2. Block Drivers

- Block-oriented I/O for storage
- Used by disks and storage-like devices
- Integrates with Linux block layer and request handling

### 3. Network Drivers

- Packet transmit/receive
- Uses net_device and NAPI
- Focus on throughput, latency, and offloads

### 4. USB Drivers

- Match by USB IDs and interfaces
- Handle hotplug probe/disconnect
- Communicate through USB endpoints

### 5. PCI/PCIe Drivers

- Match by vendor/device IDs
- Manage BARs, MSI/MSI-X interrupts, DMA
- Common in NICs, GPUs, storage controllers, accelerators

### 6. Platform Drivers

- Common in SoC/embedded systems
- Non-discoverable hardware described by Device Tree or ACPI
- Typical for board-integrated peripherals

### 7. I2C and SPI Drivers

- Peripheral bus drivers (sensors, PMICs, codecs, displays)
- Frequent in embedded Linux

### 8. Input Drivers

- Keyboard, mouse, touch, game controllers
- Integrate with Linux input subsystem

### 9. TTY/Serial Drivers

- UART and terminal-style interfaces
- Integrate with TTY core

### 10. Sound Drivers (ALSA/ASoC)

- Audio playback/capture
- Codec and routing management

### 11. Graphics Drivers (DRM/KMS)

- Display and GPU pipeline management
- Advanced subsystem with modesetting and memory management

### 12. Virtual and Misc Drivers

- Software-only or pseudo devices
- Useful for testing and education

## Essential Topics in Any Linux Driver Course

1. Linux kernel architecture and module lifecycle
2. Kernel build system and out-of-tree module compilation
3. Character driver fundamentals
4. Kernel memory APIs (kmalloc, kzalloc, vmalloc)
5. Safe user memory transfer (copy_to_user/copy_from_user)
6. Concurrency and synchronization:
	- mutex
	- spinlock
	- atomics
	- completions
7. Interrupt handling and deferred work
8. Kernel device model (bus, device, driver matching)
9. Device file creation, major/minor numbers, udev
10. sysfs/procfs interfaces
11. ioctl design and ABI stability
12. DMA basics and cache coherency
13. Power management (suspend/resume, runtime PM)
14. Robust error handling and cleanup paths
15. Security hardening and input validation
16. Debugging and tracing
17. Testing, stress testing, and fault injection
18. Upstream coding style and patch workflow

## Synchronization and Context Rules

Knowing context is critical:

- Process context: can sleep (mutex is allowed)
- Interrupt context: must not sleep (use spinlock or lock-free approach)

Common primitives:

- mutex: sleeping lock for process context
- spinlock: non-sleeping lock for atomic/IRQ context
- atomic_t: lockless counters/flags where appropriate
- completion: wait for async event completion

## Security and Reliability Checklist

- Validate all user input lengths and states
- Protect all shared data against races
- Avoid stack overuse in kernel code
- Handle every failure path
- Prevent use-after-free and double-free
- Keep user-space interfaces stable
- Test repeated load/unload cycles

## Build, Load, and Inspect Tools

Core commands:

- make
- insmod
- rmmod
- lsmod
- modinfo
- dmesg

Useful debug facilities:

- printk/dev_info/dev_err
- dynamic debug
- ftrace and tracepoints
- perf and eBPF-based tracing
- lockdep, KASAN, UBSAN, KCSAN

## Suggested Learning Roadmap

### Phase 1: Foundations

1. C programming for systems
2. Linux internals (processes, memory, syscalls)
3. Kernel module basics

### Phase 2: Core Driver Skills

1. Character driver implementation
2. ioctl and poll
3. procfs/sysfs
4. synchronization and interrupts

### Phase 3: Real Hardware Models

1. Platform + Device Tree
2. I2C/SPI drivers
3. USB or PCI driver fundamentals

### Phase 4: Advanced Topics

1. DMA and performance tuning
2. power management
3. tracing and sanitizer-driven debugging
4. production-grade error handling

### Phase 5: Professional Readiness

1. Write clean, reviewable patches
2. Follow kernel coding style
3. design stable interfaces
4. build automated test routines

## Common Beginner Mistakes

1. Missing cleanup in error paths
2. Sleeping in interrupt context
3. Incorrect lock usage leading to deadlock
4. Unsafe user pointer handling
5. Ignoring suspend/resume behavior
6. Insufficient stress testing

## Repository Tutorials

Current practical examples in this repository:

1. simple_char_driver
2. user_intractable_driver (interactive character driver + proc filesystem tutorial)

## Outcome Goal

You are considered strong in Linux driver development when you can:

1. Build a robust character driver with ioctl/poll/mmap support
2. Handle interrupts and synchronization safely
3. Implement correct probe/remove resource management
4. Integrate with at least one real bus model (USB/PCI/platform/I2C/SPI)
5. Debug race conditions and kernel crashes methodically
6. Deliver stable and maintainable kernel code
