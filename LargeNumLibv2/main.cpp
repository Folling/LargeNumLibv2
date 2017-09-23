#include <iostream>
#include <chrono>
#include <fstream>
#include <typeinfo>
#include <random>

#include "LargeInt.h"

std::random_device rd;
using namespace std::chrono;

bool debug = true;

int main() {		
	LargeInt a(500000);
	LargeInt b;
	if (debug) {
		int q = 500000;
		for (int i = 0; i < 40000; i++) {
			std::cout << a << ' ' << q << '\n';
			if (--a != --q) {
				std::cout << "Something went wrong\n";
				std::cout << a;
				std::cout << q;
				getchar();
			}
		} 
		std::cout << "Everything worked!\n";
	} else {
		while(true) {
			std::cin >> a;
			//std::cin >> b;
			high_resolution_clock clock;
			high_resolution_clock::time_point start = clock.now();
			std::cout << factorial(a) << '\n';
			auto end = clock.now();
			std::cout << "Process took " << duration_cast<milliseconds>(end - start).count() << " ms.\n";
			getchar();
			system("CLS");
		}
	}	
	getchar();
}