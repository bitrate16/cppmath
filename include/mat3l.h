#include <vector>
#include <stdexcept>

// https://www.codeproject.com/articles/560163/csharp-cubic-spline-interpolation

namespace cppmath {
	class mat3l {
		// Example for 6x6
		// 
		//  A[0]|B[0]C[0]                |        
		//      |A[1]B[1]C[1]            |                
		//      |    A[2]B[2]C[2]        |                
		//      |        A[3]B[3]C[3]    |                    
		//      |            A[4]B[4]C[4]|                        
		//      |                A[5]B[5]|C[5]
		
	public:
		
		// A is being indexed [1, n-1]
		// B is being indexed [0, n-1]
		// C is being indexed [0, n-2]
		
		std::vector<double> A;
		std::vector<double> B;
		std::vector<double> C;
	
		// Initialize empty matrix
		mat3l() {};
		
		// Initialize matrix of given size filled with given value
		mat3l(int size, double value = 0.0) { 
			if (size == 0)
				return;
			
			A.resize(size, value);
			B.resize(size, value);
			C.resize(size, value);
		};
		
		// Copy metrix
		mat3l(const mat3l& m) : A(m.A), B(m.B), C(m.C) {};
		
		// Initialize from lines
		mat3l(std::vector<double>& a, std::vector<double>& b, std::vector<double>& c) : A(a), B(b), C(C) {};
		
		std::vector<double>& a() { return A; }
		std::vector<double>& b() { return B; }
		std::vector<double>& c() { return C; }
		
		// Checks if this is valid 3-line-matrix
		inline bool valid() {
			return A.size() == C.size() && A.size() == B.size();
		};
		
		// Length of B line (number of lines in the matrix)
		inline int length() {
			return valid() ? B.size() : -1;
		};
		
		// Solve system of linear equations [this] * X = d for X.
		std::vector<double> solve(std::vector<double>& d) {
			int size = length();
			
			if (size != d.size())
				throw std::invalid_argument("the input d is not the same size as matrix");
			
			std::vector<double> cPrime(size);
			cPrime[0] = C[0] / B[0];
			
			for (int i = 1; i < size; ++i)
				cPrime[i] = C[i] / (B[i] - cPrime[i - 1] * A[i]);
			
			std::vector<double> dPrime(size);
			dPrime[0] = d[0] / B[0];
			
			for (int i = 1; i < size; ++i)
				dPrime[i] = (d[i] - dPrime[i - 1] * A[i]) / (B[i] - cPrime[i - 1] * A[i]);
			
			std::vector<double> X(size);
			X[size - 1] = dPrime[size - 1];
			
			for (int i = size - 2; i >= 0; --i)
				X[i] = dPrime[i] - cPrime[i] * X[i + 1];
			
			return X;
		};
	};
};