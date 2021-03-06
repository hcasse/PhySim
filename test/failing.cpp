/*
 * reactive.cpp
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

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

class FailingTest: public ReactiveTest {
public:
	Square s;
	OutputPort<int> x;
	InputPort<int> x2;

	FailingTest():
		ReactiveTest("failing-test"),
		s("square", this),
		x(this, "x"),
		x2(this, "x2")
	{
		connect(x, s.x);
		connect(s.x2, x2);
	}

	void test() override {
		x = 2;
		step();
		check(x2, 4);
		x = 0;
		step();
		check(x2, 2);
		check(x2, 1);
		x = 1;
		step();
		check(x2, 1);
	}
};

int main() {
	return FailingTest().run();
}



