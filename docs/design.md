
# Memory Management Simulator – Design Document

---

## 1. Overview

This project simulates an Operating System memory management subsystem in user space.  
It models physical memory allocation, a multilevel CPU cache hierarchy, and virtual memory translation using paging.

The goal of the simulator is to demonstrate understanding of OS-level abstractions such as:
- Memory allocation strategies
- Fragmentation
- Cache behavior
- Virtual-to-physical address translation

---

## 2. Memory Layout and Assumptions

The simulator models physical memory as a contiguous block of addressable memory.  
Memory is represented using software data structures rather than actual hardware memory.  
Each memory block has a starting address, size, and allocation status (free or allocated).

The total memory size is fixed at initialization time (for example, 1024 bytes).  
Memory allocation and deallocation operations update this simulated layout by splitting and merging blocks as required.

### Assumptions

- Memory addresses are byte-addressable.
- All allocations are aligned to a fixed word size to avoid misaligned accesses.
- The simulator does not model hardware-level details such as DRAM banks or memory timing.
- The system assumes a single contiguous physical memory region.
- No concurrent processes are simulated; memory requests are handled sequentially.

### Memory Representation Diagram

+--------------------------------------------------+
| Physical Memory |
+--------------------------------------------------+
| Block 1 | Block 2 | Free Block | Block 3 |
+--------------------------------------------------+
| Used | Used | Free | Used |
+--------------------------------------------------+

vbnet
Copy code

Each block stores metadata such as:

- Start address
- Allocated size
- Requested size (for fragmentation analysis)
- Free/allocated status

This abstraction allows the simulator to track memory usage, fragmentation, and allocation behavior in a controlled environment.

---

## 3. Allocation Strategy Implementations

The simulator implements dynamic memory allocation as a standalone module.  
This module is responsible for managing physical memory using variable-sized blocks and supports multiple allocation strategies.

The allocator operates on a contiguous region of simulated physical memory and maintains a list of free and allocated blocks.

Each allocation request searches the free memory blocks according to the selected strategy and allocates memory by splitting blocks when necessary.

### Implemented Allocation Strategies

#### First Fit

In the First Fit strategy, the allocator selects the first free block that is large enough to satisfy the requested memory size.  
This approach is simple and fast, as it stops searching once a suitable block is found.  
However, it can lead to fragmentation near the beginning of memory.

#### Best Fit

In the Best Fit strategy, the allocator searches all free blocks and selects the smallest block that can accommodate the requested size.  
This strategy attempts to minimize wasted space within allocated blocks but may increase search time and external fragmentation.

#### Worst Fit

In the Worst Fit strategy, the allocator selects the largest available free block for allocation.  
By allocating from the largest hole, it aims to reduce the creation of very small unusable fragments.

### Block Management

When a block larger than the requested size is selected, it is split into:

- An allocated block of the requested size
- A remaining free block

During deallocation, adjacent free blocks are merged (coalesced) to reduce fragmentation and maintain larger contiguous free regions.

### Allocation Interface

The allocator is accessed through a command-line interface supporting operations such as:

- Memory allocation
- Memory deallocation
- Memory state dumping
- Statistics reporting

### Allocation Strategy Diagram

Free Memory Blocks
+--------+------------+----------+
| Free | Free | Free |
+--------+------------+----------+

Request Size = X

First Fit -> selects first suitable block
Best Fit -> selects smallest suitable block
Worst Fit -> selects largest suitable block

vbnet
Copy code

---

## 4. Buddy System Design

The simulator includes an implementation of the Buddy Memory Allocation system as a standalone module.  
The buddy system is designed as an alternative physical memory management strategy and is implemented independently from the variable-sized block allocator.

### Buddy System Overview

The buddy allocator manages memory in blocks whose sizes are powers of two.  
The total memory size is assumed to be a power of two at initialization.  
Allocation requests are rounded up to the nearest power of two before allocation.

Memory is conceptually represented as a binary tree, where each node corresponds to a memory block.  
Two blocks of the same size that originate from the same parent block are called buddies.

### Allocation Process

When a request is made:

- The requested size is rounded up to the nearest power of two.
- The allocator searches for a free block of that size.
- If no block of the required size is available, a larger block is recursively split into two buddy blocks.
- One buddy is allocated, and the other is added back to the appropriate free list.

