#include<iostream>

using namespace std;

class B;
class A {
	public:
	B* ptr;
};

class B {
	char array[1024];
	public:
	
	int value = 12;
	
	B() {};
	
	B(A* a) {
		a->ptr = this;
		cout << "constructor B addr = " << reinterpret_cast<std::uintptr_t>(this) << endl;
	}
};

int main() {
	B b;
	A a;
	
	b = B(&a);
	
	cout << "stack B addr       = " << reinterpret_cast<std::uintptr_t>(&b) << endl;
	
	a.ptr->value = 10;
	cout << "B.value            = " << b.value << endl;
	
	return 0;
};

