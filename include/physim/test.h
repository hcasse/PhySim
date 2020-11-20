/*
 * PhySim library -- DEVS for physics
 * Copyright (C) 2020  Hugues Cassé <hug.casse@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
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
	inline void check(InputPort<T, N>& x, T ex, int i = 0) {
		if(x[i] != ex) { err() << "failed: expected " << ex << ", got " << x << endl; _failed = true; }
	}

	template <class T, int N>
	inline void checkApprox(InputPort<T, N>& x, T ex, int i = 0, long double prec = 1e-5) {
		if(fabs(x[i] - ex) > prec) { err() << "failed: " << (date() - 1) << ": " << x.fullname() << ": expected " << ex << ", got " << x << endl; _failed = true; }
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

	template <class T, int N>
	inline void checkApprox(InputPort<T, N>& x, T ex, int i = 0, long double prec = 1e-5) {
		if(fabs(x[i] - ex) > prec) { err() << "failed: " << (date() - 1) << ": " << x.fullname() << ": expected " << ex << ", got " << x << endl; _failed = true; }
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
