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
*/

#pragma once

#include <iostream>
#include <cstring>
#include <assert.h>
#include <cstdint>
#include <cstdlib>

namespace big_number {
	class base_number {
		// Size of the number base
		int base = 2;
		
		// Number buffer
		int len = 0;
		int size = 0;
		unsigned char *map = nullptr;
		
		void allocate(int s) {
			size = s;
			map = (unsigned char*) calloc(1, size);
		};
		
		void enlarge() {
			int temp_size = size;
			map = (unsigned char*) realloc(map, size <<= 2);
			
			assert(map != nullptr);
			
			for (int i = temp_size; i < size; ++i)
				map[i] = 0;
		};
		
		void deallocate() {
			free(map);
			size = 0;
			len = 0;
		};
		
		void compact() {
			while (size > 16 && len < (size >> 1))
				map = (unsigned char*) realloc(map, size >>= 1);
		}
		
		void calc_len() {
			for (len = size; len > 1; --len)
				if (map[len - 1])
					break;
		};
		
	public:

		base_number(const base_number &n) {
			allocate(n.size);
			len = n.len;
			base = n.base;
			
			for (int i = 0; i < n.len; ++i)
				map[i] = n.map[i];
		}

		base_number(long long n = 0, int base = 10) {
			assert(("2 <= base <= 256", base >= 2 && base <= 256));
			
			allocate(sizeof(long long) * 8);
			
			this->base = base;
			
			int i = 0;
			while (n) {
				map[i] = n % base;
				n /= base;
				++i;
			}
			calc_len();
		};
		
		~base_number() {
			deallocate();
		};
		
		unsigned char *get_map() {
			return map;
		};
		
		int get_digit(int i) const {
			if (i < 0 || i >= size)
				return 0;
			
			return map[i];
		};
		
		void set_digit(int i, int n) {
			if (i < 0)
				return;
			
			while (i >= size) enlarge();
			
			map[i] = n;
		};
		
		int get_len() const {
			return len;
		};
		
		int get_size() const {
			return size;
		};
		
		void set_len(int len_) {
			len = len_;
		};
		
		void set_zero() {
			for (int i = 0; i < len; ++i)
				map[i] = 0;
			
			len = 0;
			compact();
		};
		
		bool is_zero() const {
			return len <= 1 && get_digit(0) == 0;
		};
		
		void add(int digit_pos, long long n) {
			long long carry = n;
			long long i = digit_pos;
			
			while (carry) {
				long long a = get_digit(i) + carry;
				carry = a / base;
				set_digit(i, a % base);
				++i;
			}
			calc_len();
		};
		
		void add(long long n) {
			add(0, n);
		};
		
		void add(const base_number &b) {
			if (base == b.base)
				for (int i = 0; i < b.len; ++i)
					add(i, b.get_digit(i));
			else for (int i = 0; i < b.len; ++i) {					
					base_number m(b.map[i], base);	
					
					// XXX: Use single base multiplier instead of multiply on each cycle
					for (int j = 0; j < i; ++j)
						m.mul(b.base);
					
					add(m);
				}
			calc_len();
		};
		
		void mul(long long n) {
			if (n == 0) {
				set_zero();
				return;
			}
			for (int i = len - 1; i >= 0; --i) {
				long long t = n;
				int j = 0;
				long long digit = get_digit(i);
				set_digit(i, 0);
				while (t) {
					add(i + j, digit * (t % base));
					
					t /= base;
					++j;
				}
			}
			calc_len();
		};
		
		base_number operator=(const base_number &b) {
			deallocate();
			allocate(b.size);
			len = b.len;
			
			for (int i = 0; i < b.len; ++i)
				map[i] = b.map[i];
			
			return *this;
		};
		
		base_number operator+(const base_number &b) {
			base_number n = *this;
			if (base == b.base)
				for (int i = 0; i < b.len; ++i)
					n.add(i, b.get_digit(i));
			else {
				for (int i = 0; i < b.len; ++i) {					
					base_number m(b.map[i], base);
					
					for (int j = 0; j < i; ++j)
						m.mul(b.base);
					
					n.add(m);
				}
			}
			
			return n;
		};
		
