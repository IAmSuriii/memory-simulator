#include <iostream>
#include "virtual_memory/vm.h"
using namespace std;

int main() {
    VirtualMemory vm(16, 256, 64);

    int addresses[] = {0, 1, 2, 16, 17, 32, 0, 48, 64, 0, 16, 32};

    for (int addr : addresses) {
        int phys = vm.translate(addr);
        cout << "VA " << addr << " -> PA " << phys << endl;
    }

    vm.stats();
    return 0;
}
