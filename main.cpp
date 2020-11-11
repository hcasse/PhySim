#include <iostream>
using namespace std;

#include <physim.h>
#include <physim/test.h>
using namespace physim;

class Square: public ReactiveModel {
public:
	InputPort<int> x;
	OutputPort<int> x2;

	Square(string name, ComposedModel *parent):
		ReactiveModel(name, parent),
		x(this, "x"),
		x2(this, "x2")
	{ }
protected:

	void update() override {
		x2 = x * x;
	}
};

class SquareTest: public ReactiveTest {
public:
	Square s;
	OutputPort<int> x;
	InputPort<int> x2;

	SquareTest():
		ReactiveTest("square-test"),
		s("square", this),
		x(this, "x"),
		x2(this, "x2")
	{
		connect(x, s.x);
		connect(s.x2, x2);
	}

	void test() override {
		x = 2;
		check(x2, 4);
		x = 0;
		check(x2, 3);
		x = 1;
		check(x2, 1);
	}
};

int main() {
	SquareTest().run();
}

