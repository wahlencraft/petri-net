# Petri Net
In this repository I have written a basic implementation of 
[petri nets](https://en.wikipedia.org/wiki/Petri_net).

Petri Nets can be used to model a distributed system and verify some of it's 
properties. In my implementation it is possible to verify:
1. Liveness
    - Can this net reach a deadlock?
2. Boundedness
    - View the maximum number of tokens that occupied a place
    - Set a maximum number of tokens per place, throw error if this is broken.
3. Reachability
    - Did the net reach a specific state?
    - Throw error if a specific state is reached

For examples of how to use the verifier see the last test case in `tests/test.cpp`.

### Notes
I made this project mostly as an excuse to practice C++. One of the core goals
was to learn to write more efficient code which executes faster. Because of
this I spent a lot of time benchmarking. Some of these benchmarks can be found
in the benchmarks folder.

An interesting find was that most of the execution time was spent allocating
memory. By minimizing the constructor calls I was able to greatly decrease the
execution time. This is best seen in commit `fc589de` after which one test ran
3.7 times faster!

Finally I built a thread-pool and could execute multiple verification paths
simultaneously. This resulted in a speedup of 11.8 times on my machine.
Using multiple threads works better for larger problems, small nets actually
gets slower.

