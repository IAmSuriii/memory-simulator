# Memory Management Simulator

## Overview
This project simulates important memory management concepts used in operating systems.
It is written in C++ and demonstrates how memory allocation, virtual memory, and cache work.

This is only a simulation and does not use real system memory.

---

## Features
- Physical memory simulation
- First Fit, Best Fit, and Worst Fit allocation
- Memory free and block merging
- Internal and external fragmentation calculation
- Buddy memory allocation system
- Virtual memory using paging
- L1 and L2 cache simulation
- Integration of Virtual Memory and Cache

---

## Project Structure
src/
├── allocator/ // First, Best, Worst Fit allocator
├── buddy/ // Buddy allocation system
├── cache/ // Cache simulation
├── virtual_memory/ // Virtual memory simulation
└── main.cpp // Integration of VM and Cache

---

## How to Run

### Run Memory Allocator
Compile:
g++ src/allocator/allocator_main.cpp -o allocator


Run:
./allocator


After running, you can type:
malloc 100
malloc 200
dump
free 1
stats
exit



---

### Run Virtual Memory + Cache Integration
Compile:
g++ src/main.cpp src/cache/cache.cpp src/virtual_memory/vm.cpp -o memsim


Run:
./memsim


Enter virtual addresses (between 0 and 255).
Enter `-1` to exit.

Example input:
0
16
32
0
-1

---

## Notes
- Cache access happens only after virtual address translation
- Buddy allocator and normal allocator are implemented separately
- The project follows the order described in the given PDF

---

## Conclusion
This project demonstrates how an operating system manages memory using allocation strategies,
paging-based virtual memory, and cache hierarchy.