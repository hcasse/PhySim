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

class ReactiveTest: public ApplicationModel {
public:
	ReactiveTest(string name);
	virtual void test() = 0;
protected:
	int perform() override ;
	void step();
	template <class T, int N>
	inline void check(InputPort<T, N>& x, int ex, int i = 0) {
		if(x[i] != ex) { err() << "failed: expected " << ex << ", got " << x << endl; _failed = true; }
	}
private:
	bool _failed;
	int _error_cnt;
};

class PeriodicTest: public ApplicationModel {
public:
	PeriodicTest(string name, PeriodicModel& model, duration_t duration);

	template <class T, int N>
	inline void check(InputPort<T, N>& x, int ex, int i = 0) {
		if(x[i] != ex) { err() << "failed: " << (date() - 1) << ": " << x.fullname() << ": expected " << ex << ", got " << x << endl; _failed = true; }
	}

protected:
	int perform() override;
	virtual void test(date_t date) = 0;

private:
	PeriodicModel& _model;
	date_t _duration;
	bool _failed;
};

}	// physim

#endif /* INCLUDE_PHYSIM_TEST_H_ */
