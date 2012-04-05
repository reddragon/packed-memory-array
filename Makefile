CXXFLAGS := -Wall -O2

all: impl1 impl2

impl1: impl1.cpp
	$(CXX) impl1.cpp -o impl1 $(CXXFLAGS)

impl2: impl2.cpp
	$(CXX) impl2.cpp -o impl2 $(CXXFLAGS)

clean:
	rm -f impl1 impl2
