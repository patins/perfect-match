## Dynamic Greedy Algorithm 
Finds a matching using the greedy algorithm and presents an efficient solution to compute a matching with the addition of a single edge (should return the greedy solution, which is a 1/2 approximation).
A sketch of the algorithm can be found here: https://www.overleaf.com/read/wdwsrjmxdpbw

### Benchmarks
| Optimization  | Runtime (`normal.mtx`) | Runtime (`GA*.mtx`) | Total Weight |
| ------------- | -----------------------| --------------------| -------------|
| Vanilla | | | | 
| Carry around pointers instead of actual memory for vertices/edges | | | 
| Use BST instead of vector to store adjacency lists | | | 
| Reduce branch misprediction in update step | | | 
