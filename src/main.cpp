#include <iostream>
#include "virtual_memory/vm.h"
#include "cache/cache.h"

using namespace std;

int main() {
    VirtualMemory vm(16, 256, 64);

    Cache L1(64, 16);
    Cache L2(256, 16);

    cout << "Integrated Memory Simulator\n";
    cout << "Flow: VA -> Page Table -> PA -> L1 -> L2 -> Memory\n";
    cout << "Enter virtual addresses (-1 to exit)\n\n";

    while (true) {
        int va;
        cin >> va;

        if (va == -1)
            break;

        int pa = vm.translate(va);

        cout << "VA " << va << " -> PA " << pa << " : ";

        if (L1.access(pa)) {
            cout << "L1 HIT\n";
        } else {
            cout << "L1 MISS -> ";

            if (L2.access(pa)) {
                cout << "L2 HIT\n";
                L1.access(pa);
            } else {
                cout << "L2 MISS -> MAIN MEMORY\n";
                L2.access(pa);
                L1.access(pa);
            }
        }
    }

    cout << "\n--- Virtual Memory Stats ---\n";
    vm.stats();

    cout << "\n--- L1 Cache Stats ---\n";
    L1.stats();

    cout << "\n--- L2 Cache Stats ---\n";
    L2.stats();

    return 0;
}
