#include "include/math.h"
#include "include/vec2.h"
#include "include/vec3.h"

#include <iostream>
#include <cstring>

using namespace std;
using namespace cppmath;

// clear && gcc -std=c++11 -w -g vec2.cpp vec3.cpp arrangement_test.cpp -lstdc++ -lm -o arrangement_test && cat test_arrangement.txt | ./arrangement_test

int main(int argc, char **argv) {
	while (cin) {		
		vec2 A, B, C, D;
		cin >> A.x >> A.y;
		cin >> B.x >> B.y;
		cin >> C.x >> C.y;
		cin >> D.x >> D.y;
		
		int test;
		cin >> test;
		
		int result = math::arrangement(A, B, C, D);
		if (result != test)
			cout << "Missed for " << A << "; " << B << "; " << C << "; " << D << "; result = " << result << ", test = " << test << " MISS" << endl; 
		else
			cout << "Passed for " << A << "; " << B << "; " << C << "; " << D << "; result = " << result << ", test = " << test << endl; 
	}
	
	return 0;
};
