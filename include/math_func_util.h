#pragma once

#include "math_func.h"

namespace math_func {
	
	func* mul(func* a, func* b) { 
		return new operator_func(operator_func::MUL, a, b);
	};
	
	func* div(func* a, func* b) { 
		return new operator_func(operator_func::DIV, a, b);
	};
	
	func* sum(func* a, func* b) { 
		return new operator_func(operator_func::ADD, a, b);
	};
	
	func* sub(func* a, func* b) { 
		return new operator_func(operator_func::SUB, a, b);
	};
	
	func* neg(func* a) { 
		return new operator_func(operator_func::NEG, a);
	};
	
	func* pow(func* a, func* b) { 
		return new operator_func(operator_func::POW, a, b);
	};
	
	func* pow(func* a, double value) { 
		return new operator_func(operator_func::POW, a, new const_func(value));
	};
	
	func* derivate(func* f, const std::string& varname) {
		if (f == nullptr)
			return nullptr;
		
		if (dynamic_cast<const_func*>(f)) // const' = 0
			return new const_func(0);
			
		if (dynamic_cast<name_func*>(f)) { // x' = 1
			name_func* t = dynamic_cast<name_func*>(f);
			if (t->name == varname)
				return new const_func(1);
			else
				return new const_func(0);
		}
		
		if (dynamic_cast<call_func*>(f)) {
			call_func* t = dynamic_cast<call_func*>(f);
			
			// d f(x, y, z, ...) / dt = f'[x] (x, y, z, ...) * x'[t] + f'[y] (x, y, z, ...) * y'[t] + ...
			
			if (t->name == "sin") {
				call_func* fun = new call_func("cos", t->copy_args());
				
				if (fun->args.size() == 0)
					return nullptr;
				
				func* der = mul(fun, derivate(t->args[0], varname));
				
				for (int i = 1; i < fun->args.size(); ++i)
					der = sum(der, mul(fun, derivate(t->args[i], varname)));
				
				return der;
			}
			
			if (t->name == "cos") {
				func* fun = neg(new call_func("sin", t->copy_args()));
				
				if (t->args.size() == 0)
					return nullptr;
				
				func* der = mul(fun, derivate(t->args[0], varname));
				
				for (int i = 1; i < t->args.size(); ++i)
					der = sum(der, mul(fun, derivate(t->args[i], varname)));
				
				return der;
			}
			
			throw std::runtime_error("derivate function failed: function not defined");
		}
		
		if (dynamic_cast<operator_func*>(f)) {
			operator_func* t = dynamic_cast<operator_func*>(f);
			
			// a + b -> a' + b'
			// a - b -> a' - b'
			// a * b -> a' * b + a * b'
			// a / b -> (a' * b - a * b') / b ^ 2
			// a ^ b -> a' * b * a ^ (b - 1)
			// -a -> -a'
			// +a -> +a'
			
			switch(t->opcode) {
				case operator_func::ADD:
					return sum(derivate(t->left, varname), derivate(t->right, varname));
				case operator_func::SUB: 
					return sum(derivate(t->left, varname), derivate(t->right, varname));
				case operator_func::MUL: 
					return  sum(mul(derivate(t->left, varname), t->right->copy()), 
								mul(t->left->copy(), derivate(t->right, varname)));
				case operator_func::DIV: 
					return  div(sub(mul(derivate(t->left, varname), t->right->copy()), 
									mul(t->left->copy(), derivate(t->right, varname))),
								pow(t->right->copy(), 2.0));
				case operator_func::POW: 
					return  mul(derivate(t->left, varname),
								mul(t->right->copy(),
									pow(t->left->copy(),
										sub(t->right->copy(),
											new const_func(1)))));
				case operator_func::POS:
					return derivate(t->left, varname);
				case operator_func::NEG: 
					return neg(derivate(t->right, varname));
			}
		}
			
		throw std::runtime_error("derivate failed: undefined");
	};
};