		base_number operator*(const base_number &b) {
			base_number n(0, base);
			
			if (b.is_zero()) 
				return n;
			
			base_number o;
			if (base != b.base) {
				o = base_number(0, base);
				o = o + b;
			} else
				o = b;
			
			for (int i = 0; i < len; ++i) 
				for (int j = 0; j < o.len; ++j) 
					n.add(i + j, get_digit(i) * o.get_digit(j));
				
			return n;
		};

		friend std::ostream& operator<<(std::ostream& os, const base_number& b) {
			for (int i = b.len - 1; i >= 0; --i) {
				std::cout << (int) b.map[i];
				if (i != 0)
					std::cout << ' ';
			}
			return os;
		};
		
	};

	/*
	 * Big int class. 
	 * Dynamically allocates new memory for storing very big numbers.
	 * Uses binary array for representing numbers
	 */
	class bigint {
		unsigned char *map;
		int size = 0;
		int len  = 0;
		// + ~ 0
		// - ~ 1
		bool sign;
		
		void setbit(int n, int b) {
			set_byte(n >> 3, get_byte(n >> 3) & ~(1 << (n & 0x7)) | (b << (n & 0x7)));
		};
		
		int getbit(int n) const {
			return (get_byte(n >> 3) >> n) & 1;
		};
		
		/* Add v to nth digit */
		void abs_add_digit(int n, long long v) {
			long long carry = v;
			
			for (int j = n; carry; ++j) {
				long long a = get_byte(j);
				long long b = a + carry;
				
				set_byte(j, b & 0xFF);
				
				carry = b >> 8;
				// Used for keeping max size to avoid get_byte returning 0
				len = size;
			}
		}
		
		/* Sub v to nth digit */
		void abs_sub_digit(int n, unsigned long long v) {		
			for (int i = len - 1; i >= n; --i) {
				int x = get_byte(i);
				int y = (v >> ((i - n) << 3)) & 0xFF;
				
				if (x < y) {
					set_byte(i, 256 + x - y);
					int j = i + 1;
					while (get_byte(j) == 0) {
						set_byte(j, 0xFF);
						++j;
					}
					set_byte(j, get_byte(j) - 1);
				} else {
					set_byte(i, x - y);
				}
			}
		}
		
		/* Add passed number without changing sign */
		void abs_add(const bigint &b) {		
			int tlen = len < b.len ? b.len : len;
			
			for (int i = tlen - 1; i >= 0; --i) 
				abs_add_digit(i, b.get_byte(i));
			
			calc_len();
		};
		
		/* Sub passed number without changing sign */
		void abs_sub(const bigint &b) {
			// Assuming self > b
			
			for (int i = len - 1; i >= 0; --i) {
				int x = get_byte(i);
				int y = b.get_byte(i);
				
				if (x < y) {
					set_byte(i, 256 + x - y);
					int j = i + 1;
					while (get_byte(j) == 0) {
						set_byte(j, 0xFF);
						++j;
					}
					set_byte(j, get_byte(j) - 1);
				} else 
					set_byte(i, x - y);
			}
			
			calc_len_down();
			compact();
		};
		
		/* Compact used space by freeing it */
		inline void compact() {
			while (size > 16 && len < (size >> 1))
				map = (unsigned char*) realloc(map, size >>= 1);
		}
		
		/* Enlarge map by calling realloc and filling it with zeros */
		inline void enlarge() {
			int start = size;
			
			map = (unsigned char*) realloc(map, size <<= 2);
			
			for (int i = start; i < size; ++i)
				map[i] = 0;
		};
		
		uint64_t round_int(uint64_t v) {
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8; // 2b
			if (sizeof(long long) >= 4) {
				v |= v >> 16; // 4b
				if (sizeof(long long) >= 8) {
					v |= v >> 32; // 8b
					if (sizeof(long long) >= 16) {
						v |= v >> 64; // 16b
						if (sizeof(long long) >= 32) {
							v |= v >> 128; // 32b
							if (sizeof(long long) >= 64)
								v |= v >> 256; // 64b
						}
					}
				}
			}
			v++;
			return v;
		};
		
