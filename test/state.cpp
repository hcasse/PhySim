/*
 * reactive.cpp
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

#include <physim.h>
#include <physim/test.h>
using namespace physim;

class StateModel: public ReactiveModel {
public:
	InputPort<int> a, in;
	OutputPort<int> y, out;

	StateModel(ComposedModel *parent):
		ReactiveModel("state", parent),
		a(this, "a"),
		//b(this, "b"),
		y(this, "y"),
		s(this, "x", 0),
		in(this, "in"),
		out(this, "out")
	{ }
protected:

	void init() override {
		s = 0;
		y = s;
		out = 0;
	}

	void update() override {
		s = s + 1;
		y = s;
		if(in != date())
			out = date();
	}

private:
	State<int> s;
};

class StarterModel: public PeriodicModel {
public:
	OutputPort<int> y;
	StarterModel(ComposedModel *parent):
		PeriodicModel("starter", 2, parent),
		y(this, "y") { }

protected:
	void init() {
		y = 0;
	}
	void update(date_t date) {
		y = not y;
	}
};

class StateTest: public PeriodicTest {
public:
	StateModel state;
	StarterModel starter;
	InputPort<int> y;

	StateTest():
		PeriodicTest("simple-periodic-test", starter, 10),
		state(this),
		starter(this),
		y(this, "y")
	{
		connect(starter.y, state.a);
		connect(state.y, y);
		connect(state.out, state.in);
	}

	void test(date_t date) override {
		err() << "INFO: date = " << date << ", y  = " << y << endl;
		check(y, date / 2);
	}
};

PHYSIM_RUN(StateTest)
