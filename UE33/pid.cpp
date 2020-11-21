/*
 * pid.cpp
 *
 *  Created on: 18 nov. 2020
 *      Author: casse
 */

#include <physim.h>
#include <physim/test.h>
#include <physim/std.h>
#include <physim/qtcharts.h>
using namespace physim;

const double
	Kp = .6,
	Ki = 0.2,
	Kd = 0.4;


// Add3 model and test
class Add3: public ReactiveModel {
public:
	InputPort<double> x, y, z;
	OutputPort<double> s;
	Add3(ComposedModel *parent):
		ReactiveModel("Add3", parent),
		x(this, "x"),
		y(this, "y"),
		z(this, "z"),
		s(this, "s")
	{ }

protected:

	void init() override {
		s = 0;
	}

	void update() override {
		s = x + y +z;
	}
};


class Add3Test: public ReactiveTest {
public:
	Add3 add3;
	OutputPort<double> x, y, z;
	InputPort<double> s;
	Add3Test():
		ReactiveTest("Add3Test"),
		add3(this),
		x(this, "x"),
		y(this, "y"),
		z(this, "z"),
		s(this, "s")
	{
		connect(x, add3.x);
		connect(y, add3.y);
		connect(z, add3.z);
		connect(add3.s, s);
	}

protected:
	void test() override {
		x = 1; y = 2; z = 3;
		step();
		check(s, 6.);

		x = 0; y = 0; z = 0;
		step();
		check(s, 0.);

		x = 0; y = 1; z = 0;
		step();
		check(s, 3.);
	}
};


// Prop model and test


// Deriv model and test


// Integ class and test
class Integ: public PeriodicModel {
public:

	InputPort<double> e;
	OutputPort<double> I;
	Integ(ComposedModel *parent):
		PeriodicModel("Integ", 1, parent),
		e(this, "e"),
		I(this, "I"),
		s(0)
	{ }

protected:
	void init() override {
		// TODO
	}

	void update(date_t date) override {
		// TODO
	}

private:
	double s;
};

class IntegTest: public PeriodicTest {
public:
	Integ integ;
	OutputPort<double> e;
	InputPort<double> I;

	IntegTest():
		PeriodicTest("IntegTest", integ, 20),
		integ(this),
		e(this, "e"),
		I(this, "I")
	{
		connect(e, integ.e);
		connect(integ.I, I);
	}

protected:

	void init() {
		e = 0;
	}

	void test(date_t date) {
		out() << date << ": e=" << e << ", I = " << I << endl;
		if(date == 5)
			e = 0.3;
		else if(date == 10)
			e = 0.1;
		else if(date == 15)
			e = 0.;
	}
};


// Height model and test
// Height class
class Height: public PeriodicModel {
public:
	InputPort<double> u;
	OutputPort<double> y;
	Height(ComposedModel *parent):
		PeriodicModel("Height", 1, parent),
		u(this, "u"),
		y(this, "y"),
		h(0) { }
protected:
	void init() override { h = 0; y = 0; }
	void update(date_t date) override { h += u; y = h; }
private:
	double h;
};


// World class
class World: public Simulate {
public:
	Constant<double> rule;
	Integ integ;
	Add3 add3;
	Height height;
	Report report;

	World():
		Simulate("PID", 25),
		rule(100., this),
		integ(this),
		add3(this),
		height(this),
		report("report", this)
	{
		report.add(add3.s);
	}
};


PHYSIM_RUN(Add3Test)
//PHYSIM_RUN(IntegTest)
//PHYSIM_RUN(World)