		/* Allocates first space, fill with zeros */
		void allocate(int size) {
			size = size < sizeof(long long) ? sizeof(long long) : size;
			size = (size && !(size & (size - 1))) ? size : round_int(size);
			this->size = size;
			this->map = (unsigned char*) calloc(size, 1);
		}
		
		/* Frees the memory */
		void deallocate() {
			free(map);
		};
		
	public:

		static const bool POSITIVE = 0;
		static const bool NEGATIVE = 1;

		bigint() {
			allocate(sizeof(long long));
			
			this->len = 1;
			this->sign = 0;
			
			map[0] = 0;
		};
		
		bigint(const bigint &b) {
			allocate(b.len);
			
			this->len = b.len;
			this->sign = b.sign;
			
			for (int i = 0; i < len; ++i)
				this->map[i] = b.map[i];
		};
		
		bigint(long long l) {
			allocate(sizeof(long long));
			
			this->sign = l < 0;
			
			if (l < 0)
				l = -l;
			
			unsigned long long t = l;
			for (int i = 0; i < sizeof(long long); ++i)
				set_byte(i, (t >> i * 8) & 0xFF);
			
			calc_len();
			
			if (this->len == 0)
				this->len = 1;
		};		
		
		bigint(const char *decimal_string, int base) {
			allocate(sizeof(long long));
			
			if (decimal_string == nullptr)
				return;
			
			int stlen = strlen(decimal_string);
			
			if (stlen == 0)
				return;
			
			int cursor = 0;
			sign = decimal_string[cursor] == '-';
			
			bool has_sign = 0;
			
			if (has_sign = (decimal_string[cursor] == '-' || decimal_string[cursor] == '+'))
				++cursor;
			
			if (cursor && stlen == 1) {
				sign = 0;
				return;
			}
			
			// string -> decimal number
			base_number n(0, base);
			for (; cursor < stlen; ++cursor) 
				if (decimal_string[cursor] <= '9' && decimal_string[cursor] >= '0')
					n.set_digit(stlen - cursor - 1, decimal_string[cursor] - '0');
				else if (base > 10 && decimal_string[cursor] <= 'Z' && decimal_string[cursor] >= 'A')
					n.set_digit(stlen - cursor - 1, decimal_string[cursor] - 'A' + 10);
				else if (base > 10 && decimal_string[cursor] <= 'z' && decimal_string[cursor] >= 'a')
					n.set_digit(stlen - cursor - 1, decimal_string[cursor] - 'a' + 10);
				else
					break;
			n.set_len(stlen - (has_sign ? 1 : 0));
			
			// decimal number -> bigint
			base_number t(0, 256);
			t.add(n);
			
			for (int i = 0; i < t.get_len(); ++i)
				set_byte(i, t.get_digit(i));
			calc_len();
		};
		
		~bigint() {
			deallocate();
		};
		
		
		/* Returns n-th byte if maps, else 0 */
		inline int get_byte(int n) const {
			return (n >= len || n < 0) ? 0 : map[n];
		};
		
		/* Sets n-th byte, returns amount of reallocs */
		inline int set_byte(int n, int byte) {
			if (n < 0)
				return 0;
			
			int inc = 0;
			while (size <= n) { enlarge(); ++inc; }
			
			map[n] = byte;
			
			return inc;
		}
		
		/* size -> new_len */
		inline void calc_len() {
			for (len = size; len > 1; --len)
				if (map[len - 1])
					break;
		};
			
		/* len -> new_len */
		inline void calc_len_down() {
			for (; len > 1; --len)
				if (map[len - 1])
					break;
		};
			
		
		/* Convert number to decimal string by base*/
		char *toCString(int base) const {
			assert(("2 <= base <= 36", 2 <= base && base <= 36));
			
			base_number n(0, base);
			
			for (int i = 0; i < len; ++i) {
				base_number t(map[i], base);
				
				for (int k = 0; k < i; ++k)
					t.mul(256);
				
				n.add(t);
			}
			
			int size = n.get_size() + 2;
			int len = 0;
			char *string = (char*) malloc(size);
			
			if (sign)
				string[len++] = '-';
			
			for (int i = n.get_len() - 1; i >= 0; --i) {
				string[len++] = n.get_map()[i] > 9 ? n.get_map()[i] + 'A' - 10 : n.get_map()[i] + '0';
			}
			
			string[len++] = 0;

			return string;
		};

