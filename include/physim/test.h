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
	ReactiveTest(string name): ComposedModel(name), _sim(nullptr), _tracing(false), _error_cnt(0) { }

	void run() {
		_sim = new Simulation(*this);
		_sim->setTracing(_tracing);
		err() << "launching " << name() << endl;
		_error_cnt = 0;
		test();
		if(_error_cnt == 0)
			err() << "Success !" << endl;
		else
			err() << "Failed: " << _error_cnt << " error(s) found!" << endl;
		delete _sim;
		_sim = nullptr;
	}

	virtual void test() = 0;

	inline void setTracing(bool t) { _tracing = t; }

protected:
	void step() { _sim->step(); }
	template <class T, int N>
	inline void check(InputPort<T, N>& x, int ex, int i = 0) {
		if(x[i] != ex) { err() << "failed: expected " << ex << ", got " << x << endl; _error_cnt++; }
	}
private:
	Simulation *_sim;
	bool _tracing;
	int _error_cnt;
};

}	// physim

#endif /* INCLUDE_PHYSIM_TEST_H_ */
