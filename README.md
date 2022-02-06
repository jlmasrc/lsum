# lsum
C library for large sums of floating point numbers with controlled round-off error.

Features:
* Online, non-recursive pairwise summation (cascade summation)
* Kahan summation

See psum_test.c and ksum_test.c for pairwise and Kahan summation examples.

To compile all tests:

```
make
```

To run a benchmark of all tests:

```
make benchmark
```

To change the length of the benchmark, edit size_bm.h.
