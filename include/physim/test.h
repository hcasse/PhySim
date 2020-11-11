/*
 * test.h
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
 */

#ifndef INCLUDE_PHYSIM_TEST_H_
#define INCLUDE_PHYSIM_TEST_H_

#include <physim.h>

namespace physim {

class ReactiveTest: public ComposedModel {
public:
	ReactiveTest(string name);
	int run();
	virtual void test() = 0;
	inline void setTracing(bool t) { _tracing = t; }
protected:
	void step();
	template <class T, int N>
	inline void check(InputPort<T, N>& x, int ex, int i = 0) {
		if(x[i] != ex) { err() << "failed: expected " << ex << ", got " << x << endl; _failed = true; }
	}
private:
	Simulation *_sim;
	bool _tracing, _failed;
	int _error_cnt;
};

class PeriodicTest: public ComposedModel {
public:
	PeriodicTest(string name, PeriodicModel& model, duration_t duration);
	int run();
	virtual void test(date_t date) = 0;
	inline void setTracing(int t) { _tracing = true; }

	template <class T, int N>
	inline void check(InputPort<T, N>& x, int ex, int i = 0) {
		if(x[i] != ex) { err() << "failed: " << (date() - 1) << ": " << x.fullname() << ": expected " << ex << ", got " << x << endl; _failed = true; }
	}

private:
	Simulation *_sim;
	PeriodicModel& _model;
	date_t _duration;
	bool _failed;
	bool _tracing;
};

}	// physim

#endif /* INCLUDE_PHYSIM_TEST_H_ */
