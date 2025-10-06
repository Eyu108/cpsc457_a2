#!/bin/bash

# Test script for CPSC 457 Assignment 2
# This script runs all simulations and validates output

echo "=============================================="
echo "CPSC 457 Assignment 2 - Test Script"
echo "=============================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if input file exists
if [ ! -f "inputfile1.csv" ]; then
    echo -e "${RED}Error: inputfile1.csv not found!${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Input file found${NC}"
echo ""

# Compile all programs
echo "Compiling programs..."
echo "----------------------"

gcc -O2 a2p1.c -o a2p1 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ a2p1 compiled successfully${NC}"
else
    echo -e "${RED}✗ a2p1 compilation failed${NC}"
    exit 1
fi

gcc -O2 a2p2.c -o a2p2 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ a2p2 compiled successfully${NC}"
else
    echo -e "${RED}✗ a2p2 compilation failed${NC}"
    exit 1
fi

gcc -O2 a2p3.c -o a2p3 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ a2p3 compiled successfully${NC}"
else
    echo -e "${RED}✗ a2p3 compilation failed${NC}"
    exit 1
fi

echo ""

# Run Part 1: FCFS
echo "Running Part 1: FCFS..."
echo "----------------------"
./a2p1 < inputfile1.csv
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ FCFS simulation completed${NC}"
    
    # Check output files
    if [ -f "fcfs_results.csv" ] && [ -f "fcfs_results_details.csv" ]; then
        lines=$(wc -l < fcfs_results.csv)
        echo "  - fcfs_results.csv: $lines lines"
        
        detail_lines=$(wc -l < fcfs_results_details.csv)
        echo "  - fcfs_results_details.csv: $detail_lines lines"
        
        # Validate: should have 201 lines (1 header + 200 data)
        if [ $lines -eq 201 ]; then
            echo -e "  ${GREEN}✓ Correct number of summary lines${NC}"
        else
            echo -e "  ${YELLOW}⚠ Expected 201 lines, got $lines${NC}"
        fi
        
        # Validate detail file: should have 200001 lines (1 header + 200*1000)
        expected_detail=$((200 * 1000 + 1))
        if [ $detail_lines -eq $expected_detail ]; then
            echo -e "  ${GREEN}✓ Correct number of detail lines${NC}"
        else
            echo -e "  ${YELLOW}⚠ Expected $expected_detail lines, got $detail_lines${NC}"
        fi
    else
        echo -e "${RED}✗ Output files not created${NC}"
    fi
else
    echo -e "${RED}✗ FCFS simulation failed${NC}"
fi
echo ""

# Run Part 2: Round Robin
echo "Running Part 2: Round Robin..."
echo "------------------------------"
./a2p2 < inputfile1.csv
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Round Robin simulation completed${NC}"
    
    # Check output files
    if [ -f "rr_results.csv" ] && [ -f "rr_results_details.csv" ]; then
        lines=$(wc -l < rr_results.csv)
        echo "  - rr_results.csv: $lines lines"
        
        detail_lines=$(wc -l < rr_results_details.csv)
        echo "  - rr_results_details.csv: $detail_lines lines"
        
        # Validate
        if [ $lines -eq 201 ]; then
            echo -e "  ${GREEN}✓ Correct number of summary lines${NC}"
        else
            echo -e "  ${YELLOW}⚠ Expected 201 lines, got $lines${NC}"
        fi
        
        expected_detail=$((200 * 1000 + 1))
        if [ $detail_lines -eq $expected_detail ]; then
            echo -e "  ${GREEN}✓ Correct number of detail lines${NC}"
        else
            echo -e "  ${YELLOW}⚠ Expected $expected_detail lines, got $detail_lines${NC}"
        fi
    else
        echo -e "${RED}✗ Output files not created${NC}"
    fi
else
    echo -e "${RED}✗ Round Robin simulation failed${NC}"
fi
echo ""

# Run Part 3: MLFQ
echo "Running Part 3: MLFQ..."
echo "----------------------"
./a2p3 < inputfile1.csv > mlfq_output.txt
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ MLFQ simulation completed${NC}"
    echo "  Output saved to mlfq_output.txt"
    echo "  Results:"
    cat mlfq_output.txt
else
    echo -e "${RED}✗ MLFQ simulation failed${NC}"
fi
echo ""

# Summary
echo "=============================================="
echo "Test Summary"
echo "=============================================="
echo ""

FILES=("a2p1" "a2p2" "a2p3" "fcfs_results.csv" "fcfs_results_details.csv" "rr_results.csv" "rr_results_details.csv" "mlfq_output.txt")

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${GREEN}✓${NC} $file"
    else
        echo -e "${RED}✗${NC} $file (missing)"
    fi
done

echo ""
echo "Next steps:"
echo "1. Run: python3 plot_results.py (to generate plots)"
echo "2. Review the plots and CSV files"
echo "3. Write your reflections"
echo "4. Compile everything into your PDF report"
echo ""