*This project has been created as part of the 42 curriculum by bmoura-s.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads. A group of coders sits around a table in a co-working hub. Between each pair of neighbours lies one USB dongle (as many dongles as coders), and a coder needs **both** adjacent dongles to compile. After compiling, a coder debugs, then refactors, then tries to compile again. A coder who does not *start* a compile within `time_to_burnout` milliseconds of the start of their previous compile (or of the simulation) burns out, and the simulation stops. It also stops when every coder has compiled at least `number_of_compiles_required` times.

The goal is to share the dongles safely and fairly: no data races, no deadlock, no starvation, a serialized and correctly formatted log, and a burnout reported within 10 ms of when it really happens. Dongles are handed out by a scheduler, either `fifo` (first request first) or `edf` (earliest deadline first, deadline = `last_compile_start + time_to_burnout`), implemented on top of a hand-written binary heap.

## Instructions

### Compilation

A `Makefile` is provided at the root of the repository.

* `make`: builds the `codexion` executable (`cc -Wall -Wextra -Werror -pthread`).
* `make clean`: removes the object files.
* `make fclean`: removes the object files and the executable.
* `make re`: `fclean` followed by a full rebuild.

The objects depend on `includes/codex.h`, so editing the header rebuilds everything, and running `make` twice never relinks.

### Execution

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | Number of coders, and also of dongles (at least 1) |
| `time_to_burnout` | ms a coder may go without *starting* a compile |
| `time_to_compile` | ms spent compiling (two dongles held) |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `number_of_compiles_required` | The simulation stops once every coder has *finished* at least this many compiles. `0` means the target is already met, so the program exits immediately without output |
| `dongle_cooldown` | ms a released dongle stays unavailable |
| `scheduler` | exactly `fifo` or `edf` |

All eight arguments are mandatory. The numeric ones must be plain non-negative integers that fit in an `int` (digits only, one optional leading `+`). Empty strings, spaces, signs other than `+`, decimals, trailing characters and overflowing values are rejected, as is any scheduler other than `fifo` or `edf`. On invalid input the program prints `Error: ...` on **stderr** and exits with status 1.

**Example (EDF scheduler):**

```
./codexion 5 800 200 200 200 7 10 edf
```

**Example (FIFO scheduler):**

```
./codexion 4 600 200 100 100 10 50 fifo
```

### Log format

