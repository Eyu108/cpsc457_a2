# CPSC 457 Assignment 2: CPU Scheduling Simulators

## Student Information
- **Name:** Eyuel Kahsay
- **UCID:** 30181884

## Overview

This assignment implements three CPU scheduling algorithms in C to compare their performance. The key challenge was properly aggregating threads by process ID (PID) rather than treating each thread as a separate process.

**Algorithms:**
1. **FCFS** - Non-preemptive, processes execute in arrival order
2. **Round Robin** - Preemptive with time quantum, uses circular queue
3. **MLFQ** - Three priority levels with different scheduling policies

## Quick Start

```bash
# Compile and run everything
make runall

# Generate plots
python3 plot_results.py

# Clean up
make clean
```

## Files

```
├── a2p1.c                    # FCFS scheduler
├── a2p2.c                    # Round Robin scheduler
├── a2p3.c                    # MLFQ scheduler
├── inputfile1.csv            # Input data (1000 threads, 50 processes)
├── Makefile                  # Build system
├── plot_results.py           # Generates plots
├── test_simulation.sh        # Automated testing
└── README.md                 # This file
```

## Implementation Details

### The PID Aggregation Problem

The biggest challenge was understanding that the input file contains **1000 threads** across **50 processes** (PIDs 1-50), not 1000 separate processes. Each PID can have multiple threads that need to be aggregated into per-process metrics.

**Per-Process Metrics:**
- **Turnaround Time** = Latest thread finish - Earliest thread arrival (for that PID)
- **Waiting Time** = Turnaround - Sum of all thread burst times (for that PID)
- **Response Time** = Earliest first response - Earliest arrival (for that PID)

The averages are then calculated over the 50 unique processes, not the 1000 threads.

### FCFS Implementation

Tests dispatcher latency from 1-200 time units. For each latency value, runs a complete simulation of all threads, then aggregates by PID to get per-process metrics.

**Key finding:** Performance degrades linearly with latency. As overhead increases, throughput drops proportionally since we're adding latency before each thread execution.

### Round Robin Implementation

Tests quantum sizes 1-200 with fixed dispatcher latency of 20. Uses a circular queue for ready processes.

**Key finding:** Very small quantums (1-10) have terrible performance due to constant context switching. The sweet spot for this workload seems to be around 50-100. Beyond ~150, it starts behaving more like FCFS since most threads complete in one quantum.

### MLFQ Implementation

Three queues with decreasing priority:
- **Q1:** Quantum=40 (highest priority)
- **Q2:** Quantum=80 (medium priority)
- **Q3:** FCFS (lowest priority)

New threads always enter Q1. If they don't finish within their quantum, they demote to the next queue. This gives short processes better response time while still completing long-running processes.

**Implementation note:** Always check Q1→Q2→Q3 in that order, and remember to check for new arrivals after each execution slice to maintain proper priority.

## Response Time Calculation

This was tricky. The "Time until first Response" column in the input is when the response happens **during execution**, not from arrival. So:

```
First Response Time (FRT) = Start Time + Time_until_first_Response
Response Time = FRT - Arrival Time
```

For processes with multiple threads, we use the earliest FRT across all threads minus the earliest arrival time for that PID.

## Running Individual Parts

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

## Output Files

**FCFS:**
- `fcfs_results_details.csv` - Per-process results for each latency (10,000 rows: 200 latencies × 50 processes)
- `fcfs_results.csv` - Average metrics per latency (200 rows)

**Round Robin:**
- `rr_results_details.csv` - Per-process results for each quantum (10,000 rows)
- `rr_results.csv` - Average metrics per quantum (200 rows)

**MLFQ:**
- Terminal output with final averaged metrics

## Testing

Used the TA's small test case (5 threads, 4 PIDs) to verify the logic:

```bash
gcc -O2 a2p1.c -o a2p1
./a2p1 < test_input_small.csv
```

For latency=2, the expected output was:
```
Throughput=0.108108, Avg_Wait=9.00, Avg_TAT=16.00, Avg_RT=8.50
```

My results matched closely, confirming the aggregation logic was correct.

## Debugging Process

Initially had issues where:
1. Turnaround time was 10x too small - forgot to aggregate thread burst times
2. Start time was showing as larger than finish time - was using sum of waiting times instead of actual first start time
3. Response time calculation was wrong - needed to track first response time properly

Fixed by adding proper tracking of:
- `first_start` - when the first thread of each PID starts
- `total_burst` - sum of all thread burst lengths for each PID
- Calculating waiting as `Turnaround - Total_Burst` instead of summing individual waits

## Performance Results

Results vary based on the specific input file, but general patterns:

**FCFS:**
- Linear degradation as latency increases
- Throughput inversely proportional to latency
- Waiting time increases linearly

**Round Robin:**
- Poor performance at quantum=1 (excessive context switching)
- Improves and stabilizes as quantum increases
- Converges toward FCFS behavior at large quantums

**MLFQ:**
- Balanced approach between response time and throughput
- Better than FCFS for mixed workloads
- Some processes may experience longer waits in lower queues

## Plots

The Python script generates visualization showing how metrics change with:
- Latency (for FCFS)
- Quantum size (for Round Robin)

Each plot includes throughput, average waiting time, average turnaround time, and average response time.

## Code Quality

Focused on:
- **Meaningful names:** `earliest_arrival`, `latest_finish`, `ready_queue`
- **Clear structure:** Separate functions for simulation vs aggregation
- **Comments:** Explaining the trickier parts like PID aggregation
- **Efficiency:** O(n) simulation, proper data structures

## Challenges

The main challenge was understanding the per-process aggregation requirement. The assignment mentions "threads with the same PID" but it wasn't immediately obvious that this fundamentally changes how metrics are calculated. Once I understood that waiting time = turnaround - total_burst (not sum of individual thread waits), everything fell into place.

## Testing on University Servers

Everything compiles and runs on cslinux.ucalgary.ca:

```bash
ssh eyuel.kahsay@cslinux.ucalgary.ca
cd cpsc457_a2
make runall
```

No special dependencies needed beyond standard C libraries and Python with matplotlib/pandas for plotting.

## Submission Checklist

- [x] Code compiles without warnings
- [x] Programs run correctly on university servers
- [x] Output format matches specification
- [x] PID aggregation implemented correctly
- [x] Git repository with commit history
- [x] Code commented and organized
- [x] Plots generated
- [x] README complete

## References

- CPSC 457 Lecture Slides
- TA's FAQ document on PID aggregation
- Operating System Concepts - Silberschatz, Galvin, Gagne