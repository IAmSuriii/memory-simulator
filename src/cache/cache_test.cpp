#include <iostream>
#include "cache/cache.h"
using namespace std;

int main() {
    Cache L1(64, 16);
    Cache L2(256, 16);

    int addresses[] = {0, 16, 32, 0, 64, 16, 128, 0, 256, 16, 32};

    for (int addr : addresses) {
        cout << "Accessing address " << addr << " : ";

        if (L1.access(addr)) {
            cout << "L1 HIT" << endl;
        } else {
            cout << "L1 MISS -> ";

            if (L2.access(addr)) {
                cout << "L2 HIT" << endl;
                L1.access(addr);
            } else {
                cout << "L2 MISS -> MAIN MEMORY" << endl;
                L2.access(addr);
                L1.access(addr);
            }
        }
    }

    cout << "\n--- L1 Stats ---\n";
    L1.stats();

    cout << "\n--- L2 Stats ---\n";
    L2.stats();

    return 0;
}
