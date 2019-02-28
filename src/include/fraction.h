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
		
		if (!p.is_positive() || !q.is_positive()) {
			p.set_sign(0);
			q.set_sign(0);
			w.set_sign(w.is_positive());
			
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
		
		if (p == q) {
			w += 1;
			p = 0;
			q = 1;
			return;
		}
		
		if (p == 0) {
			q = 1;
			return;
		}
		
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
	
	fraction& operator*=(const fraction& f) {
		w *= f.w;
		p *= f.p;
		q *= f.q;
		
		normalize();
		return *this;
	};
	
	fraction& operator+=(const fraction& f) {
		w *= f.w;
		
		if (q == f.q) 
			p += f.p;
		else {
			p += f.p * q;
			q *= f.q;
		}
		
		normalize();
		return *this;
	};
	
	fraction& operator-=(const fraction& f) {
		w *= f.w;
		
		if (q == f.q) 
			p -= f.p;
		else {
			p -= f.p * q;
			q *= f.q;
		}
		
		normalize();
		return *this;
	};
	
	fraction& operator-=(const fraction& f) {
		w *= f.w;
		
		if (q == f.q) 
			p -= f.p;
		else {
			p -= f.p * q;
			q *= f.q;
		}
		
		normalize();
		return *this;
	};
	
	fraction& operator/=(const fraction& f) {
		
	};
};