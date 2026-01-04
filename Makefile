CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Default target
all: memsim

# Integrated VM + Cache
memsim:
	$(CXX) $(CXXFLAGS) src/main.cpp src/cache/cache.cpp src/virtual_memory/vm.cpp -o memsim

# Memory allocator (First/Best/Worst Fit)
allocator:
	$(CXX) $(CXXFLAGS) src/allocator/allocator_main.cpp -o allocator

# Buddy allocator
buddy:
	$(CXX) $(CXXFLAGS) src/buddy/buddy_main.cpp src/buddy/buddy.cpp -o buddy

# Clean compiled files
clean:
	rm -f memsim allocator buddy
