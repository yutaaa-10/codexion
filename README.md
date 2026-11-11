*This project has been created as part of the 42 curriculum by yukurosa.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads. It models
multiple coders sharing a limited number of USB dongles.

Each coder runs in a separate thread. To compile, a coder must acquire both the
left and right dongles. After compiling, the coder releases both dongles, then
debugs and refactors. A released dongle remains unavailable until its cooldown
has elapsed. A coder burns out if they do not start compiling again before their
deadline.

When multiple coders compete for dongles, one of two scheduling policies decides
which request has priority:

- `fifo`: First In, First Out; requests are served in arrival order.
- `edf`: Earliest Deadline First; the coder with the earliest burnout deadline
  has priority.

Waiting requests are stored in a custom binary-heap priority queue. The
simulation stops when a coder burns out or when every coder has completed the
required number of compilations.

The goal of the project is to coordinate threads and shared resources safely
with mutexes, condition variables, precise timing, and fair scheduling while
preventing deadlocks, starvation, data races, and interleaved logs.

## Instructions

### Compilation

Run the following command from the project root:

```sh
make
```

This creates the codexion executable. The available Makefile rules are:

```sh
make all
make clean
make fclean
make re
```

- `all`: builds the program.
- `clean`: removes object files.
- `fclean`: removes object files and the executable.
- `re`: performs a clean rebuild.

### Execution

The program takes eight mandatory arguments:

```sh
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> \
	<time_to_debug> <time_to_refactor> <number_of_compiles_required> \
	<dongle_cooldown> <scheduler>
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders and dongles |
| `time_to_burnout` | Time from the last compile start until burnout, in milliseconds |
| `time_to_compile` | Compile duration, in milliseconds |
| `time_to_debug` | Debug duration, in milliseconds |
| `time_to_refactor` | Refactor duration, in milliseconds |
| `number_of_compiles_required` | Number of compilations each coder must complete |
| `dongle_cooldown` | Time before a released dongle can be reused, in milliseconds |
| `scheduler` | Scheduling policy: `fifo` or `edf` |

Negative values, non-integer values, out-of-range values, and unsupported
scheduler names are rejected.

### Example

```sh
./codexion 5 3500 200 200 200 10 800 edf
```

In this example, five coders must each complete ten compilations. Compiling,
debugging, and refactoring each take 200 ms, dongle cooldown is 800 ms, and EDF
is used for scheduling.

The log contains the elapsed time since simulation start, the coder ID, and the
new state:

```text
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
```

A burnout is logged as follows:

```text
1241 1 burned out
```

## Blocking cases handled

This implementation addresses deadlocks, starvation, cooldown violations,
delayed burnout detection, data races, and interleaved log output.

### Deadlock prevention

A coder never holds one dongle while waiting for the other. The scheduler
reserves both required dongles atomically only when both are available; if
either is unavailable, neither is reserved.

Whenever two dongle mutexes are needed, they are locked in ascending dongle
index order. Every thread therefore follows the same lock order, preventing a
circular wait caused by opposite lock acquisition orders.

The implementation handles Coffman's four deadlock conditions as follows:

- **Mutual exclusion:** retained because one dongle cannot be used by multiple
  coders simultaneously.
- **Hold and wait:** prevented by reserving both dongles together instead of
  holding one while waiting for another.
- **No preemption:** retained because a compiling coder releases its dongles
  when compilation ends.
- **Circular wait:** prevented by always locking dongle mutexes in index order.

All four conditions are required for deadlock. Breaking hold-and-wait and
circular-wait prevents a deadlock from forming.

### Starvation prevention and scheduling

Waiting requests are managed by a custom binary heap. FIFO uses request arrival
order as its priority. EDF uses the next burnout deadline:

```text
deadline = last_compile_start + time_to_burnout
```

Equal EDF deadlines are resolved by arrival order and then coder ID, making the
policy deterministic. The scheduler selects the highest-priority request among
those whose two dongles are currently available. Coders that use non-overlapping
dongles may therefore compile concurrently.

FIFO prevents an older conflicting request from being repeatedly overtaken by
newer ones. Under EDF, the request with the most urgent burnout deadline is
served first. This provides the required liveness for EDF when the supplied
timings make the workload feasible.

### Cooldown handling

When dongles are released, their next usable time is recorded:

```text
available_at = release_time + dongle_cooldown
```

The scheduler reserves a dongle only when it is not in use and the current time
is at least available_at. Each dongle's in_use and available_at fields are
protected by that dongle's mutex, preventing simultaneous or premature use.

### Precise burnout, completion, and logging

A dedicated monitor thread checks coders approximately every millisecond.
Timing uses CLOCK_MONOTONIC, and a coder burns out when:

```text
current_time - last_compile_start >= time_to_burnout
```

On burnout, the monitor sets the shared stop state and wakes every coder waiting
on the condition variable. The simulation also stops as soon as every coder has
completed the required number of compilations.

Normal state logs and burnout logs are serialized with state_mutex, so two
messages cannot interleave on one line. The same critical section checks the stop
state, suppressing normal log messages after the simulation has stopped.

All coder threads and the monitor thread wait at a start barrier. Once every
thread has been created, a common start_time is set and all threads are
released. This prevents thread creation order from giving some coders an unfair
deadline.

With one coder, only one dongle exists. The coder takes that dongle, waits without
compiling, and eventually burns out at the deadline, as required.

## Thread synchronization mechanisms

The implementation uses pthread_mutex_t and pthread_cond_t to coordinate
coders, the scheduler, dongles, logging, and the monitor. It does not define a
separate custom event type; condition variables combined with the shared
`started, granted, and stopped states provide event-like communication.

