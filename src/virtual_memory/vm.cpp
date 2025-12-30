#include "vm.h"
#include <iostream>
using namespace std;

VirtualMemory::VirtualMemory(int pageSize, int virtualSize, int physicalSize)
    : page_size(pageSize),
      num_pages(virtualSize / pageSize),
      num_frames(physicalSize / pageSize),
      page_faults(0),
      page_hits(0) {

    page_table.resize(num_pages);
    for (auto &p : page_table)
        p.valid = false;
}

int VirtualMemory::translate(int virtual_address) {
    int page = virtual_address / page_size;
    int offset = virtual_address % page_size;

    if (page_table[page].valid) {
        page_hits++;
        return page_table[page].frame * page_size + offset;
    }

    page_faults++;

    int frame;
    if ((int)fifo_queue.size() < num_frames) {
        frame = fifo_queue.size();
    } else {
        int victim = fifo_queue.front();
        fifo_queue.pop();
        frame = page_table[victim].frame;
        page_table[victim].valid = false;
    }

    page_table[page].valid = true;
    page_table[page].frame = frame;
    fifo_queue.push(page);

    return frame * page_size + offset;
}

void VirtualMemory::stats() const {
    cout << "Page hits: " << page_hits << endl;
    cout << "Page faults: " << page_faults << endl;
}
