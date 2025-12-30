#include "buddy.h"
#include <iostream>
using namespace std;

static int nextPowerOfTwo(int x) {
    int p = 1;
    while (p < x) p <<= 1;
    return p;
}

BuddyAllocator::BuddyAllocator(int totalSize, int minBlockSize)
    : total_size(totalSize), min_block_size(minBlockSize) {
    free_list[total_size].push_back(0);
}

int BuddyAllocator::allocate(int size) {
    int req = nextPowerOfTwo(size);
    if (req < min_block_size)
        req = min_block_size;

    int curr = req;
    while (curr <= total_size && free_list[curr].empty())
        curr <<= 1;

    if (curr > total_size)
        return -1;

    int addr = free_list[curr].front();
    free_list[curr].pop_front();

    while (curr > req) {
        curr >>= 1;
        free_list[curr].push_back(addr + curr);
    }

    return addr;
}

void BuddyAllocator::deallocate(int address, int size) {
    int block = nextPowerOfTwo(size);
    if (block < min_block_size)
        block = min_block_size;

    while (block < total_size) {
        int buddy = address ^ block;
        auto &list = free_list[block];

        auto it = list.begin();
        bool merged = false;

        for (; it != list.end(); ++it) {
            if (*it == buddy) {
                list.erase(it);
                address = min(address, buddy);
                block <<= 1;
                merged = true;
                break;
            }
        }

        if (!merged)
            break;
    }

    free_list[block].push_back(address);
}

void BuddyAllocator::dump() const {
    cout << "---- BUDDY FREE LISTS ----" << endl;
    for (auto &p : free_list) {
        cout << "Size " << p.first << ": ";
        for (int addr : p.second)
            cout << addr << " ";
        cout << endl;
    }
}
