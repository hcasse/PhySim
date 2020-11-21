/*
 * reactive.cpp
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

#include <physim.h>
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

	void init() override {
		y = s;
	}

	void update(date_t at) override {
		s = s + x;
		y = s;
	}

private:
	int s;
};

class SimplePeriodicTest: public PeriodicTest {
public:
	Accu a;
	OutputPort<int> x;
	InputPort<int> y;

	SimplePeriodicTest():
		PeriodicTest("simple-periodic-test", a, 10),
		a("accu", 2, this),
		x(this, "x"),
		y(this, "y")
	{
		connect(x, a.x);
		connect(a.y, y);
	}

	void init() override {
		x = 1;
	}

	void test(date_t date) override {
		check(y, date / 2);
	}
};

PHYSIM_RUN(SimplePeriodicTest)




