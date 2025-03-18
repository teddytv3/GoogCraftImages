#include <iostream>
#include <string>
#include "test.h"

int main(int argc, char* argv[]) {
	int a = 5;
	int b = 8;

	std::cout << a << " + " << b << " = " << testFunction(a, b) << std::endl;
		
	return 0;
}