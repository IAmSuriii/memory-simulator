#include "cache.h"
#include <iostream>
using namespace std;

Cache::Cache(int cacheSize, int blockSize)
    : cache_size(cacheSize), block_size(blockSize),
      hits(0), misses(0) {
    num_lines = cache_size / block_size;
    tag.resize(num_lines, -1);
    valid.resize(num_lines, false);
}

bool Cache::access(int address) {
    int block_number = address / block_size;
    int index = block_number % num_lines;
    int tag_value = block_number / num_lines;

    if (valid[index] && tag[index] == tag_value) {
        hits++;
        return true;
    } else {
        misses++;
        valid[index] = true;
        tag[index] = tag_value;
        return false;
    }
}

void Cache::stats() const {
    cout << "Cache Hits: " << hits << endl;
    cout << "Cache Misses: " << misses << endl;
}
