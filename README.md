# CPSC 457 Assignment 2: Processor Scheduling Algorithms

## Team Members
- [Name 1] - [UCID]
- [Name 2] - [UCID]
*(Add all team member names and UCIDs)*

## Overview
This assignment implements three CPU scheduling algorithm simulators:
1. **First-Come-First-Served (FCFS)** with varying dispatcher latency
2. **Round Robin (RR)** with varying quantum sizes
3. **Multi-Level Feedback Queue (MLFQ)** with 3 priority queues

## File Structure
```
.
├── a2p1.c                      # FCFS simulator
├── a2p2.c                      # Round Robin simulator
├── a2p3.c                      # MLFQ simulator
├── inputfile1.csv              # Input data (1000 processes)
├── plot_results.py             # Python script to generate plots
├── Makefile                    # Build automation
├── test_simulation.sh          # Testing script
└── README.md                   # This file
```

## Quick Start

### Option 1: Using Makefile (Recommended)
```bash
# Compile all programs
make

# Run all simulations
make runall

# Generate plots
make plots

# Clean up
make clean
```

### Option 2: Manual Compilation and Execution

**Part 1: FCFS**
```bash
gcc -O2 a2p1.c -o a2p1
./a2p1 < inputfile1.csv
```

**Part 2: Round Robin**
```bash
gcc -O2 a2p2.c -o a2p2
./a2p2 < inputfile1.csv
```

**Part 3: MLFQ**
```bash
gcc -O2 a2p3.c -o a2p3
./a2p3 < inputfile1.csv
```

### Option 3: Using Test Script
```bash
chmod +x test_simulation.sh
./test_simulation.sh
```

## Output Files

### Part 1 (FCFS)
- `fcfs_results.csv` - Summary metrics for each latency (1-200)
- `fcfs_results_details.csv` - Detailed per-process results for each latency

### Part 2 (Round Robin)
- `rr_results.csv` - Summary metrics for each quantum size (1-200)
- `rr_results_details.csv` - Detailed per-process results for each quantum

### Part 3 (MLFQ)
- Terminal output with final metrics

### Plots
- `fcfs_plot.png` - FCFS individual metric plots
- `fcfs_combined_plot.png` - FCFS all metrics on one graph
- `rr_plot.png` - RR individual metric plots
- `rr_combined_plot.png` - RR all metrics on one graph

## Implementation Details

### Part 1: FCFS Algorithm
- Non-preemptive scheduling
- Processes executed in arrival order
- Dispatcher latency ranges from 1 to 200 time units
- Ties broken by process ID (lower ID first)

**Key Metrics Calculated:**
- Throughput: processes/time unit
- Average Waiting Time
- Average Turnaround Time
- Average Response Time

### Part 2: Round Robin Algorithm
- Preemptive scheduling with time quantum
- Fixed dispatcher latency of 20 time units
- Quantum size varies from 1 to 200 time units
- Ready queue implemented as circular queue

**Algorithm Flow:**
1. New processes enter ready queue
2. Process at front of queue executes for quantum or until completion
3. If not completed, process moves to back of queue
4. Repeat until all processes complete

### Part 3: Multi-Level Feedback Queue
- Three-level priority queue system:
  - **Q1**: RR with quantum = 40 (highest priority)
  - **Q2**: RR with quantum = 80 (medium priority)
  - **Q3**: