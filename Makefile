# Makefile for CPSC 457 Assignment 2
# CPU Scheduling Simulators

CC = gcc
CFLAGS = -O2 -Wall -Wextra
INPUT = inputfile1.csv

# Targets
all: a2p1 a2p2 a2p3

# Part 1: FCFS
a2p1: a2p1.c
	$(CC) $(CFLAGS) a2p1.c -o a2p1

# Part 2: Round Robin
a2p2: a2p2.c
	$(CC) $(CFLAGS) a2p2.c -o a2p2

# Part 3: MLFQ
a2p3: a2p3.c
	$(CC) $(CFLAGS) a2p3.c -o a2p3

# Run Part 1
run1: a2p1
	./a2p1 < $(INPUT)

# Run Part 2
run2: a2p2
	./a2p2 < $(INPUT)

# Run Part 3
run3: a2p3
	./a2p3 < $(INPUT)

# Run all simulations
runall: run1 run2 run3

# Generate plots
plots:
	python3 plot_results.py

# Clean up
clean:
	rm -f a2p1 a2p2 a2p3
	rm -f fcfs_results.csv fcfs_results_details.csv
	rm -f rr_results.csv rr_results_details.csv
	rm -f *.png

# Clean and rebuild
rebuild: clean all

# Help
help:
	@echo "CPSC 457 Assignment 2 - Makefile Commands"
	@echo "=========================================="
	@echo "make          - Compile all programs"
	@echo "make a2p1     - Compile FCFS simulator"
	@echo "make a2p2     - Compile Round Robin simulator"
	@echo "make a2p3     - Compile MLFQ simulator"
	@echo "make run1     - Run FCFS simulation"
	@echo "make run2     - Run Round Robin simulation"
	@echo "make run3     - Run MLFQ simulation"
	@echo "make runall   - Run all simulations"
	@echo "make plots    - Generate plots from results"
	@echo "make clean    - Remove executables and output files"
	@echo "make rebuild  - Clean and recompile"
	@echo "make help     - Show this help message"

.PHONY: all run1 run2 run3 runall plots clean rebuild help