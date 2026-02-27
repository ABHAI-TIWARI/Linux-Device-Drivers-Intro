# 4) `modinfo` and the `.mod.c` File

## What `modinfo` does

`modinfo` displays metadata embedded in a kernel module.

For your module:

```bash
modinfo simple_char_driver.ko
```

Typical fields include:
- `filename`
- `license`
- `author`
- `description`
- `srcversion`
- `depends`
- `vermagic` (must match running kernel build details)

This command is useful for debugging load failures and checking module details.

## What is `simple_char_driver.mod.c`

`simple_char_driver.mod.c` is an auto-generated build file created by Kbuild (during `make`).

It usually contains:
- module metadata strings,
- build-time registration helpers,
- `MODULE_INFO(...)` entries,
- compiler/kernel version glue information.

## Important notes

- Do **not** manually edit `.mod.c` files.
- If source metadata changes (for example `MODULE_LICENSE`, `MODULE_AUTHOR`, `MODULE_DESCRIPTION` in `simple_char_driver.c`), regenerate by rebuilding:

```bash
make clean
make
```

## Connection between source, `.mod.c`, and `modinfo`

1. You declare metadata in `simple_char_driver.c` using macros like `MODULE_LICENSE(...)`.
2. Kbuild generates `simple_char_driver.mod.c` and compiles metadata into `.ko`.
3. `modinfo simple_char_driver.ko` reads and shows that embedded metadata.

So, `modinfo` output is the user-facing view of metadata that comes from your module source and generated build artifacts.
