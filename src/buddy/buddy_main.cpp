#include <iostream>
#include "buddy/buddy.h"
using namespace std;

int main() {
    BuddyAllocator buddy(1024, 16);

    int a = buddy.allocate(100);
    int b = buddy.allocate(200);
    int c = buddy.allocate(60);

    cout << "Allocated at: " << a << ", " << b << ", " << c << endl;

    buddy.dump();

    buddy.deallocate(a, 100);
    buddy.deallocate(b, 200);
    buddy.deallocate(c, 60);

    buddy.dump();

    return 0;
}