		/* Prints hex dump */
		void print_dump() const {
			if (sign)
				std::cout << '-';
			else
				std::cout << '+';
			
			std::cout << "0x";
			
			for (int i = len - 1; i >= 0; --i) {
				int c0 = map[i] & 0xF;
				int c1 = (map[i] >> 4) & 0xF;
				
				c0 += c0 > 9 ? 'A'-10 : '0';
				c1 += c1 > 9 ? 'A'-10 : '0';
				
				std::cout << (char) c1 << (char) c0;
			}
			std::cout << std::endl;
		};

		void print_raw_dump() const {
			if (sign)
				std::cout << '-';
			else
				std::cout << '+';
			
			std::cout << "0x";
			
			for (int i = size - 1; i >= 0; --i) {
				int c0 = map[i] & 0xF;
				int c1 = (map[i] >> 4) & 0xF;
				
				c0 += c0 > 9 ? 'A'-10 : '0';
				c1 += c1 > 9 ? 'A'-10 : '0';
				
				std::cout << (char) c1 << (char) c0;
			}
			std::cout << std::endl;
		};

		/* returns 1 if sign is 0 */
		inline bool is_positive() const {
			return !sign;
		};

		/* returns byte len */
		inline int get_len() const {
			return len;
		}
		
		/* returns bytemap size */
		inline int get_size() const {
			return size;
		}
		
		/* returns bytemap */
		inline unsigned char *get_map() const {
			return map;
		};
		
		/* Set sign of a number. 1 = negative, 0 = positive */
		inline void set_sign(int s) {
			if (sign != 0 && sign != 1)
				return;
			
			sign = s;
		};
		
		/* Set number to zero */
		inline void set_zero() {
			for (int i = 0; i < len; ++i)
				map[i] = 0;
			len = 1;
			sign = 0;
			compact();
		}
		
		/* Check if zero */
		inline bool is_zero() const {
			return len <= 1 && get_byte(0) == 0;
		};
		
		/* Clear the bytemap */
		void clear() {
			sign = 0;
			deallocate();
			allocate(sizeof(long long));
		};
		
		/* return long long value representing this bigint. signed */
		long long int_value() const {
			long long n = 0;
			for (int i = 0; i < sizeof(long long); ++i)
				n |= get_byte(i) << (i * 8);
			
			if (sign)
				n = -n;
			
			return n;
		};
		
		
		bigint operator=(const bigint &b) {
			deallocate();
			allocate(b.len);
			
			this->len = b.len;
			this->sign = b.sign;
			
			for (int i = 0; i < len; ++i)
				this->map[i] = b.map[i];
			
			return *this;
		};
		
		
		void add(const bigint &b) {
			if (sign == b.sign) {
				abs_add(b);
				return;
			}
			
			if (abs_greater(b)) { // |this| > |b|
				// sign = sign;
				abs_sub(b);			
			} else if (b.abs_greater(*this)) { // |self| < |b|
				bigint temp = b;
				temp.abs_sub(*this);
				
				set_zero();
				for (int i = 0; i < temp.len; ++i)
					set_byte(i, temp.get_byte(i));
				
				sign = b.sign;
				len = temp.len;
			} else { 
				sign = 0;
				set_zero();
			}
		};
		
		bigint operator+(const bigint &b) {
			bigint n = *this;
			n.add(b);
			return n;
		};
		
		friend bigint operator+(long long l, const bigint &b) {
			bigint n = l;
			n.add(b);
			return n;
		}
		
		
		void sub(const bigint &b) {
			if (sign != b.sign) {
				abs_add(b);
				return;
			}
			
			if (abs_greater(b)) { // |this| > |b|
				// sign = sign;
				abs_sub(b);			
			} else if (b.abs_greater(*this)) { // |self| < |b|
				bigint temp = b;
				temp.abs_sub(*this);
				
				set_zero();
				for (int i = 0; i < temp.len; ++i)
					set_byte(i, temp.get_byte(i));
				
				sign = !b.sign;
				len = temp.len;
			} else {
				sign = 0;
				set_zero();
			}
		}; 
		
