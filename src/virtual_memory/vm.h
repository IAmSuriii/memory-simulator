#ifndef VM_H
#define VM_H

#include <vector>
#include <queue>

struct PageTableEntry {
    bool valid;
    int frame;
};

class VirtualMemory {
    int page_size;
    int num_pages;
    int num_frames;

    std::vector<PageTableEntry> page_table;
    std::queue<int> fifo_queue;

    int page_faults;
    int page_hits;

public:
    VirtualMemory(int pageSize, int virtualSize, int physicalSize);
    int translate(int virtual_address);
    void stats() const;
};

#endif
