## Dynamic Greedy Algorithm
Finds a matching using the greedy algorithm and presents an efficient solution to compute a matching with the addition of a single edge (should return the greedy solution, which is a 1/2 approximation).
A sketch of the algorithm can be found here: https://www.overleaf.com/read/wdwsrjmxdpbw

### Benchmarks
For $b=6$ case.
| Optimization  | Runtime (`normal_small_minus_one.mtx`) | Runtime (`normal_big_minus_one.mtx`) |
| ------------- | -----------------------| --------------------|
| RERUN GREEDY | 8 us | 9 us |
| Vanilla | 282 us |  148863 us|
| Carry around pointers instead of actual memory for vertices/edges | |
| Use BST instead of vector to store adjacency lists | |
| Reduce branch misprediction in update step | |
