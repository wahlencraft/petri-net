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

Then I built a thread-pool and could execute multiple verification paths
simultaneously. This resulted in a speedup of 11.8 times on my machine (14
hetrogeneous cores) Using multiple threads works better for larger problems,
small nets actually gets slower.

After that I realized that it is equivalent to fire one enabled transition at a
time and to check for possible patterns to fire simultaneously. This makes
finding the next possible states O(n) rather than O(n^2). This resulted in an
enormous speed increase for very large nets. This did however ruin some of the
benefits of multithreading as the program is no longer compute limited. 

At this point I tried to remove all multithreading and the overhead that it
introduces. Compared to the single-threaded tests (with the overhead) it was
obviously significantly faster. Unfortunately quite a few cores were needed to
for the multithreaded code to be faster. Luckily I managed to remove some of
the overhead by first checking if the thread pool was full before enqueuing
another job. If the queue is full, just finish the job in the active thread
instead. Then just 2 cores are needed to be faster then the unthreaded code.
These benchmarks can be found in the `very_large_net.txt` file.

In my final test the code runs roughly 4.5 times faster with many cores
compared to one. It is also interesting to note that it's slightly slower with
16 threads compared to 8. This could be because the last couple of threads are
hyper-threaded or because the test task is simply not large enough to benefit
from more threads.

