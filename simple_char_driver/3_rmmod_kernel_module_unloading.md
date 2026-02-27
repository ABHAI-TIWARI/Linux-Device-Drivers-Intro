# 3) `rmmod`, Kernel, and Module Unloading

> Note: The command is `rmmod` (often misspelled as *rmmode*).

## What `rmmod` does

`rmmod` removes a loaded kernel module from the running kernel.

```bash
sudo rmmod simple_char_driver
```

Use module name **without** `.ko`.

## What happens during unload

When unloading:
1. Kernel checks module reference count (`Used by`).
2. If safe, module exit function runs (registered by `module_exit(...)`).
3. Driver cleanup occurs: device node/class/cdev/allocated numbers are released.
4. Module memory is freed.

## Verify unload

```bash
lsmod | grep simple_char_driver
sudo dmesg | tail -n 20
```

- No `lsmod` output for your module means unload succeeded.
- `dmesg` should show cleanup/unload logs.

## Why unload can fail

`rmmod` may fail with “module is in use” when:
- a process still has `/dev/simple_char` open,
- another module depends on this module,
- reference count is non-zero.

Find processes using your device:

```bash
sudo lsof /dev/simple_char
```

Stop those processes and try `rmmod` again.


===========================================================================================
## insmod module

1. calls `init_module` to hint the kernel that a module insertion is attempted.

2. Transfers control to the kernel.

3. kernel execute `sys_init_module`

    1. Verifies permissions

    2. `load_module` function is called

        1. Checks the sanity of the .ko

        2. Creates memory.

        3. Copies from user space to kernel space.

        4. Resolves sysmbols.

        5. Returns a reference to the Kernel

    3. Adds the reference to a linked list that has all the loaded modules.

4. `module_init` listed function



=======================================================


## rmmod module

1. `rmmod` calls `delete_module()` which hints the kernel that a module is to be removed.

    1. Control is transferred to the kernel.

2. kernel executes `sys_delete_module()`

    1. Checks the permissions of the one requesting.

    2. Checks if any other loaded module needs the current module.

    3. Checks if the module is actually loaded!

    4. Executes the function provided in `module_exit`

    5. `free_module()`

        1. removes references and kernel object reference.

        2. performs any other cleanup.

        3. Unloads the module

        4. Changes the state in list

        5. Removes it from the list and frees the memory.
