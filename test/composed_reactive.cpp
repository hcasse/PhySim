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

class Twice: public ReactiveModel {
public:
	InputPort<int> x;
	OutputPort<int> twox;

	Twice(string name, ComposedModel *parent):
		ReactiveModel(name, parent),
		x(this, "x"),
		twox(this, "twox")
	{ }
protected:

	void update() override {
		twox = x * 2;
	}
};

class ComposedReactiveTest: public ReactiveTest {
public:
	Square s;
	Twice t;
	OutputPort<int> x;
	InputPort<int> y;

	ComposedReactiveTest():
		ReactiveTest("square-test"),
		s("square", this),
		t("twice", this),
		x(this, "x"),
		y(this, "y")
	{
		connect(x, s.x);
		connect(s.x2, t.x);
		connect(t.twox, y);
	}

	void test() override {
		x = 2;
		step();
		check(y, 8);

		x = 0;
		step();
		check(y, 0);

		x = 1;
		step();
		check(y, 2);
	}
};

int main() {
	return ComposedReactiveTest().run();
}



