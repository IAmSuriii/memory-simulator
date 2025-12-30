#ifndef CACHE_H
#define CACHE_H

#include <vector>

class Cache {
    int cache_size;
    int block_size;
    int num_lines;

    std::vector<int> tag;
    std::vector<bool> valid;

    int hits;
    int misses;

public:
    Cache(int cacheSize, int blockSize);
    bool access(int address);
    void stats() const;
};

#endif
