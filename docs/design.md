1. Memory Layout and Assumptions

The simulator models physical memory as a contiguous block of addressable memory. Memory is represented using software data structures rather than actual hardware memory. Each memory block has a starting address, size, and allocation status (free or allocated).

The total memory size is fixed at initialization time (for example, 1024 bytes). Memory allocation and deallocation operations update this simulated layout by splitting and merging blocks as required.

The following assumptions are made in the design:

Memory addresses are byte-addressable.

All allocations are aligned to a fixed word size to avoid misaligned accesses.

The simulator does not model hardware-level details such as DRAM banks or memory timing.

The system assumes a single contiguous physical memory region.

No concurrent processes are simulated; memory requests are handled sequentially.

Memory Representation Diagram
+--------------------------------------------------+
|                  Physical Memory                 |
+--------------------------------------------------+
| Block 1 | Block 2 |   Free Block   |  Block 3    |
+--------------------------------------------------+
|  Used   |  Used   |     Free       |   Used      |
+--------------------------------------------------+


Each block stores metadata such as:

Start address

Allocated size

Requested size (for fragmentation analysis)

Free/allocated status

This abstraction allows the simulator to track memory usage, fragmentation, and allocation behavior in a controlled environment.

2. Allocation Strategy Implementations

The simulator implements dynamic memory allocation as a standalone module. This module is responsible for managing physical memory using variable-sized blocks and supports multiple allocation strategies. The allocator operates on a contiguous region of simulated physical memory and maintains a list of free and allocated blocks.

Each allocation request searches the free memory blocks according to the selected strategy and allocates memory by splitting blocks when necessary.

Implemented Allocation Strategies
First Fit

In the First Fit strategy, the allocator selects the first free block that is large enough to satisfy the requested memory size. This approach is simple and fast, as it stops searching once a suitable block is found. However, it can lead to fragmentation near the beginning of memory.

Best Fit

In the Best Fit strategy, the allocator searches all free blocks and selects the smallest block that can accommodate the requested size. This strategy attempts to minimize wasted space within allocated blocks but may increase search time and external fragmentation.

Worst Fit

In the Worst Fit strategy, the allocator selects the largest available free block for allocation. By allocating from the largest hole, it aims to reduce the creation of very small unusable fragments.

Block Management

When a block larger than the requested size is selected, it is split into:

an allocated block of the requested size

a remaining free block

During deallocation, adjacent free blocks are merged (coalesced) to reduce fragmentation and maintain larger contiguous free regions.

Allocation Interface

The allocator is accessed through a command-line interface supporting operations such as:

memory allocation

memory deallocation

memory state dumping

statistics reporting

This design allows each allocation strategy to be tested independently and compared under different memory usage patterns.

Allocation Strategy Diagram
Free Memory Blocks
+--------+------------+----------+
|  Free  |    Free    |   Free   |
+--------+------------+----------+

Request Size = X

First Fit  -> selects first suitable block
Best Fit   -> selects smallest suitable block
Worst Fit  -> selects largest suitable block

3. Buddy System Design

The simulator includes an implementation of the Buddy Memory Allocation system as a standalone module. The buddy system is designed as an alternative physical memory management strategy and is implemented independently from the variable-sized block allocator.

Buddy System Overview

The buddy allocator manages memory in blocks whose sizes are powers of two. The total memory size is assumed to be a power of two at initialization. Allocation requests are rounded up to the nearest power of two before allocation.

Memory is conceptually represented as a binary tree, where each node corresponds to a memory block. Two blocks of the same size that originate from the same parent block are called buddies.

Allocation Process

When a request is made:

The requested size is rounded up to the nearest power of two.

The allocator searches for a free block of that size.

If no block of the required size is available, a larger block is recursively split into two buddy blocks.

One buddy is allocated, and the other is added back to the appropriate free list.

Deallocation and Coalescing

When a block is freed:

The allocator checks whether its buddy is also free.

If both buddies are free, they are merged (coalesced) into a larger block.

This merging continues recursively until no further coalescing is possible.

The buddy of a block is identified using the XOR operation on the block’s starting address and its size.

Free List Management

Separate free lists are maintained for each block size (power of two). This allows efficient lookup, allocation, and merging operations.

Design Choice

The buddy allocator is implemented as a separate module and is not integrated into the virtual memory pipeline. The virtual memory simulation operates on fixed-size frames and does not require direct interaction with the buddy allocator. This separation reflects real operating system design, where physical memory allocation policies and virtual memory management are distinct components.

Buddy System Diagram
Initial Block (1024)
        |
        v
     Split
    /     \
 512       512
           |
         Split
        /     \
     256       256

4. Cache Hierarchy and Replacement Policy

The simulator implements a multilevel CPU cache hierarchy as a standalone module. The cache system models two levels of cache—L1 and L2—to study the impact of caching on memory access performance.

Cache Organization

Both L1 and L2 caches are implemented as direct-mapped caches. Each cache consists of a fixed number of cache lines, where each line stores:

a valid bit

a tag identifying the memory block

The cache index and tag are derived from the physical memory address using the block size and number of cache lines.

