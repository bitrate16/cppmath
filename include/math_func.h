#pragma once

#include <stdexcept>
#include <functional>
#include <map>
#include <string>
#include <vector>

// Represents callable math functions
// a * b - x + sin(cos( e ^ 12))
//
// function ::= <add_exp>
// add_exp  ::= <mul_exp>  | <mul_exp>  + <add_exp> | <mul_exp>  - <add_exp>
// mul_exp  ::= <atomic>   | <atomic> * <mul_exp>   | <atomic> / <mul_exp>  | <atomic> ^ <mul_exp>
// atomic   ::= -<atomic>  | +<atomic> | <name>     | <name>({<add_exp>, }) | <number> | (<add_exp>)

namespace math_func {
	struct func {
		virtual ~func() {};
		
		// Should evaluate the function and retuan result
		virtual double evaluate(const std::map<std::string, double>& values, const std::map<std::string, std::function<double(const std::vector<double>&)>>& functions) = 0;
		
		virtual void print(std::ostream& os) const = 0;
		
		friend std::ostream& operator<<(std::ostream& os, const func& f);
	};
	
	std::ostream& operator<<(std::ostream& os, const func& f) {
		f.print(os);
		
		return os;
	};
	
	struct operator_func : public func {
		static const int ADD = 0;
		static const int SUB = 1;
		static const int MUL = 2;
		static const int DIV = 3;
		static const int POS = 4;
		static const int NEG = 5;
		
		func* left = nullptr;
		func* right = nullptr;
		int opcode;
		
		operator_func(int _opcode, func* _left = nullptr, func* _right = nullptr) : opcode(_opcode), left(_left), right(_right) {};
		
		~operator_func() {
			if (left) delete left;
			if (right) delete right;
		};
		
		double evaluate(const std::map<std::string, double>& values, const std::map<std::string, std::function<double(const std::vector<double>&)>>& functions) {
			switch(opcode) {
				case ADD: return left->evaluate(values, functions) + right->evaluate(values, functions);
				case SUB: return left->evaluate(values, functions) - right->evaluate(values, functions);
				case MUL: return left->evaluate(values, functions) * right->evaluate(values, functions);
				case DIV: return left->evaluate(values, functions) / right->evaluate(values, functions);
				case POS: return + (left->evaluate(values, functions));
				case NEG: return - (left->evaluate(values, functions));
			}
		};
		
		void print(std::ostream& os) const {
			switch(opcode) {
				case ADD: os << *left << " + " << *right; break;
				case SUB: os << *left << " - " << *right; break;
				case MUL: os << *left << " * " << *right; break;
				case DIV: os << *left << " / " << *right; break;
				case POS: os << *left;                    break;
				case NEG: os << "-(" << *left << ')';     break;
			}
		};
	};
	
	struct name_func : public func {		
		std::string name;
		
		name_func(const std::string& _name) : name(_name) {};
		
		~name_func() {};
		
		double evaluate(const std::map<std::string, double>& values, const std::map<std::string, std::function<double(const std::vector<double>&)>>& functions) {
			auto it = values.find(name);
		
			if (it == values.end())
				throw std::invalid_argument("value for " + name + " not defined");
			
			return it->second;
		};
		
		void print(std::ostream& os) const {
			os << name;
		};
	};
	
	struct const_func : public func {		
		double val;
		
		const_func(double _val) : val(_val) {};
		
		~const_func() {};
		
		double evaluate(const std::map<std::string, double>& values, const std::map<std::string, std::function<double(const std::vector<double>&)>>& functions) {
			return val;
		};
		
		void print(std::ostream& os) const {
			os << val;
		};
	};
	
	struct call_func : public func {		
		std::string name;
		std::vector<func*> args;
		
		call_func(const std::string& _name, const std::vector<func*>& _args) : name(_name), args(_args) {};
		
		~call_func() {
			for (int i = 0; i < args.size(); ++i)
				delete args[i];
		};
		
		double evaluate(const std::map<std::string, double>& values, const std::map<std::string, std::function<double(const std::vector<double>&)>>& functions) {
			auto it = functions.find(name);
		
			if (it == functions.end())
				throw std::invalid_argument("function for " + name + " not defined");
			
			std::vector<double> argv(args.size());
			
			for (int i = 0; i < args.size(); ++i)
				argv[i] = args[i]->evaluate(values, functions);
			
			return it->second(argv);
		};
		
