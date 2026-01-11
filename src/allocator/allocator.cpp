#include <iostream>
#include <list>
#include <string>
using namespace std;

struct Block {
    int start;
    int size;
    int requested;
    bool free;
    int id;
};

list<Block> memory;
int next_id = 1;
int TOTAL_MEMORY = 1024;

int alloc_requests = 0;
int alloc_success = 0;
int alloc_failure = 0;

enum AllocatorType {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

AllocatorType currentAllocator = FIRST_FIT;

void dumpMemory() {
    cout << "------ MEMORY DUMP ------" << endl;
    for (auto &b : memory) {
        int end = b.start + b.size - 1;
        cout << "[" << b.start << " - " << end << "] ";
        if (b.free)
            cout << "FREE";
        else
            cout << "USED (id=" << b.id << ")";
        cout << endl;
    }
    cout << "-------------------------" << endl;
}

void printStats() {
    int used = 0;
    int free_mem = 0;
    int largest_free = 0;
    int internal_frag = 0;

    for (auto &b : memory) {
        if (b.free) {
            free_mem += b.size;
            if (b.size > largest_free)
                largest_free = b.size;
        } else {
            used += b.size;
            internal_frag += (b.size - b.requested);
        }
    }

    double external_frag = 0.0;
    if (free_mem > 0)
        external_frag = 1.0 - (double)largest_free / free_mem;

    double utilization = (double)used / TOTAL_MEMORY;

    cout << "------ MEMORY STATS ------" << endl;
    cout << "Total memory: " << TOTAL_MEMORY << endl;
    cout << "Used memory : " << used << endl;
    cout << "Free memory : " << free_mem << endl;
    cout << "Memory utilization: " << utilization * 100 << "%" << endl;
    cout << "Internal fragmentation: " << internal_frag << endl;
    cout << "External fragmentation: " << external_frag * 100 << "%" << endl;
    cout << "Allocation success rate: "
         << (alloc_requests ? (double)alloc_success / alloc_requests * 100 : 0)
         << "%" << endl;
    cout << "Allocation failure rate: "
         << (alloc_requests ? (double)alloc_failure / alloc_requests * 100 : 0)
         << "%" << endl;
    cout << "--------------------------" << endl;
}

void allocateBlock(list<Block>::iterator it, int req_size) {
    int remaining = it->size - req_size;
    int start_addr = it->start;

    it->free = false;
    it->requested = req_size;
    it->size = req_size;
    it->id = next_id++;

    if (remaining > 0) {
        Block newBlock;
        newBlock.start = start_addr + req_size;
        newBlock.size = remaining;
        newBlock.requested = 0;
        newBlock.free = true;
        newBlock.id = -1;
        memory.insert(next(it), newBlock);
    }

    alloc_success++;
    cout << "Allocated block id=" << it->id
         << " at address=" << start_addr << endl;
}

void mallocFirstFit(int req_size) {
    alloc_requests++;
    for (auto it = memory.begin(); it != memory.end(); it++) {
        if (it->free && it->size >= req_size) {
            allocateBlock(it, req_size);
            return;
        }
    }
    alloc_failure++;
    cout << "Allocation failed" << endl;
}

void mallocBestFit(int req_size) {
    alloc_requests++;
    auto best = memory.end();
    for (auto it = memory.begin(); it != memory.end(); it++) {
        if (it->free && it->size >= req_size) {
            if (best == memory.end() || it->size < best->size)
                best = it;
        }
    }
    if (best != memory.end())
        allocateBlock(best, req_size);
    else {
        alloc_failure++;
        cout << "Allocation failed" << endl;
    }
}

void mallocWorstFit(int req_size) {
    alloc_requests++;
    auto worst = memory.end();
    for (auto it = memory.begin(); it != memory.end(); it++) {
        if (it->free && it->size >= req_size) {
            if (worst == memory.end() || it->size > worst->size)
                worst = it;
        }
    }
    if (worst != memory.end())
        allocateBlock(worst, req_size);
    else {
        alloc_failure++;
        cout << "Allocation failed" << endl;
    }
}

void freeBlock(int id) {
    for (auto it = memory.begin(); it != memory.end(); it++) {
        if (!it->free && it->id == id) {
            it->free = true;
            it->requested = 0;
            it->id = -1;

            if (it != memory.begin()) {
                auto prev = it;
                prev--;
                if (prev->free) {
                    prev->size += it->size;
                    it = memory.erase(it);
                    it = prev;
                }
            }

            auto next = it;
            next++;
            if (next != memory.end() && next->free) {
                it->size += next->size;
                memory.erase(next);
            }

            cout << "Block freed and merged" << endl;
            return;
        }
    }
    cout << "Invalid block id" << endl;
}

int main() {
    memory.push_back({0, TOTAL_MEMORY, 0, true, -1});
    cout << "Memory simulator started\n";

    while (true) {
        string cmd;
        cin >> cmd;

        if (cmd == "malloc") {
            int sz;
            cin >> sz;
            if (currentAllocator == FIRST_FIT)
                mallocFirstFit(sz);
            else if (currentAllocator == BEST_FIT)
                mallocBestFit(sz);
            else
                mallocWorstFit(sz);
        }
        else if (cmd == "free") {
            int id;
            cin >> id;
            freeBlock(id);
        }
        else if (cmd == "dump") {
            dumpMemory();
        }
        else if (cmd == "stats") {
            printStats();
        }
        else if (cmd == "set") {
            string type;
            cin >> type;
            if (type == "first") currentAllocator = FIRST_FIT;
            else if (type == "best") currentAllocator = BEST_FIT;
            else if (type == "worst") currentAllocator = WORST_FIT;
            cout << "Allocator set\n";
        }
        else if (cmd == "exit") {
            break;
        }
        else {
            cout << "Unknown command\n";
        }
    }
    return 0;
}
