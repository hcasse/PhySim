/*
 * reactive.cpp
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

#include <physim.h>
#include <physim/std.h>
using namespace physim;

class Ping: public PeriodicModel {
public:
	InputPort<int> x;
	OutputPort<int> y;

	Ping(string name, duration_t period, ComposedModel *parent):
		PeriodicModel(name, period, parent),
		x(this, "x"),
		y(this, "y"),
		ball(1)
	{ }
protected:

	void init() override { y = ball; }
	void update(date_t at) override { ball = x; y = ball; }
private:
	int ball;
};

class Pong: public PeriodicModel {
public:
	InputPort<int> x;
	OutputPort<int> y;

	Pong(string name, duration_t period, ComposedModel *parent):
		PeriodicModel(name, period, parent),
		x(this, "x"),
		y(this, "y"),
		ball(0)
	{ }
protected:

	void init() override { y = ball; }
	void update(date_t at) override { ball = x; y = ball; }
private:
	int ball;
};


class PingPongSim: public Simulate {
public:
	Ping ping;
	Pong pong;
	Report report;

	PingPongSim():
		Simulate("simple-periodic-test"),
		ping("ping", 2, this),
		pong("pong", 2, this),
		report("report", this)
	{
		connect(ping.y, pong.x);
		connect(pong.y, ping.x);
		report.add(ping.y);
		report.add(pong.y);
	}

};

PHYSIM_RUN(PingPongSim)




