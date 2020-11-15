/*
 * Reporter test
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

#include <physim.h>
#include <physim/qtcharts.h>
#include <physim/std.h>
using namespace physim;

class Accu: public PeriodicModel {
public:
	InputPort<int> x;
	OutputPort<int> y;
	OutputPort<int> y2;

	Accu(string name, duration_t period, ComposedModel *parent):
		PeriodicModel(name, period, parent),
		x(this, "x"),
		y(this, "y"),
		y2(this, "y2"),
		s(0)
	{ }
protected:

	void update(date_t at) override {
		s = s + x;
		y = s;
		y2 = s * s;
	}

private:
	int s;
};

class QtLineTest: public Simulate {
public:
	Accu a;
	QtLineDisplay r;
	Constant<int> one;

	QtLineTest():
		Simulate("constant-display-test"),
		a("accu", 2, this),
		r("report", this),
		one(1, this)
	{
		connect(one.y, a.x);
		r.add(one.y);
		r.add(a.y);
		r.add(a.y2);
	}
};

PHYSIM_RUN(QtLineTest)




