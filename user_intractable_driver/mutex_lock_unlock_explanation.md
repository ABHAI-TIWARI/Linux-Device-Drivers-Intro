# Why `mutex_lock()` and `mutex_unlock()` Are Used in This Driver

## What problem the mutex solves

Your driver has shared data:

- `ui_dev.buffer`
- `ui_dev.data_len`

Both are accessed by multiple paths:

- Character device read/write (`/dev/user_interactable`)
- Procfs read/write (`/proc/user_interactable`)

If two processes access these at the same time, data can become inconsistent. This is called a **race condition**.

---

## Real race example (without mutex)

Suppose:

- Process A is reading `ui_dev.buffer`
- At the same time, Process B writes a new message and updates `ui_dev.data_len`

Without locking, Process A might:

- read old bytes with new length, or
- read partially updated data, or
- get mismatched buffer/length state

This can produce corrupted/incorrect output and unpredictable behavior.

---

## How mutex fixes it

`mutex_lock(&ui_dev.lock)` means:

- “Only one thread can enter this critical section at a time.”

`mutex_unlock(&ui_dev.lock)` means:

- “I’m done updating/reading shared state; others may proceed.”

So each read/write operation sees a consistent state of `buffer` + `data_len`.

---

## Where mutex is used in your code

- Char device read: [user_interactable_driver.c](user_interactable_driver.c#L62-L91)
- Char device write: [user_interactable_driver.c](user_interactable_driver.c#L97-L125)
- Proc read: [user_interactable_driver.c](user_interactable_driver.c#L140-L169)
- Proc write: [user_interactable_driver.c](user_interactable_driver.c#L171-L199)
- Mutex initialization: [user_interactable_driver.c](user_interactable_driver.c#L244)

These are exactly the places where shared data is read/modified.

---

## Why mutex (and not spinlock) here

This driver uses operations like `copy_to_user()` and `copy_from_user()` in process context. A mutex is appropriate for this sleepable context and simpler for beginner character drivers.

---

## Key takeaway

`mutex_lock`/`mutex_unlock` are used to protect shared driver state from concurrent access, ensuring correctness when multiple user processes interact with `/dev/user_interactable` or `/proc/user_interactable` at the same time.