		bigint operator-(const bigint &b) {
			bigint n = *this;
			n.sub(b);
			return n;
		};
		
		friend bigint operator-(long long l, const bigint &b) {
			bigint n = l;
			n.sub(b);
			return n;
		};
		
		
		void mul(const bigint &b) {
			if (b.is_zero()) {
				set_zero();
				return;
			}
			if (is_zero())
				return;
			
			sign = (sign == 0) ^ (b.sign == 0);
			
			for (int j = len - 1; j >= 0; --j) {
				long long digit = get_byte(j);
				set_byte(j, 0);
				for (int i = 0; i < b.len; ++i) 
					abs_add_digit(i + j, digit * b.get_byte(i)); 
			}
			
			calc_len();
		};
		
		bigint operator*(const bigint &b) {
			if (is_zero() || b.is_zero())
				return 0;
			
			bigint n = *this;
			n.mul(b);
				
			return n;
		};
		
		friend bigint operator*(long long l, const bigint &b) {
			if (l == 0 || b.is_zero())
				return 0;
			
			bigint n = l;
			n.mul(b);
			
			return n;
		};
		
		
		void div(const bigint &d) {
			assert(("Can't divide by zero", !d.is_zero()));
			
			if (d.get_len() == 1 && d.get_byte(0) == 1) {
				sign = sign ^ d.sign;
				return;
			}
			
			if (len < d.len) {
				set_zero();
				return;
			}
			
			bigint n;
			bigint b = d;
			int b_len = b.len;
			b.sign = sign;
			
			n.sign = sign ^ d.sign;
			
			b.shl((len - b_len) * 8);
			
			while (b.len >= b_len) {
				n.shl(8);
				
				unsigned char res = 0;
				
				while (abs_greater_equals(b)) {
					sub(b);
					++res;
				}
				
				if (get_byte(len - 1) == 0)
					--len;
				
				n.set_byte(0, res);
				
				int b_len_t = b.len;
				b.shr(8);
				if (b_len_t == 1)
					b.len = 0;
			}
			
			set_zero();
			sign = n.sign;
			n.calc_len();
			
			for (int i = 0; i < n.get_len(); ++i)
				set_byte(i, n.get_byte(i));
			calc_len();
		};
		
		bigint operator/(const bigint &b) {
			bigint n = *this;
			n.div(b);
			return n;
		}
		
		friend bigint operator/(long l, const bigint &b) {
			bigint n = l;
			n.div(b);
			return n;
		}
		
		
		void abs_mod(const bigint &d) {
			assert(("Can't divide by zero", !d.is_zero()));
			
			sign = 0;
			if (d.get_len() == 1 && d.get_byte(0) == 1) {
				set_zero();
				return;
			}
			
			if (len < d.len) 
				return;
			
			bigint b = d;
			int b_len = b.len;
			b.sign = 0;
			
			b.shl((len - b_len) * 8);
			
			while (b.len >= b_len) {	
				while (abs_greater_equals(b)) 
					sub(b);
				
				if (get_byte(len - 1) == 0)
					--len;
				
				int b_len_t = b.len;
				b.shr(8);
				if (b_len_t == 1)
					b.len = 0;
			}
			
			calc_len();
		};
		
		bigint operator%(const bigint &b) {
			bigint n = *this;
			n.abs_mod(b);
			return n;
		}
		
		friend bigint operator%(long long l, const bigint &b) {
			bigint n = l;
			n.abs_mod(b);
			return n;
		}
		
		
		bigint operator-() {
			bigint n = *this;
			n.sign ^= 1;
			return n;
		};
		
		bigint operator+() {
			bigint n = *this;
			return n;
		};
		
		
		bool abs_equals(const bigint &b) const {
			if (len != b.len)
				return 0;
			
			for (int i = 0; i < len; ++i)
				if (map[i] != b.map[i])
					return 0;
			
			return 1;
		};
		