Every state change is printed as `timestamp_in_ms coder_id message`, with the timestamp relative to the start of the simulation:

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
300 1 is refactoring
...
301 3 burned out
```

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)

A deadlock needs all four Coffman conditions at once. Two are inherent to the problem (mutual exclusion: a dongle has one holder; no preemption: a dongle is only given back voluntarily). The other two are broken on purpose:

* **Hold and wait is broken.** A coder never holds one dongle while waiting for the other. In `take_dongles`, under `queue_lock`, a coder waits until *both* of its dongles are free, off cooldown and granted to it by the scheduler, and then claims both in the same critical section (`dongle_ready[] = -1`). Nobody can be left with one dongle in hand.
* **Circular wait is broken.** The dongle mutexes are always locked in ascending index order. Coder `i` locks dongle `i` then `i + 1`, and the last coder, whose right neighbour is dongle `0`, locks `0` first and then `n - 1`. A resource ordering has no cycles, so even the mutex level cannot deadlock.

### Starvation prevention

Requests are ordered by a priority queue. With `fifo` the priority is a monotonically increasing request ticket, so nobody can be overtaken. With `edf` it is the burnout deadline `last_compile + time_to_burnout`, so the coder closest to burning out is served first, and ties are broken by the ticket so the order is fully deterministic. With feasible parameters no coder is starved. The even-numbered coders also wait 10 ms before their first request, which spreads out the initial rush on the dongles.

### Cooldown handling

Each dongle has a `dongle_ready[]` timestamp: `-1` while held, otherwise the time from which it may be taken again. Releasing a dongle sets it to `now + dongle_cooldown`. Nobody sleeps on a cooldown: a waiting coder simply re-evaluates `now >= dongle_ready` whenever it is woken. Cooldown expiry is not an event anyone signals, so the monitor's periodic broadcast (below) re-wakes the waiters, which bounds the extra delay to about 1 ms.

### Precise burnout detection

A dedicated monitor thread loops every millisecond. It reads each coder's `last_compile` under `sim_lock` and declares burnout as soon as `now - last_compile > time_to_burnout`. In my measurements the message appears 1 to 2 ms after the real deadline, far inside the 10 ms limit, even with 200 coders. The monitor starts checking immediately, with no start-up delay. I chose a polling monitor over `pthread_cond_timedwait` because one simple loop covers burnout, the "everyone compiled enough" condition and the cooldown wake-ups, with a known worst-case latency.

### Log serialization

Every line is printed while holding `print_lock`, so lines never interleave. `print_log` also checks that the simulation is still running *inside* that lock, and the burnout message is printed under the same lock after the running flag has been cleared. Therefore nothing can ever appear after the `burned out` line.

### Clean and prompt shutdown

When the simulation ends (burnout, goal reached, or a thread failing to start) `stop_sim` clears `sim_running`, then broadcasts `arbitrator_cond` while holding `queue_lock`. Coders sleeping in compile/debug/refactor use `ft_usleep`, which checks the flag every 0.5 ms and returns early, so the process exits within a few milliseconds of the stop, even with very long `time_to_debug` values. A coder that was in the middle of acquiring dongles still runs through `drop_dongles`, so no mutex is ever destroyed while locked. If `pthread_create` fails part-way, the threads already started are stopped and joined and the program exits with status 1.

## Thread synchronization mechanisms

| Primitive | Protects |
|---|---|
| `dongle_locks[i]` (`pthread_mutex_t`) | Physical ownership of dongle `i` while it is being used |
| `queue_lock` (`pthread_mutex_t`) | The per-dongle priority queues, `dongle_ready[]`, the ticket counter, and the predicate of `arbitrator_cond` |
| `sim_lock` (`pthread_mutex_t`) | `sim_running`, and each coder's `last_compile` and `compiles` (written by coders, read by the monitor) |
| `print_lock` (`pthread_mutex_t`) | `stdout` |
| `arbitrator_cond` (`pthread_cond_t`) | Coders waiting to be granted their two dongles |

**Lock order.** `queue_lock` and `print_lock` may each be held while taking `sim_lock`; `sim_lock` is a leaf and is never held while taking another lock. The dongle mutexes are locked with no other lock held. There is therefore no cycle between the locks.

**Condition variable.** A coder waits in `while (running && !can_take_dongles()) pthread_cond_wait(...)`. The check and the wait are atomic under `queue_lock`, and every `pthread_cond_broadcast` is made under the same lock, so a wake-up can never be lost. Broadcasts come from three places: a coder releasing its dongles (`drop_dongles`), the monitor's 1 ms heartbeat (cooldown expiry), and `stop_sim` (shutdown).

**Examples of race conditions that are prevented**

* *Two coders taking the same dongle.* Checking that a dongle is free and claiming it happen in a single `queue_lock` critical section, so two coders can never both see it free.
* *Monitor reading while a coder writes.* `last_compile` and `compiles` are only accessed under `sim_lock`, so the monitor never sees a torn or stale value.
* *Output after burnout, or interleaved lines.* The running-flag check and the `printf` share `print_lock`.
* *Lost wake-up at shutdown.* The flag is cleared first and the broadcast is made under `queue_lock`; a coder is either already waiting (and is woken) or has not checked yet (and sees the cleared flag).
* *Mutex destroyed while locked.* A coder that acquires its dongles just as the simulation stops still completes the cycle and releases them (`take_dongles` reports success, `do_compile` always ends with `drop_dongles`), so `cleanup_sim` only destroys unlocked mutexes.

**Communication between coders and monitor.** Coders never call the monitor and the monitor never touches a coder's thread. They communicate only through shared state under `sim_lock` (`last_compile`, `compiles`, `sim_running`) and through `arbitrator_cond`.

## Technical choices

* **One heap per dongle.** Each dongle owns a min-heap of the coders waiting for it (capacity 2, since a dongle has exactly two neighbours). A coder is pushed into the heaps of both its dongles and may take them only if it is at the top of both and both are off cooldown, so the dongle really is granted according to the scheduler. The heap comparison is `compare_coders`: ticket for `fifo`, deadline then ticket for `edf`. Insertion and removal use the usual sift-up / sift-down, and the grant decision is an O(1) peek at the top of each heap.
* **A compile counts when it finishes**, matching "compiled at least N times"; `last_compile` is set when it starts, matching the burnout rule.
* **`gettimeofday`** gives millisecond timestamps; `ft_usleep` sleeps in 0.5 ms slices and is interruptible by the end of the simulation.
* **Single coder.** With one coder there is one dongle, so the coder takes it, waits, and burns out.

## Resources

* [POSIX Threads Programming (LLNL)](https://computing.llnl.gov/tutorials/pthreads/)
* [Operating Systems: Three Easy Pieces, Concurrency](https://pages.cs.wisc.edu/~remzi/OSTEP/)
* [Introduction To Threads (pthreads), C Programming Tutorial](https://www.youtube.com/watch?v=ldJ8WGZVXZk)

### How AI was used

AI was used as a tutor, a reviewer and a debugging aid. I read, tested and understood what it produced before keeping it.

* **Understanding the concepts:** explanations of mutexes, condition variables, Coffman's conditions and priority scheduling to get started.
* **Norm compliance:** help splitting functions and files to fit the Norm.
* **Testing and debugging:** ideas for stress tests and for diagnosing races and timing problems, followed by fixes.
* **Final review pass:** an AI-assisted review of the finished code against the subject, followed by these changes, which I re-read, tested (valgrind, ThreadSanitizer, helgrind, a script that checks the log for overlapping compiles, cooldown violations and burnout latency) and checked with norminette: the strict argument parser (`parse.c`), the interruptible `ft_usleep`, the start-up failure handling and shutdown path (`engine.c`, `routine.c`, `actions.c`), counting a compile when it finishes, removing the monitor's start-up delay, the per-dongle heaps (`queue.c`, `arbiter.c`), and the Makefile.
* **README:** structure and prose reviewed by AI.