		void print(std::ostream& os) const {
			os << name << '(';
			for (int i = 0; i < args.size(); ++i) {
				os << *args[i];
				if (i != args.size())
					os << ", ";
			}
			os << ')';
		};
	};
	
	namespace parser {
		enum token_type {
			TNAME, TNUM,
			TLBR, TRBR,
			TPLS, TMNS, TMUL, TDIV, 
			TCOL, // ','
			TERR
		};
		
		struct token {
			token_type type;
			std::string str;
			double dbl;
			
			token(token_type _type) : type(_type) {};
			token(std::string _str) : type(TNAME), str(_str) {};
			token(double _dbl) : type(TNUM), dbl(_dbl) {};
		};
		
		void print(const std::vector<token>& tokens) {
			for (int i = 0; i < tokens.size(); ++i) {
				switch(tokens[i].type) {
					case TNAME: std::cout << tokens[i].str; break;
					case TNUM : std::cout << tokens[i].dbl; break;
					case TLBR : std::cout << '(';           break;
					case TRBR : std::cout << ')';           break;
					case TPLS : std::cout << '+';           break;
					case TMNS : std::cout << '-';           break;
					case TMUL : std::cout << '*';           break;
					case TDIV : std::cout << '/';           break;
					case TCOL : std::cout << ',';           break;
					case TERR : std::cout << "TERR ";       break;
				};
			}
			std::cout << std::endl;
		};
		
		std::vector<token> tokenize(const std::string& in) {
			std::vector<token> tokens;
			
			int i = 0;
			
			auto alpha = [](int c) -> bool {
				return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_';
			};
			
			auto digit = [](int c) -> bool {
				return '0' <= c && c <= '9';
			};
			
			auto white = [](int c) -> bool {
				return c == ' ' || c == '\n' || c == '\r' || c == '\t';
			};
			
			while (1) {
				if (i >= in.size())
					break;
				
				if (white(in[i])) { ++i; continue; }
				
				if (alpha(in[i])) {
					std::string tmp;
					
					while (i < in.size() && alpha(in[i])) {
						tmp += in[i];
						++i;
					}
					
					tokens.push_back({ tmp });
				} else if (	digit(in[i]) // i, i.
							|| 
							(i < in.size() - 1 // {-,+}i, {-,+}.
								&& 
								(in[i] == '-' 
									|| 
									in[i] == '+') 
								&& 
								(digit(in[i + 1])
									||
									in[i + 1] == '.')
							)
							|| 
							(i < in.size() - 1 // .i
								&& 
								in[i] == '.' 
								&& 
								digit(in[i + 1])
							)
						  ) {
					std::string tmp;
					
					bool has_point = 0;
					
					if (in[i] == '.') {
						has_point = 1;
						tmp += "0.";
						++i;
					} else if (in[i] == '-' || in[i] == '+') {
						tmp += in[i];
						++i;
						
						if (in[i] == '.') {
							has_point = 1;
							++i;
						}
					}
					
					while (i < in.size() && (digit(in[i]) || !has_point && in[i] == '.')) {
						tmp += in[i];
						
						if (in[i] == '.')
							has_point = 1;
						
						++i;
					}
					
					tokens.push_back({ std::stod(tmp) });
				} else if (in[i] == '-') {
					++i;
					tokens.push_back({ TMNS });
				} else if (in[i] == '+') {
					++i;
					tokens.push_back({ TPLS });
				} else if (in[i] == '*') {
					++i;
					tokens.push_back({ TMUL });
				} else if (in[i] == '/') {
					++i;
					tokens.push_back({ TDIV });
				} else if (in[i] == '(') {
					++i;
					tokens.push_back({ TLBR });
				} else if (in[i] == ')') {
					++i;
					tokens.push_back({ TRBR });
				} else {
					tokens.clear();
					tokens.push_back({ TERR });
					return tokens;
				}
			}
			
			return tokens;
		};
		
		func* add_exp(const std::vector<token>& tokens, int& index);
		