		bool abs_greater(const bigint &b) const {
			if (len < b.len) 
				return 0;
			if (len > b.len)
				return 1;
			
			for (int i = len - 1; i >= 0; --i) 
				if (map[i] < b.get_byte(i))
					return 0;
				else if (map[i] > b.get_byte(i))
					return 1;
					
			return 0;
		};
		
		bool abs_greater_equals(const bigint &b) const {
			if (len < b.len) 
				return 0;
			if (len > b.len)
				return 1;
			
			for (int i = len - 1; i >= 0; --i)
				if (map[i] < b.get_byte(i))
					return 0;
				else if (map[i] > b.get_byte(i))
					return 1;
					
			return 1;
		};
		
		bool operator>(const bigint &b) {
			if (sign == 0 && b.sign == 1)
				return 1;
			if (sign == 1 && b.sign == 0)
				return 0;
			
			if (len < b.len) 
				return sign;
			if (len > b.len)
				return !sign;
			
			if (sign == 0)
				for (int i = len - 1; i >= 0; --i)
					if (map[i] < b.map[i])
						return 0;
					else if (map[i] > b.map[i])
						return 1;
			else 
				for (int i = len - 1; i >= 0; --i)
					if (map[i] > b.map[i])
						return 0;
					else if (map[i] < b.map[i])
						return 1;
					
			return 0;
		};
		
		friend bool operator>(long long l, const bigint &b) {
			bigint t = l;
			if (t.sign == 0 && b.sign == 1)
				return 1;
			if (t.sign == 1 && b.sign == 0)
				return 0;
			
			if (t.len < b.len) 
				return t.sign;
			if (t.len > b.len)
				return !t.sign;
			
			if (t.sign == 0)
				for (int i = t.len - 1; i >= 0; --i)
					if (t.map[i] < b.map[i])
						return 0;
					else if (t.map[i] > b.map[i])
						return 1;
			else 
				for (int i = t.len - 1; i >= 0; --i)
					if (t.map[i] > b.map[i])
						return 0;
					else if (t.map[i] < b.map[i])
						return 1;
					
			return 0;
		};
		
		bool operator>=(const bigint &b) {
			if (sign == 0 && b.sign == 1)
				return 1;
			if (sign == 1 && b.sign == 0)
				return 0;
			
			if (len < b.len) 
				return sign;
			if (len > b.len)
				return !sign;
			
			if (sign == 0)
				for (int i = len - 1; i >= 0; --i)
					if (map[i] < b.map[i])
						return 0;
					else if (map[i] > b.map[i])
						return 1;
			else 
				for (int i = len - 1; i >= 0; --i)
					if (map[i] > b.map[i])
						return 0;
					else if (map[i] < b.map[i])
						return 1;
					
			return 1;
		};

		friend bool operator>=(long long l, const bigint &b) {
			bigint t = l;
			if (t.sign == 0 && b.sign == 1)
				return 1;
			if (t.sign == 1 && b.sign == 0)
				return 0;
			
			if (t.len < b.len) 
				return t.sign;
			if (t.len > b.len)
				return !t.sign;
			
			if (t.sign == 0)
				for (int i = t.len - 1; i >= 0; --i)
					if (t.map[i] < b.map[i])
						return 0;
					else if (t.map[i] > b.map[i])
						return 1;
			else 
				for (int i = t.len - 1; i >= 0; --i)
					if (t.map[i] > b.map[i])
						return 0;
					else if (t.map[i] < b.map[i])
						return 1;
					
			return 1;
		};

		bool operator<(const bigint &b) {
			return !(*this >= b);
		};

		friend bool operator<(long long l, const bigint &b) {
			return !(l >= b);
		};

		bool operator<=(const bigint &b) {
			return !(*this > b);
		};
		
		friend bool operator<=(long long l, const bigint &b) {
			return !(l > b);
		};
		
		bool operator==(const bigint &b) {
			if (len != b.len)
				return 0;
			
			for (int i = 0; i < len; ++i)
				if (map[i] != b.map[i])
					return 0;
			
			return 1;
		};
		