Cache Access Procedure

For every memory access:

The physical address is checked in the L1 cache.

If an L1 hit occurs, the access completes immediately.

On an L1 miss, the L2 cache is checked.

If an L2 hit occurs, the block is promoted to L1.

If both caches miss, the block is fetched from main memory and inserted into both L2 and L1.

This hierarchy models the behavior of real processors, where lower-level cache misses propagate to higher-latency memory levels.

Replacement Policy

Since the cache is direct-mapped, replacement is implicit. Each memory block maps to exactly one cache line. When a new block is inserted into a cache line, it automatically replaces the existing entry in that line.

No explicit replacement algorithm (such as LRU or FIFO) is required in this design.

Statistics Collection

Each cache level independently tracks:

number of cache hits

number of cache misses

hit ratio

These statistics are printed at the end of execution to evaluate cache performance.

Cache Hierarchy Diagram
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

5. Virtual Memory Model

The simulator implements a paging-based virtual memory system as a standalone module. Virtual memory provides an abstraction that allows programs to use virtual addresses, which are translated into physical addresses using a page table.

Virtual Address Space

The virtual address space is divided into fixed-size pages. Each virtual address consists of:

a page number

an offset within the page

The page size is fixed at initialization time and is used consistently for address translation.

Page Table Structure

A page table is maintained to map virtual pages to physical frames. Each page table entry stores:

frame number

valid bit indicating whether the page is currently loaded in memory

If a page is not present in physical memory, a page fault occurs.

Page Fault Handling

On a page fault:

A physical frame is selected for loading the required page.

If all frames are occupied, a page replacement policy is applied.

The page table is updated with the new mapping.

Page Replacement Policy

The simulator uses a First-In, First-Out (FIFO) replacement policy. Pages are evicted in the order in which they were loaded into memory. FIFO is simple to implement and clearly demonstrates page replacement behavior.

Virtual Memory Design Choice

The virtual memory module operates on fixed-size frames and does not depend on the underlying physical memory allocation strategy. As a result, it is implemented independently of both the list-based allocator and the buddy allocator.

Virtual Memory Diagram
Virtual Address
     |
     v
+-------------+
| Page Number |
+-------------+
|   Offset    |
+-------------+

Page Table
+-----------+-----------+
| Page No.  | Frame No. |
+-----------+-----------+

Physical Memory (Frames)
+-----+-----+-----+-----+
| F0  | F1  | F2  | F3  |
+-----+-----+-----+-----+

6. Address Translation and Integration Flow

The final integration of the simulator demonstrates the correct operating system memory access path. Memory accesses originate as virtual addresses and are translated before interacting with the cache hierarchy. This integration is implemented in the top-level controller program (main.cpp).

Address Translation Flow

For each memory access, the following steps are performed:

A virtual address is provided as input.

The virtual memory module translates the virtual address into a physical address using the page table.

If the required page is not present in memory, a page fault occurs and the page is loaded using the FIFO replacement policy.

The resulting physical address is then used for cache access.

Cache Access After Translation

Once a physical address is obtained:

The address is checked in the L1 cache.

On an L1 miss, the L2 cache is checked.

If the block is found in L2, it is promoted to L1.

If both caches miss, the block is fetched from main memory and inserted into both cache levels.

Cache accesses always occur after address translation, ensuring that the cache operates exclusively on physical addresses. This ordering matches real operating system behavior.

Integration Design Choice

The integration intentionally excludes direct interaction with the allocator and buddy system. Virtual memory operates on fixed-size frames, and cache simulation assumes the existence of physical memory. Allocation policies are demonstrated independently to maintain modularity and clarity.

Address Translation and Cache Flow Diagram
Virtual Address
      |
      v
+------------------+
|  Page Table      |
+------------------+
      |
      v
Physical Address
      |
      v
+---------+   Miss   +---------+
|   L1    | ------> |   L2    |
+---------+          +---------+
      |                 |
      | Hit             | Hit
      v                 v
   Data Returned     Data Returned
          |
       L2 Miss
          |
          v
     Main Memory

7. Limitations and Simplifications

This simulator is designed as an educational model to demonstrate core operating system memory management concepts. To keep the design simple and understandable, several limitations and simplifications are intentionally made.

Simplifications

The simulator assumes a single-process environment; no concurrency or multitasking is modeled.

Memory access timing, latency, and hardware-specific delays are not simulated.

The cache is implemented as a direct-mapped cache without advanced replacement policies such as LRU or LFU.

The virtual memory system uses a single-level page table and does not simulate multi-level page tables.

Disk I/O and backing store behavior are not modeled during page faults.

Limitations

The buddy allocator and list-based allocator are implemented as independent modules and are not integrated into the virtual memory pipeline.

Cache coherence between multiple processors is not simulated.

Memory protection mechanisms such as access permissions are not implemented.

The simulator does not model real hardware constraints such as TLBs or address translation caches.

Design Rationale

These design choices were made to ensure clarity, modularity, and ease of understanding. By separating allocation strategies, virtual memory, and cache simulation, the project focuses on illustrating fundamental operating system principles rather than low-level hardware details.