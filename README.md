# Hash Tables: Collision Handling and Performance

This project compares two collision-handling strategies:
- Linear probing
- Separate chaining

## Files
- `hash_tables.h` - implementations of both hash tables
- `main.cpp` - benchmark driver
- `hash_benchmark` - compiled executable
- `benchmark_results.csv` - sample benchmark results
- `sample_output.txt` - sample terminal output

## Compile
```bash
g++ -std=c++17 -O2 main.cpp -o hash_benchmark
```

## Run
```bash
./hash_benchmark
```

## What the program measures
- Insertion time
- Search time
- Number of collisions
- Average insertion probes
- Average search probes
- Performance at load factors 0.50, 0.70, 0.80, 0.90, and 0.95

## Notes
rerun the program on your own machine for your own measurements.