### Deallocation and Coalescing

When a block is freed:

- The allocator checks whether its buddy is also free.
- If both buddies are free, they are merged (coalesced) into a larger block.
- This merging continues recursively until no further coalescing is possible.

The buddy of a block is identified using the XOR operation on the block’s starting address and its size.

### Free List Management

Separate free lists are maintained for each block size (power of two).  
This allows efficient lookup, allocation, and merging operations.

### Design Choice

The buddy allocator is implemented as a separate module and is not integrated into the virtual memory pipeline.  
The virtual memory simulation operates on fixed-size frames and does not require direct interaction with the buddy allocator.

### Buddy System Diagram

Initial Block (1024)
|
v
Split
/
512 512
|
Split
/
256 256

vbnet
Copy code

---

## 5. Cache Hierarchy and Replacement Policy

The simulator implements a multilevel CPU cache hierarchy as a standalone module.  
The cache system models two levels of cache—L1 and L2—to study the impact of caching on memory access performance.

### Cache Organization

Both L1 and L2 caches are implemented as direct-mapped caches.  
Each cache consists of a fixed number of cache lines, where each line stores:

- A valid bit
- A tag identifying the memory block

The cache index and tag are derived from the physical memory address using the block size and number of cache lines.

### Cache Access Procedure

For every memory access:

- The physical address is checked in the L1 cache.
- If an L1 hit occurs, the access completes immediately.
- On an L1 miss, the L2 cache is checked.
- If an L2 hit occurs, the block is promoted to L1.
- If both caches miss, the block is fetched from main memory and inserted into both L2 and L1.

### Replacement Policy

Since the cache is direct-mapped, replacement is implicit.  
Each memory block maps to exactly one cache line.  
When a new block is inserted, it automatically replaces the existing entry.

### Cache Hierarchy Diagram

lua
Copy code
          CPU
           |
           v
       +--------+
       |  L1    |
       +--------+
           |
       L1 Miss
           v
       +--------+
       |  L2    |
       +--------+
           |
       L2 Miss
           v
     Main Memory
yaml
Copy code

---

## 6. Virtual Memory Model

The simulator implements a paging-based virtual memory system as a standalone module.

### Virtual Address Space

The virtual address space is divided into fixed-size pages.  
Each virtual address consists of:

- A page number
- An offset within the page

### Page Table Structure

A page table maps virtual pages to physical frames.  
Each page table entry stores:

- Frame number
- Valid bit indicating presence in memory

### Virtual Memory Diagram

Virtual Address
|
v
+-------------+
| Page Number |
+-------------+
| Offset |
+-------------+

Page Table
+-----------+-----------+
| Page No. | Frame No. |
+-----------+-----------+

Physical Memory (Frames)
+-----+-----+-----+-----+
| F0 | F1 | F2 | F3 |
+-----+-----+-----+-----+

css
Copy code

### Page Replacement Policy

The simulator uses a First-In, First-Out (FIFO) replacement policy.  
Pages are evicted in the order they were loaded.

---

## 7. Address Translation and Integration Flow

The final integration demonstrates the correct operating system memory access path.

### Address Translation Flow

- A virtual address is provided as input.
- The virtual memory module translates it to a physical address.
- On a page fault, FIFO replacement is applied.
- The physical address is passed to the cache hierarchy.

### Integration Diagram

Virtual Address
|
v
+------------------+
| Page Table |
+------------------+
|
v
Physical Address
|
v
+---------+ Miss +---------+
| L1 | ------> | L2 |
+---------+ +---------+
| |
| Hit | Hit
v v
Data Returned Data Returned
|
L2 Miss
|
v
Main Memory

yaml
Copy code

Cache accesses always occur after address translation, matching real OS behavior.

---

## 8. Limitations and Simplifications

This simulator is an educational model with the following limitations:

### Simplifications

- Single-process environment only
- No timing or latency modeling
- Direct-mapped caches only
- Single-level page table
- No disk or TLB simulation

### Limitations

- Buddy allocator is not integrated into virtual memory
- Cache coherence is not simulated
- Memory protection is not modeled

These design choices prioritize clarity and modularity over hardware realism.