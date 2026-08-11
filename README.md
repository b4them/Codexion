*This project has been created as part of the 42 curriculum by bmoura-s.*

# Codexion

## Description

Codexion is a complex concurrency and resource synchronization simulation written in C. It models a high-pressure co-working environment where multiple coders must share a limited number of USB dongles to compile their code. The project demonstrates the mastery of POSIX threads, mutexes, condition variables, and custom scheduling algorithms (FIFO and Earliest Deadline First) to prevent deadlocks, data races, and thread starvation before a strict burnout deadline is reached.

## Instructions

### Compilation

A `Makefile` is provided at the root of the repository. To compile the program run:
`make`

Other available rules:
* `make clean`: Removes object files.
* `make fclean`: Removes object files and the executable.
* `make re`: Recompiles the entire project.


### Execution

Run the program with the following arguments:
Number of coders (a);
Time to burnout (b);
Time to compile (c);
Time to debug (d);
Time refactor (e);
Number of compiles required (f);
Dongle cooldown (g);
Scheduler (h);

`./codexion a b c d e f g h`

** Example (EDF Scheduler):**

`./codexion 5 800 200 200 200 7 10 edf`

**Example (FIFO Scheduler):**

`./codexion 4 600 200 100 100 10 50 fifo`

## Blocking cases handled

Several critical concurrency issues were addressed to ensure the simulation runs flawlessly without undefined behavior:

* **Deadlock Prevention (Coffman's Conditions):** To prevent a circular wait (where every coder takes their left dongle and waits infinitely for the right), the dongle acquisition order is strictly managed. The last coder requests the `0` (left) dongle first and then their `i` (right) dongle, breaking the cycle.

* **Starvation Prevention:** A custom priority queue (heap) algorithm arbitrates dongle distribution. Under the `edf` scheduler, dynamic priority is given to the coder closest to their burnout deadline, guaranteeing liveness and preventing starvation.

* **Cooldown Handling:** Dongle cooldowns are managed via absolute time checks (`get_time_ms`) rather than inaccurate blocking sleep calls, ensuring accurate availability without desyncing the simulation.

* **Precise Burnout Detection & Missed Wakeups:** Instead of relying on `pthread_cond_timedwait` (which can fail due to kernel `timespec` strictness), a dedicated monitor thread acts as a simulation "heartbeat." It broadcasts a condition variable signal every 1 ms, ensuring that no coder sleeps through their cooldown and that any burnout is logged well within the 10 ms tolerance.

* **Log Serialization:** All terminal outputs are protected by a single `print_lock` mutex, guaranteeing that state messages never interleave or corrupt the standard output.


## Thread synchronization mechanisms

This implementation relies heavily on POSIX thread primitives to safely manage shared state across multiple cores:

* `pthread_mutex_t`: Mutexes are used extensively to protect shared resources. An array of `dongle_locks` ensures that a dongle can only be held by one coder at a time. Additionally, `queue_lock`, `sim_lock`, and `print_lock` prevent data races when threads read or mutate the priority queue, the simulation status (`sim_running`), or write to `stdout`.
* `pthread_cond_t`: A condition variable (`arbitrator_cond`) manages the waiting queues efficiently. Instead of busy-waiting (which spikes CPU usage), coders natively sleep using `pthread_cond_wait(&arbitrator_cond, &queue_lock)` when they cannot acquire dongles.
* **Thread-Safe Communication:** Communication between the worker threads and the monitor thread is strictly event-driven. When a coder drops their dongles, they broadcast a signal to awaken any sleeping coders. Simultaneously, the monitor thread continuously checks for burnout conditions safely behind the `sim_lock`. If a burnout occurs, the monitor flips `sim_running` to 0 and broadcasts to all sleeping threads, orchestrating a clean, leak-free exit.


## Resources

* [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
* [Operating Systems: Three Easy Pieces - Concurrency](https://pages.cs.wisc.edu/~remzi/OSTEP/)
* [Introduction To Threads (pthreads) | C Programming Tutorial](https://www.youtube.com/watch?v=ldJ8WGZVXZk)

## AI Usage:

* **Clarification of projects:** AI greatly helped jumpstart my deepdive into the concepts at play in this project.
* **Refactoring for Norm compliance:** Due to the strict limitations of the Norminette, I frequently had to refactor my code, given this AI greatly fastracked this step.
* **Debugging:** When testing this project, AI was an incredible tool to not only test program to its limits, but also to fix its shortcomings.
* **Text overview:** This README, in structure and in prose, was reviewed by AI.
