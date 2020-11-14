/*
 * reactive.cpp
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

#include <physim.h>
#include <physim/std.h>
#include <physim/test.h>
using namespace physim;

class Accu: public PeriodicModel {
public:
	InputPort<int> x;
	OutputPort<int> y;

	Accu(string name, duration_t period, ComposedModel *parent):
		PeriodicModel(name, period, parent),
		x(this, "x"),
		y(this, "y"),
		s(0)
	{ }
protected:

	void update(date_t at) override {
		s = s + x;
		y = s;
	}

private:
	int s;
};

class ConstantDisplayTest: public Simulate {
public:
	Accu a;
	Constant<int> one;
	Display<int> disp;

	ConstantDisplayTest():
		Simulate("constant-display-test"),
		a("accu", 2, this),
		one(1, this),
		disp("out", this)
	{
		connect(one.y, a.x);
		connect(a.y, disp.x);
	}
};

PHYSIM_RUN(ConstantDisplayTest)




