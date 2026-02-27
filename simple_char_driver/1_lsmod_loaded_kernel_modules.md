# 1) Using `lsmod` to List Loaded Kernel Modules

> Note: The command is `lsmod` (often misspelled as *ismod*).

## What `lsmod` does

`lsmod` shows all kernel modules currently loaded in the running Linux kernel.

It is helpful to:
- verify whether your module is loaded,
- check module size,
- see dependency counts (how many modules/devices are using it).

## Basic usage

```bash
lsmod
```

To find only your driver:

```bash
lsmod | grep simple_char_driver
```

## Understanding output columns

Typical output format:

```text
Module                  Size  Used by
simple_char_driver     16384  0
```

- **Module**: module name (usually the `.ko` filename without `.ko`)
- **Size**: memory used by module code/data
- **Used by**: number of active references/dependencies

If `Used by` is not `0`, removal may fail until users/dependencies are released.

## Example workflow

```bash
sudo insmod simple_char_driver.ko
lsmod | grep simple_char_driver
```

If you see the module in output, it is loaded into the kernel.
