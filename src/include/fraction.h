#pragma once

#include <exception>
#include <cmath>

#include "bigint"

class fraction {
	// Normal fraction:
	//    p
	// w ---
	//    q
	bigint p = 0, q = 1;
	
public:
	
	fraction() {};
	
	fraction(double d) {
		double t = d;
		long exp = 0;
		while (fract(t)) {
			++exp;
			t *= 10;
		}
		
		for (int i = 0; i < exp; ++i)
			q *= 10;
		
		p = (long long) t;
	};
	
	fraction(const fraction& f) {
		p = f.p;
		q = f.q;
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
			p = 1;
			q = 1;
			return;
		}
		
		if (p == 0) {
			q = 1;
			return;
		}
		
		if (q.get_sign()) {
			p.set_sign(p.get_sign() ^ q.get_sign());
			q.set_sign(0);
		}
		
		bigint gcd = bigint::gcd(p, q);
		
		if (gcd > 1) {
			p /= gcd;
			q /= gcd;
		}
	};
	
	fraction& operator*=(const fraction& f) {
		p *= f.p;
		q *= f.q;
		
		normalize();
		return *this;
	};
	
	fraction& operator+=(const fraction& f) {		
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
		p *= f.q;
		if (f.p.is_zero())
			throw std::runtime_error("divide by zero");
		q *= f.p;
		
		normalize();
		return *this;
	};
	
	friend fraction operator*(const fraction& a, const fraction& b) {
		fraction f = a;
		return f *= b;
	};
	
	friend fraction operator/(const fraction& a, const fraction& b) {
		fraction f = a;
		return f /= b;
	};
	
	friend fraction operator+(const fraction& a, const fraction& b) {
		fraction f = a;
		return f += b;
	};
	
	friend fraction operator-(const fraction& a, const fraction& b) {
		fraction f = a;
		return f -= b;
	};
	
	bool operator==(const fraction& f) {
		return q == f.q && p == f.p;
	};
	
	bool operator!=(const fraction& f) {
		return q != f.q && p != f.p;
	};

	bool operator>(const fraction& f) {
		if (q == f.q)
			return p > f.p;
		
		fraction t = *this;
		t -= f;
		return t.p.is_positive();
	};

	bool operator>=(const fraction& f) {
		if (q == f.q)
			return p >= f.p;
		
		fraction t = *this;
		t -= f;
		return t.p.is_positive();
	};

	bool operator<(const fraction& f) {
		if (q == f.q)
			return p < f.p;
		
		fraction t = *this;
		t -= f;
		return t.p.is_negative();
	};

	bool operator<=(const fraction& f) {
		if (q == f.q)
			return p <= f.p;
		
		fraction t = *this;
		t -= f;
		return t.p.is_negative() || t.p.is_zero();
	};
	
	fraction operator-() {
		fraction f = *this;
		f.p.set_sign(f.p.is_positive());
		return f;
	};

	double to_double() {
		long long a = p.int_value();
		long long b = q.int_value();
		
		return (double) a / (double) b;
	};
	
	bigint int_value() {
		bigint res = p, rest;
		bigint::div(res, rest, q);
		
		return res;
	};
};