		friend bool operator==(long long l, const bigint &b) {
			bigint t = l;
			if (t.len != b.len)
				return 0;
			
			for (int i = 0; i < t.len; ++i)
				if (t.map[i] != b.map[i])
					return 0;
			
			return 1;
		};
		
		bool operator!=(const bigint &b) {
			if (len != b.len)
				return 1;
			
			for (int i = 0; i < len; ++i)
				if (map[i] == b.map[i])
					return 0;
			
			return 1;
		};
		
		friend bool operator!=(long long l, const bigint &b) {
			bigint t = l;
			if (t.len != b.len)
				return 1;
			
			for (int i = 0; i < t.len; ++i)
				if (t.map[i] == b.map[i])
					return 0;
			
			return 1;
		};
		
		
		bool operator&&(const bigint &b) {
			return *this && b;
		};
		
		friend bool operator&&(long long l, const bigint &b) {
			return l && b;
		};
		
		
		bool operator||(const bigint &b) {
			return *this || b;
		};
		
		friend bool operator||(long long l, const bigint &b) {
			return l || b;
		};
		
		
		bigint operator!() {
			return is_zero() ? 1 : 0;
		};
		
		
		bigint operator&(const bigint &b) const {
			bigint n;
			
			int dis = len < b.len ? b.len : len;
			for (int i = 0; i < dis; ++i)
				n.set_byte(i, get_byte(i) & b.get_byte(i));
			
			n.calc_len();
			return n;
		};
		
		friend bigint operator&(long long l, const bigint &b) {
			return b & l;
		};
		
		
		bigint operator|(const bigint &b) const {
			bigint n;
			
			int dis = len < b.len ? b.len : len;
			for (int i = 0; i < dis; ++i)
				n.set_byte(i, get_byte(i) | b.get_byte(i));
			
			n.calc_len();
			return n;
		};
		
		friend bigint operator|(long long l, const bigint &b) {
			return b | l;
		};
		
		
		void invert(int size) {
			for (int i = size - 1; i >= 0; --i)
				set_byte(i, ~get_byte(i));
		}
		
		bigint operator~() const {
			bigint n = *this;
			n.invert(size);
			return n;
		}
		
		
		void shl(unsigned int n) {
			if (n == 0)
				return;
			
			if (n < 0) {
				shr(-n);
				return;
			}
			
			if ((n & 7) == 0) {
				for (int i = get_len() - 1; i >= 0; --i) 
					set_byte(i + (n >> 3), get_byte(i));
				for (int i = 0; i < (n >> 3); ++i) 
					set_byte(i, 0);
			} else {
				int shift = n >> 3;
				
				shl(shift << 3);
				
				int rest_high = (n & 7);
				int rest      = 8 - rest_high;
				
				int mask_a = 0xFF >> rest;           // 000xxxxx
				int mask_b = 0xFF >> rest_high;      // 00000xxx - b
				int mask_a_sh = mask_a << rest;      // xxxxx000 - a
				int mask_b_sh = mask_b << rest_high; // xxx00000
				
				for (int i = get_len() - 1; i >= 0; --i) {
					int byte  = get_byte(i);
					int byte1 = get_byte(i + 1);
					
					// 000|00000|aaaaa/bbb
					// 000|ccccc/bbb|00000
					
					int byte_c = (byte & mask_a_sh) >> rest;      // 000ccccc
					int byte_d = (byte & mask_b)    << rest_high; // ddd00000
					
					set_byte(i,     (byte  & mask_a)    | byte_d);
					set_byte(i + 1, (byte1 & mask_b_sh) | byte_c);
				}
				
				set_byte(0, get_byte(0) & mask_b_sh);
			}
			
			calc_len();
		}
		
		bigint operator<<(const bigint &b) {
			bigint n = *this;
			n.shl(b.int_value());
			return n;
		};
		
