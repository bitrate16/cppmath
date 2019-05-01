#include <iostream>

#include "math_func.h"

// clear && g++ -Iinclude -std=c++17 -w -g example/demo_expression.cpp -o example/demo_expression && ./example/demo_expression

int main() {
	std::string str = "-(x + y) + y";
	
	math_func::func* funct = math_func::parse(str);
	if (!funct) {
		std::cerr << "Error parsing" << std::endl;
		return 1;
	}
	
	// Print parsed
	std::cout << "Parsed function:" << std::endl;
	std::cout << *funct << std::endl;
	
	// Print substitution of x and y
	std::map<std::string, double> values;
	std::map<std::string, std::function<double(const std::vector<double>&)>> functions;
	
	for (double x = 1.0; x <= 2.0; x += 0.1)
		for (double y = 3.0; y <= 4.0; y += 0.2) {
			values["x"] = x;
			values["y"] = y;
			
			std::cout << "f(" << x << ", " << y << ") = " << funct->evaluate(values, functions) << std::endl;
		}
	
	return 0;
};