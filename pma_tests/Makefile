all: pma_random_ip impl1 impl2

pma_random_ip: pma_random_ip.cpp
	g++ -Wall pma_random_ip.cpp -o pma_random_ip

impl1: impl1.cpp
	g++ -O2 -Wall impl1.cpp -o impl1

impl2: impl2.cpp
	g++ -O2 impl2.cpp -o impl2

clean:
	rm pma_random_ip impl1 impl2
