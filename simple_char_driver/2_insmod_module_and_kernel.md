# 2) `insmod`, Kernel Module, and Kernel

> Note: The command is `insmod` (often misspelled as *insmode*).

## Kernel vs Module

- **Kernel**: Core of the OS, always running in privileged mode.
- **Kernel module**: Loadable piece of kernel code (`.ko`) that can be inserted/removed at runtime.

Your `simple_char_driver.ko` is a kernel module.

## What `insmod` does

`insmod` inserts a specific module file into the running kernel.

```bash
sudo insmod simple_char_driver.ko
```

When this runs successfully:
1. Kernel loads the module object.
2. Module init function runs (the function registered by `module_init(...)`).
3. Your driver registers device numbers and creates `/dev/simple_char` (as implemented in your source).

## Important points

- `insmod` loads from a file path you provide.
- It does **not** automatically resolve all dependencies as well as `modprobe` does.
- Module name is typically derived from filename (without `.ko`).

## Verify after loading

```bash
lsmod | grep simple_char_driver
sudo dmesg | tail -n 20
```

`dmesg` helps confirm initialization messages from your driver.

## Common error examples

- **Operation not permitted**: run with `sudo`.
- **Invalid module format**: built for a different kernel version/architecture.
- **Unknown symbol**: unresolved dependency or build mismatch.
