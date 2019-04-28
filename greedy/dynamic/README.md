## Dynamic Greedy Algorithm
Finds a matching using the greedy algorithm and presents an efficient solution to compute a matching with the addition of a single edge (should return the greedy solution, which is a 1/2 approximation).
A sketch of the algorithm can be found here: https://www.overleaf.com/read/wdwsrjmxdpbw

### Benchmarks
For `b=6` case.


| Optimization  | Runtime (`normal_small_minus_one.mtx`) | Runtime (`normal_big_minus_one.mtx`) |
| ------------- | ---------------------------------------| -------------------------------------|
| RERUN GREEDY  |    8 us |     121101 us |
| Vanilla | 13 us |  222 us|
| Optimize finding max weight unmatched edge by exploting sortedness of list | 12 us | 166 us |
| Add `-flto` flag | 6 us | 74 us |
