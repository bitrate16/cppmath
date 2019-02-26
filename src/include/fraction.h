#pragma once

#include <exception>

#include "bigint"

class fraction {
	// Normal fraction:
	//    p
	// w ---
	//    q
	bigint w, p, q;
	
public:
	
	fraction() {};
	
	fraction(double d) {};
	
	fraction(const fraction& f) {
		w = f.w;
		p = f.p;
		q = f.q;
	};
	
	fraction(bigint w, bigint p, bigint q) {
		if (q.is_zero())
			throw std::runtime_error("divide by zero");
		
		this->w = w;
		this->p = p;
		this->q = q;
	};
	
	fraction(bigint p, bigint q) {
		if (q.is_zero())
			throw std::runtime_error("divide by zero");
		
		this->p = p;
		this->q = q;
		
		normalize();
	};
	
	void normalize() {
		if (q.is_zero())
			throw std::runtime_error("divide by zero");
		
		bigint gcd = bigint::gcd(p, q);
		
		if (gcd > 1) {
			p /= gcd;
			q /= gcd;
		}
		
		bigint res = p, rest;
		bigint::div(res, rest, q);
		
		p = rest;
		w += res;
	};
};