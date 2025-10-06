# CPSC 457 Assignment 2: CPU Scheduling Simulators

## Student Information
- **Name:** Eyuel Kahsay
- **UCID:** 30181884

## Overview

This assignment implements three CPU scheduling algorithms in C and compares their performance using a dataset of 1000 processes.

**Algorithms Implemented:**
1. **FCFS (First-Come-First-Served)** - Non-preemptive, processes run in arrival order
2. **Round Robin** - Preemptive with time quantum, circular queue implementation
3. **Multi-Level Feedback Queue** - Three priority queues (Q1: RR-40, Q2: RR-80, Q3: FCFS)

## Repository Contents

```
├── a2p1.c                    # FCFS scheduler
├── a2p2.c                    # Round Robin scheduler
├── a2p3.c                    # MLFQ scheduler
├── inputfile1.csv            # Input data (1000 processes)
├── Makefile                  # Build automation
├── plot_results.py           # Visualization script
├── test_simulation.sh        # Testing script
└── README.md                 # This file
```

## Quick Start

**Compile and run everything:**
```bash
make runall
```

**Generate plots:**
```bash
python3 plot_results.py
```

**Clean up:**
```bash
make clean
```

## Manual Execution

```bash
# Part 1: FCFS
gcc -O2 a2p1.c -o a2p1
./a2p1 < inputfile1.csv

# Part 2: Round Robin
gcc -O2 a2p2.c -o a2p2
./a2p2 < inputfile1.csv

# Part 3: MLFQ
gcc -O2 a2p3.c -o a2p3
./a2p3 < inputfile1.csv
```

## Algorithm Details

### FCFS
Tests dispatcher latency from 1-200 time units. Each latency value runs a complete simulation of all 1000 processes. Outputs detailed per-process results and summary metrics for each latency.

**Key finding:** Performance degrades linearly with latency. Throughput drops from ~0.0097 at latency=1 to ~0.0033 at latency=200.

### Round Robin
Tests quantum sizes from 1-200 with fixed dispatcher latency of 20. Uses circular queue to manage ready processes.

**Key finding:** Small quantums (1-10) cause excessive context switching. Sweet spot around 50-100. Large quantums (150+) behave like FCFS.

### MLFQ
Three-level priority queue system. New processes start in Q1. If they don't finish within the quantum, they demote to the next queue.
- Q1: RR with quantum=40 (highest priority)
- Q2: RR with quantum=80
- Q3: FCFS (lowest priority)

**Key finding:** Balances interactive and batch processes. Short processes complete quickly in Q1, long processes settle in Q3.

## Output Files

**FCFS:**
- `fcfs_results.csv` - Summary metrics (201 rows: header + 200 latencies)
- `fcfs_results_details.csv` - Per-process results (200,001 rows)

**Round Robin:**
- `rr_results.csv` - Summary metrics (201 rows: header + 200 quantums)
- `rr_results_details.csv` - Per-process results (200,001 rows)

**MLFQ:**
- Terminal output with final metrics

## Implementation Notes

### Important Details
- Dispatcher latency added BEFORE each process execution (as specified)
- Tie-breaking by process ID when arrival times are equal
- Response time taken directly from input data
- New arrivals checked after each execution slice (critical for RR/MLFQ)

### Challenges Solved
- **Queue management:** Ensuring new arrivals get added at the right time during execution
- **MLFQ priority:** Always check Q1→Q2→Q3 in order
- **Timing calculations:** Start time must account for dispatcher latency

## Testing

Run the automated test script:
```bash
chmod +x test_simulation.sh
./test_simulation.sh
```

Verifies compilation, execution, and output file correctness.

## Plots

The Python script generates 4 visualization files:
- `fcfs_plot.png` - FCFS metrics vs latency
- `fcfs_combined_plot.png` - All FCFS metrics on one graph
- `rr_plot.png` - RR metrics vs quantum
- `rr_combined_plot.png` - All RR metrics on one graph

**Requirements:** `matplotlib`, `pandas`, `numpy`
```bash
pip3 install matplotlib pandas numpy
```

## Performance Results

| Algorithm | Throughput | Avg Wait Time | Notes |
|-----------|------------|---------------|-------|
| FCFS (latency=1) | 0.009710 | 48,947 | Best case |
| FCFS (latency=200) | 0.003311 | 148,547 | Worst case |
| RR (quantum=1) | 0.000467 | 1,427,847 | Too much overhead |
| RR (quantum=200) | 0.008197 | 58,457 | Approaching FCFS |
| MLFQ | 0.006835 | 92,759 | Balanced approach |

## Running on University Servers

Tested and verified on cslinux.ucalgary.ca:
```bash
ssh eyuel.kahsay@cslinux.ucalgary.ca
cd cpsc457_a2
make runall
```

## Submission Checklist

- [x] All programs compile without warnings
- [x] Programs run correctly on university servers
- [x] Output format matches specification
- [x] Git repository with commit history
- [x] Code is commented and organized
- [x] Plots generated and included in PDF report
- [x] Reflections written for all three parts

## References

- CPSC 457 Lecture Slides (CPU Scheduling)
- Operating System Concepts - Silberschatz, Galvin, Gagne
- Assignment 2 Specification Document