### Mutexes

- state_mutex protects stopped, last_compile_start, compile_count, the
  common start time, and serialized log output.
- Each dongle's mutex protects its in_use and available_at fields.
- dispatcher.mutex protects the waiting heap, request arrival order,
  queued, granted, and the start barrier's started state.

For example, a coder updates last_compile_start while holding state_mutex.
The monitor acquires the same mutex before reading it, preventing a data race
between a compile start and burnout detection.

To reserve two dongles, the scheduler locks both dongle mutexes in index order,
checks their usage and cooldown state, and marks both as in use within the same
critical section. Because checking and reservation are atomic with respect to
other coders, two coders cannot acquire the same dongle.

### Condition variable

dispatcher.cond is used for:

- releasing all threads from the start barrier;
- sleeping while a coder waits for dongles;
- re-evaluating requests after resource state changes;
- waking coders whose requests have been granted; and
- waking all waiting coders when the simulation stops.

When dongles are unavailable, a coder uses pthread_cond_timedwait() instead of
busy-waiting. After waking, the coder always checks granted and stopped again,
which also handles spurious wakeups. The timeout ensures cooldown expiration is
re-evaluated even if no thread sends a notification at that exact moment.

### Thread-safe communication

A coder holds dispatcher.mutex while assigning an arrival order and inserting
its request into the heap. The scheduler holds the same mutex while granting a
request and performs the following state transition:

```text
remove request from heap
queued = 0
granted = 1
broadcast the condition variable
```

A waiting coder reacquires dispatcher.mutex before checking granted, so it
observes the scheduler's update safely.

When the monitor detects burnout, it sets stopped while holding state_mutex.
It then releases that mutex before broadcasting on the condition variable. This
avoids holding unrelated mutexes simultaneously and reduces the risk of lock
order inversion.

## Resources

### References

- [POSIX threads overview](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread mutex operations](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [pthread condition variables](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- [clock_gettime](https://man7.org/linux/man-pages/man2/clock_gettime.2.html)
- [Valgrind User Manual](https://valgrind.org/docs/manual/manual-intro.html)
- [Valgrind Helgrind Manual](https://valgrind.org/docs/manual/hg-manual.html)
- [Valgrind DRD Manual](https://valgrind.org/docs/manual/drd-manual.html)
- [Introduction to multithreading in C](https://daeudaeu.com/multithread/)

### Use of AI

AI was used to support design discussions, code review, test-case planning,
debugging, and requirement verification. In particular, it assisted with:

- reviewing parser boundaries and overflow handling;
- reviewing the coder, dongle, and simulation data structures;
- discussing the custom binary heap and FIFO/EDF scheduler design;
- clarifying the responsibilities of mutexes and condition variables;
- reviewing deadlock, starvation, cooldown, and burnout handling;
- debugging the start barrier, linker errors, and Norm errors;
- interpreting Valgrind, Helgrind, and DRD results;
- comparing the implementation with the project requirements; and
- organizing and proofreading this README.

All AI suggestions and code examples were reviewed before use. The resulting
implementation was validated through compilation, Norminette, execution tests,
and Valgrind-based checks.
