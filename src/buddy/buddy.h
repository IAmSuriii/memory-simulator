#ifndef BUDDY_H
#define BUDDY_H

#include <map>
#include <list>

class BuddyAllocator {
    int total_size;
    int min_block_size;
    std::map<int, std::list<int>> free_list;

public:
    BuddyAllocator(int totalSize, int minBlockSize);
    int allocate(int size);
    void deallocate(int address, int size);
    void dump() const;
};

#endif
