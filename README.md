_This project has been created as part of the 42 curriculum by ikolokou._

# Codexion

A multithreaded simulation in C based on the classic
[dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem),
built with POSIX threads, mutexes and condition variables.

Coders sit in a circle with one USB dongle between each pair of neighbours. To
**compile**, a coder needs both of their neighbouring dongles. After compiling
they put the dongles down, **debug**, **refactor**, and start again. A coder
who goes too long without starting a new compile **burns out**.

The simulation ends when either:
- every coder has compiled the required number of times (`all coders finished`), or
- a coder burns out (`<id> burned out`).

## Features

- **No deadlock.** Coders pick up their dongles in an order that rules out circular waiting.
- **Pluggable scheduling.** Each dongle keeps a small priority queue of requests,
  served either **FIFO** (first come, first served) or **EDF** (earliest deadline
  first: the coder closest to burning out goes first).
- **Dongle cooldown.** A released dongle can't be picked up again until a configurable cooldown has passed.
- **Data-race free.** All shared state is protected by mutexes. ThreadSanitizer reports no races (`make debug`).
- **Clean shutdown.** No log line is printed after the final message, sleeping
  threads wake up as soon as the simulation ends, and every thread is joined and
  every resource freed (Valgrind reports no leaks).

## Build

Requirements: a C compiler (`cc`), `make` and a POSIX system (Linux or macOS).

```sh
make          # builds ./codexion
make debug    # rebuilds with -g -fsanitize=thread
make norm     # runs norminette (42 coding style)
make clean    # removes object files
make fclean   # removes object files and the binary
```

## Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders, and of dongles (≥ 1) |
| `time_to_burnout` | ms a coder can go without starting a compile before burning out |
| `time_to_compile` | ms spent compiling (holding both dongles) |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `number_of_compiles_required` | The simulation succeeds once every coder has compiled this many times |
| `dongle_cooldown` | ms a dongle stays unusable after being released |
| `scheduler` | `fifo` or `edf` |

All numbers must be non-negative integers. Invalid input prints a usage message and exits with status `1`.

### Example

```console
$ ./codexion 5 800 200 200 200 3 0 edf
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
0 2 has taken a dongle
0 4 has taken a dongle
0 4 has taken a dongle
0 4 is compiling
...
1800 all coders finished
```

Each line has the format `<ms since start> <coder id> <state>`.

## How it works

### Threads

- **One thread per coder** runs the cycle: take dongles → compile → debug → refactor.
- **One monitor thread** checks every 0.5 ms whether any coder has burned out or
  whether everyone has finished, and stops the simulation when either happens.

### Deadlock prevention

Coder `N` sits between dongle `N-1` (left) and dongle `N % n` (right). Coders
with an odd id pick up the left dongle first and coders with an even id pick up
the right one first, so the coders can never all be waiting on each other in a
circle. A lone coder (`n = 1`) is handled separately: they take the only dongle
and wait until they burn out.

### Scheduling

Every dongle has a small priority queue (capacity 2, since each dongle has only
two neighbours). A coder who wants a dongle adds a request with a priority key:

- **FIFO**: the key is the time of the request, so requests are served in arrival order.
- **EDF**: the key is `last_compile_start + time_to_burnout`, so the coder closest to burning out is served first.

A coder gets the dongle only when it is free, its cooldown has passed, **and**
the coder's request is at the front of the queue. Waiting coders sleep on a
condition variable, which is signalled whenever the dongle is released.

### Synchronization

| Lock | Protects |
|---|---|
| `sim_mtx` | The `sim_running` flag |
| `print_mtx` | stdout, so log lines never interleave |
| `coder.mtx` | A coder's `last_compile_start` and `compile_count` |
| `dongle.mtx` + `dongle.cond` | A dongle's availability, release time and request queue |

When the simulation ends, the stop flag is cleared and the final message is
printed while `print_mtx` is held. Every other log line checks the flag under
the same lock, so nothing can be printed after `burned out` or
`all coders finished`.

## Project structure

```
Codexion/
├── include/
│   └── codexion.h     # types, constants and prototypes
├── src/
│   ├── main.c         # argument parsing and entry point
│   ├── init.c         # allocation and teardown of coders/dongles
│   ├── simulation.c   # thread start-up, joining and cleanup
│   ├── coder.c        # coder routine: compile, debug, refactor
│   ├── dongle.c       # acquiring/releasing dongles
│   ├── scheduler.c    # FIFO/EDF priority keys and request queue
│   ├── monitor.c      # burnout/completion detection and shutdown
│   └── utils.c        # time, logging and interruptible sleep
└── Makefile
```

## Resources

- [Introduction to Threads in C (CodeVault)](https://code-vault.net/course/6q6s9eerd0:1609007479575/lesson/18ec1942c2da46840693efe9b51d86a8)
- [Dining philosophers problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest deadline first scheduling (Wikipedia)](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

AI was used as a tutor: prompted to guide me and explain threading concepts,
not to hand over solutions.