		friend bigint operator<<(long long l, const bigint &b) {
			bigint n = l;
			n.shl(b.int_value());
			return n;
		};
		
		
		void shr(unsigned int n) {
			if (n == 0)
				return;
			
			if (n < 0) {
				shl(-n);
				return;
			}
			
			if ((n & 7) == 0) {
				for (int i = 0; i < get_len(); ++i) 
					set_byte(i - (n >> 3), get_byte(i));
				for (int i = get_len() - (n >> 3); i < get_len(); ++i) 
					set_byte(i, 0);
			} else {
				int shift = n >> 3;
				
				shr(shift << 3);
				
				int rest      = (n & 7);
				int rest_high = 8 - rest;
				
				int mask_a = 0xFF >> rest;           // 000xxxxx
				int mask_b = 0xFF >> rest_high;      // 00000xxx - b
				int mask_a_sh = mask_a << rest;      // xxxxx000 - a
				int mask_b_sh = mask_b << rest_high; // xxx00000
				
				for (int i = 0; i < get_len(); ++i) {
					int byte  = get_byte(i);
					int byte1 = get_byte(i - 1);
					
					// |aaaaa/bbb|000|
					// |000|ccccc/bbb|
					
					int byte_c = (byte & mask_a_sh) >> rest;      // 000ccccc
					int byte_d = (byte & mask_b)    << rest_high; // ddd00000
					
					set_byte(i,     (byte  & mask_b_sh) | byte_c);
					set_byte(i - 1, (byte1 & mask_a)    | byte_d);
				}
				
				set_byte(get_len() - 1, get_byte(get_len() - 1) & mask_a);
			}
			
			calc_len();
		}
		
		bigint operator>>(const bigint &b) {
			bigint n = *this;
			n.shr(b.int_value());
			return n;
		};
		
		friend bigint operator>>(long long l, const bigint &b) {
			bigint n = l;
			n.shr(b.int_value());
			return n;
		};
		
		
		unsigned char &operator[](const int index) {
			get_byte(index);
			return map[index];
		};
		
		
		operator void*() const {
			return is_zero() ? (void*) 0 : (void*) this;
		};
		
		
		friend std::ostream& operator<<(std::ostream& os, const bigint& b) {
			// XXX: Support hex/octal/binary
			
			char *string = b.toCString(10);
			if (!string)
				return os;
			
			int len = strlen(string);
			for (int i = 0; i < len; ++i) 
				os << string[i];
			
			free(string);
			
			return os;
		};
		
		friend std::istream& operator>>(std::istream& is, bigint& b) {
			// XXX: Support hex/octal/binary
			
			b.set_zero();
			
			int sign = 0;
			base_number n(0, 10);
			
			int len = 0;
			int size = 0;
			char *string = nullptr;
			
			bool skipws = std::cin.flags() & std::ios_base::skipws ? 1 : 0;
			std::cin.unsetf(std::ios_base::skipws);
			
			int c = is.get();
			while (c == ' ' || c == '\t' || c == '\n') c = is.get();
			if (c != 0)
				is.putback(c);
			
			while (1) {
				if (is.peek() == EOF)
					break;
				
				c = is.get();
				
				if (c == '-' || c == '+') {
					if (sign || len) {
						is.putback(c);
						free(string);
						return is;
					}
					
					sign = 1;
					b.sign = c == '-' ? 1 : 0;
					
					continue;
				} else if (c < '0' || c > '9')
					break;
				
				if (size == 0)
					string = (char*) malloc(size = 16);
				else if (len >= size)
					string = (char*) realloc(string, size <<= 1);
				
				string[len++] = c;	
			}
			
			if (skipws)
				std::cin.setf(std::ios_base::skipws);
				
			for (int i = 0; i < len; ++i) 
				n.set_digit(len - i - 1, string[i] - '0');
			n.set_len(len);
			
			base_number t(0, 256);
			t.add(n);
			
			for (int i = 0; i < t.get_len(); ++i)
				b.set_byte(i, t.get_digit(i));
			b.sign = sign;
			b.calc_len();
			
			free(string);
		};
	};
	
	/* literal for <bigint>_g from long long */
	big_number::bigint operator "" _g(unsigned long long i) {
		return big_number::bigint(i);
	};

	/* literal for <bigint>_g from string */
	big_number::bigint operator "" _g(const char *decimal_string, size_t s) {
		return big_number::bigint(decimal_string, 10);
	};
}