		func* atomic(const std::vector<token>& tokens, int& index) {
			if (index < tokens.size()) {				
				if (tokens[index].type == TNAME) { // name, name(...)
					std::string name = tokens[index].str;
					++index;
					
					if (index < tokens.size() && tokens[index].type == TLBR) {						
						++index;
						
						std::vector<func*> args;
						
						while (1) {
							// Check EOF
							if (index >= tokens.size() - 1) {
								for (int i = 0; i < args.size(); ++i)
									delete args[i];
								return nullptr;
							}
							
							// Check ')'
							if (tokens[index].type == TRBR) {
								++index;
								break;
							}
							
							// Read arg[i]
							func* exp2 = add_exp(tokens, index);
							if (!exp2) {
								for (int i = 0; i < args.size(); ++i)
									delete args[i];
								return nullptr;
							}
							args.push_back(exp2);
							
							// Check EOF
							if (index >= tokens.size() - 1) {
								for (int i = 0; i < args.size(); ++i)
									delete args[i];
								return nullptr;
							}
							
							// Check ')'
							if (tokens[index].type == TRBR) {
								++index;
								break;
							}
							
							// Check ','
							if (tokens[index].type != TCOL) {
								for (int i = 0; i < args.size(); ++i)
									delete args[i];
								return nullptr;
							} else
								++index;
						}
						
						return new call_func(name, args);
					} else 
						return new name_func(name);
				}
				
				if (tokens[index].type == TPLS) { // +exp
					++index;
					func* exp2 = atomic(tokens, index);
					
					if (!exp2) 
						return nullptr;
					
					return new operator_func(operator_func::POS, exp2);
				}
				
				if (tokens[index].type == TMNS) { // -exp
					++index;
					func* exp2 = atomic(tokens, index);
					
					if (!exp2) 
						return nullptr;
					
					return new operator_func(operator_func::NEG, exp2);
				}
				
				if (tokens[index].type == TNUM) { // num
					++index;
					
					return new const_func(tokens[index - 1].dbl);
				}
				
				if (tokens[index].type == TLBR) { // (...)
					++index;
					
					func* brk = add_exp(tokens, index);
					
					if (index >= tokens.size() || tokens[index].type != TRBR) {
						delete brk;
						return nullptr;
					} else
						++index;
					
					return brk;
				}
				
				return nullptr;
			} else
				return nullptr;
		};
		
		func* mul_exp(const std::vector<token>& tokens, int& index) {
			func* exp1 = atomic(tokens, index);
			
			if (!exp1)
				return nullptr;
			
			if (index < tokens.size()) {				
				if (tokens[index].type == TMUL) {
					++index;
					func* exp2 = mul_exp(tokens, index);
					
					if (!exp2) {
						delete exp1;
						return nullptr;
					}
					
					return new operator_func(operator_func::MUL, exp1, exp2);
				}
				
				if (tokens[index].type == TDIV) {
					++index;
					func* exp2 = mul_exp(tokens, index);
					
					if (!exp2) {
						delete exp1;
						return nullptr;
					}
					
					return new operator_func(operator_func::DIV, exp1, exp2);
				}
				
				return exp1;
			} else
				return exp1;
		};
		
		func* add_exp(const std::vector<token>& tokens, int& index) { 
			func* exp1 = mul_exp(tokens, index);
			
			if (!exp1)
				return nullptr;
			
			if (index < tokens.size()) {
				if (tokens[index].type == TPLS) {
					++index;
					func* exp2 = add_exp(tokens, index);
					
					if (!exp2) {
						delete exp1;
						return nullptr;
					}
					
					return new operator_func(operator_func::ADD, exp1, exp2);
				}
				
				if (tokens[index].type == TMNS) {
					++index;
					func* exp2 = add_exp(tokens, index);
					
					if (!exp2) {
						delete exp1;
						return nullptr;
					}
					
					return new operator_func(operator_func::SUB, exp1, exp2);
				}
				
				return exp1;
			} else
				return exp1;
		};
	
		func* parse(const std::string& in) {
			int index = 0;
			std::vector<token> tokens = tokenize(in);
			
			if (tokens.size() == 1 && tokens[0].type == TERR)
				return nullptr;
			
			func* exp = add_exp(tokens, index);
			return exp;
		};
	};
	
	// Does parsing the input string and returns resulting function.
	// On error ocurred, returns nullptr.
	static func* parse(const std::string& in) {
		return parser::parse(in);
	};
};