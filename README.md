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

