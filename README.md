_This project has been created as part of the 42 curriculum by ikolokou_

# Description
Codexion is a twist on the Philosopher's problem about concurrency. We need to simulate a circle of coders that share dongles with the ones next to them and they need to compile with the two dongles, put them down, debug, refactor and repeat. If it takes too much time waiting for the dongles, then they burn out. Simulation ends either with all compiles done without any burnout or one of them burning out.

# Instructions
1. Run `make`
2. Run `./codexion` with the following arguments in that order:
```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```
3. If provided you can run the custom checker to check the output:
```sh
./count_compiles.sh <arguments>
```

# Resources
- [Introduction to Threads in C](https://code-vault.net/course/6q6s9eerd0:1609007479575/lesson/18ec1942c2da46840693efe9b51d86a8)

_AI has been used creatively. That means it was prompted to guide me and not give me out the solutions outright. It has also been a source to deepen my understanding in threads._

# Blocking cases handled

Deadlock has been prevented with the following ways:
- Coders with an odd ID take the lower dongle first, while coders with an even ID the higher first.
- Single coder case is handled as a special case: Take the singular dongle and sleep until you burn out.
- A scheduler schedules in which order the coders can take the dongles, FIFO (First In First Out) or EDF (Earliest Deadline First). Now each coder requests for the dongle and they are put in a heap, with the top being who should get it more imminently.
- Burnout is detected with a monitor thread. It runs every 500 micro-seconds (half of a millisecond) and it checks if each coder has completed their needed compiles or if someone has burnout.  

# Thread synchronization mechanisms

To make sure there are no race conditions, all of the variables and prints have been protected with some kind of mutex. Print has a specific function called `safe_pint`. What this essentially does, is to lock the print mutex, print and then unlock. That is done because it's possible if two threads try to print at the same time, they can override each other and print garbage. For example:

Thread 1 wants to print: `1500 1 is compiling`
Thread 2 wants to print: `1500 2 is debugging`
If there is no mutex, it could be very possible to print:
`1500 1 is combuigling`

For dongles, since they have a cooldown and a heap now, I made them their own struct with their own muteces and conds. So instead of each coder trying to grab a dongle. each dongle can `broadcast` their availability and depending on the heap let the coder grab it.