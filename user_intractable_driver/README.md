# User Interactable Driver

## What this driver does (first)

This is a **Linux character device driver** that creates:

- `/dev/user_interactable`
- `/proc/user_interactable`

It allows user-space programs to interact with kernel space in two ways:

- **Character device path**: read/write using `/dev/user_interactable`
- **proc filesystem path**: read/write using `/proc/user_interactable`

So it demonstrates two-way interaction: user -> kernel (`write`) and kernel -> user (`read`) using both `/dev` and `/proc` interfaces.

---

## Files in this folder

- `user_interactable_driver.c`: driver source code with commented explanation
- `Makefile`: build instructions for kernel module

---

## Prerequisites

- Linux system with matching kernel headers
- Build tools (`gcc`, `make`)

Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r)
```

---

## How to compile

From this folder:

```bash
make
```

On success, you should get `user_interactable_driver.ko`.

To clean build artifacts:

```bash
make clean
```

---

## How to load the module

```bash
sudo insmod user_interactable_driver.ko
```

Verify it is loaded:

```bash
lsmod | grep user_interactable_driver
```

Check kernel logs:

```bash
sudo dmesg | tail -n 30
```

---

## How to interact with the driver

### Proc filesystem tutorial (primary)

After loading the module, use `/proc/user_interactable`:

1) Read default message:

```bash
cat /proc/user_interactable
```

2) Write a new message:

```bash
echo "Hello via procfs" | sudo tee /proc/user_interactable > /dev/null
```

3) Read again:

```bash
cat /proc/user_interactable
```

The last write becomes the new message stored by the driver.

### Character device path (also supported)

### 1) Read default message

```bash
cat /dev/user_interactable
```

Expected default text:

`Hello from user_interactable_driver`

### 2) Write a new message

```bash
echo "Hi kernel from user space" | sudo tee /dev/user_interactable > /dev/null
```

### 3) Read again

```bash
cat /dev/user_interactable
```

Now it should print your new text.

Both `/dev/user_interactable` and `/proc/user_interactable` operate on the same internal buffer.

---

## How to unload module

```bash
sudo rmmod user_interactable_driver
```

Verify unload:

```bash
lsmod | grep user_interactable_driver
```

No output means unload succeeded.

---

## Useful troubleshooting

### `/dev/user_interactable` not created

In some environments (minimal VM/container without `udev`), create node manually:

1) Get major:minor:

```bash
cat /sys/class/user_interactable_class/user_interactable/dev
```

2) Create node (example major=240 minor=0):

```bash
sudo mknod /dev/user_interactable c 240 0
sudo chmod 666 /dev/user_interactable
```

### `/proc/user_interactable` not present

Check whether module loaded correctly:

```bash
lsmod | grep user_interactable_driver
sudo dmesg | tail -n 30
```

If module load failed, fix the load/build error first; `/proc` entry is created during module init.

### `insmod: ERROR: could not insert module ... Invalid module format`

Module was built for different kernel version. Rebuild after installing correct headers for `uname -r`.

### `rmmod: ERROR: Module ... is in use`

Some process still has the device open. Find and stop it:

```bash
sudo lsof /dev/user_interactable
```
