/*
	cpp math utilities
    Copyright (C) 2019-3041  bitrate16

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
	
	see: https://en.wikipedia.org/wiki/Interval_arithmetic
*/

#pragma once

#include <cmath>
#include <iostream>
#inclide <exception>

#ifdef _WIN32
	#include <float.h>
#else
	#include <fenv.h>
#endif

namespace cppmath {
#ifdef _WIN32
	inline void __set_round_down()    { _control87(_RC_DOWN,    _MCW_RC); }
	inline void __set_round_up()      { _control87(_RC_UP,      _MCW_RC); }
	inline void __set_round_default() { _control87(_CW_DEFAULT, _MCW_RC); }
#else
	inline void __set_round_down()    { fesetround(FE_DOWNWARD ); }
	inline void __set_round_up()      { fesetround(FE_UPWARD   ); }
	inline void __set_round_default() { fesetround(FE_TONEAREST); }
#endif

	class interval {

	public:
		
		double a, b;
		
		interval() : a(0), b(0) {}
		
		interval(double x) : a(x), b(x) {};
		
		interval(double a_, double b_) : a(a_), b(b_) {};
		
		interval(const interval &i) : a(i.a), b(i.b) {};
		
		interval(long numer, long denom) {
			__set_round_down();
			a = double(numer) / double(denom);
			__set_round_up();
			b = double(numer) / double(denom);
			__set_round_default();
		};
		
		interval(long numer_a, long denom_a, long numer_b, long denom_b) {
			__set_round_down();
			a = double(numer_a) / double(denom_a);
			__set_round_up();
			b = double(numer_b) / double(denom_b);
			__set_round_default();
		};
		
		
		interval& operator=(const interval& i) {
			a = i.a;
			b = i.b;
			return *this;
		}
		
		interval& operator+=(const interval& i) {
			__set_round_down();
			a += i.a;
			__set_round_up();
			b += i.b;
			__set_round_default();
			
			return *this;
		}
		
		interval& operator-=(const interval& i) {
			if (this == &i) 
				a = b = 0.0;
			else {
				__set_round_down();
				long double _a = a - i.b;
				__set_round_up();
				long double _b = b - i.a;
				__set_round_default();
				
				a = _a;
				b = _b;
			}
			
			return *this;
		}
		
		interval& operator*=(const interval& i) {
			if (this == &i) 
				if (a > 0.0 || b < 0.0) {
					__set_round_down();
					double _a = std::min(a * a, b * b);
					__set_round_up();
					double _b = std::max(a * a, b * b);
					__set_round_default();
					
					a = _a;
					b = _b;
				} else {
					a = 0.0;
					__set_round_up();
					b = std::max(a * a, b * b);
					__set_round_default();
				}
			else {
				__set_round_down();
				double _a = std::min(std::min(a * i.a, a * i.b), std::min(b * i.a, b * i.b));
				__set_round_up();
				double _b = std::max(std::min(a * i.a, a * i.b), std::min(b * i.a, b * i.b));
				__set_round_default();
				
				a = _a;
				b = _b;
			}
			
			return *this;
		}
		
		interval& operator/=(const interval& i) {
			if (i.a <= 0.0 && 0.0 <= i.b)
				throw std::runtime_error("interval division by zero");
			
			if (this == &i) 
				a = b = 1.0;
			else {
				interval i_inv;
				__set_round_down();
				i_inv.a = double(1) / i.b;
				__set_round_up();
				i_inv.b = double(1) / i.a;
				__set_round_default();
				
				return (*this) *= i_inv;
			}
			
			return *this;
		}
	
		bool has(double d) {
			return a <= d && b <= b;
		};
	
		/* Check if containing i */
		bool contains(const interval& i) {
			return a >= i.a && i.b <= b;
		}
		
		/* Check if contained in i */
		bool subsetof(const interval& i) {
			return i.a >= a && b <= i.b;
		}
		
		/* Check if intersects i */
		bool intersects(const interval& i) {
			return a <= i.b && b <= i.a;
		}
		
		/* Set to 1 is not containing 0 */
		bool iszero() {
			return b <= 0.0 || a >= 0.0;
		}
	};
	
	
	interval operator+(const interval& a, const interval& b) {
		return interval(a) += b;
	};
	
	interval operator-(const interval& a, const interval& b) {
		return interval(a) -= b;
	};

	interval operator*(const interval& a, const interval& b) {
		return interval(a) *= b;
	};
	
	interval operator/(const interval& a, const interval& b) {
		return interval(a) /= b;
	};
	
	
	bool operator>(const interval& a, const interval& b) {
		return a.a > b.b;
	};
	
	bool operator>=(const interval& a, const interval& b) {
		return a.a >= b.b;
	};
	
	bool operator<(const interval& a, const interval& b) {
		return a.b < b.a;
	};
	
	bool operator<=(const interval& a, const interval& b) {
		return a.b <= b.a;
	};
	
	/* b contains a */
	/*bool operator<(const interval& a, const interval& b) {
		return b.a > a.a && a.b < b.b;
	};
	
	bool operator<=(const interval& a, const interval& b) {
		return b.a >= a.a && a.b <= b.b;
	};
	
	bool operator>(const interval& a, const interval& b) {
		return a.a > b.a && b.b < a.b;
	};
	
	bool operator>=(const interval& a, const interval& b) {
		return a.a >= b.a && b.b <= a.b;
	};*/
	
	
	/* b equals a */
	bool operator==(const interval& a, const interval& b) {
		return a.a == b.a && a.b == b.b;
	};
	
	bool operator!=(const interval& a, const interval& b) {
		return a.a != b.a && a.b != b.b;
	};
	
	
	/* intersect a and b */
	interval operator&(const interval& a, const interval& b) {
		if (a.b < b.a)
			return interval(0);
		return interval(std::max(a.a, b.a), std::min(a.b, b.a));
	};
	
	/* combine a and b */
	interval operator|(const interval& a, const interval& b) {
		if (a.b < b.a)
			return 0;
		return interval(std::min(a.a, b.a), std::max(a.b, b.a));
	};
	
	
	interval operator-(const interval& i) {
		return interval(-i.b, -i.a);
	};
	
	interval operator+(const interval& i) {
		return interval(i);
	};


	std::ostream& operator<<(std::ostream& os, const interval& i) {
		os << '[' << i.a << "; " << i.b << ']';
	};
};
