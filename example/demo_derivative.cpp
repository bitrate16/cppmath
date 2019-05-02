#include <iostream>

#include "math_func.h"
#include "math_func_util.h"

// clear && g++ -Iinclude -std=c++17 -w -g example/demo_derivative.cpp -o example/demo_derivative && ./example/demo_derivative

int main() {
	std::string str = "cos(ln(x))";
	
	math_func::func* funct = math_func::parse(str);
	if (!funct) {
		std::cerr << "Error parsing" << std::endl;
		return 1;
	}
	
	// Print parsed
	std::cout << "Parsed function:" << std::endl;
	std::cout << *funct << std::endl;
	
	// Calculate derivative
	math_func::func* der = math_func::derivate(funct, "x");
	
	// Print result
	std::cout << "Derivative:" << std::endl;
	std::cout << *der << std::endl;
	
	// Optimize derivative
	math_func::func* opt = math_func::optimize(der);
	
	// Print result
	std::cout << "Optimized:" << std::endl;
	std::cout << *opt << std::endl;
	
	delete opt;
	delete der;
	delete funct;
	
	return 0;
};