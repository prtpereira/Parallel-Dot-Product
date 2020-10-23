# Parallel Matrix-matrix multiplication

- Characterization of performance bottlenecks on multiple computing platforms and/or on the
code profiling and its performance analysis on that platform.

- Develop different single threaded implementations of the dot-product function, with a
triple nested loop, exploring two alternative combinations of the index order: (1) i-k-j
and (2) j-k-i. For each alternative implementation, the access to the elements of either A or B (or both) will be row by row, or column by column, which may impact performance

- Modify dot-product function to be executed on a cluster with all SMX of a GPU Kepler and in all cores of the Intel Knights Landing many-core server.
