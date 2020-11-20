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

#include <physim/test.h>

namespace physim {

/**
 * @class ReactiveTest
 * Test for reactive models.
 *
 * A test is build by (a) embedding the reactive model to test,
 * (b) declaring local input and output ports to connect to the tested
 * model ports and (c) by providing a test scenario in the function @ref test().
 *
 * The test scenario is made as a list of steps composed by:
 * * the setting of the tested model input (linked to test output ports),
 * * performing a step of simulation (call to @ref step()),
 * * finally checking the output ports of the tested model (linked to local
 * input ports) using function @ref check().
 *
 */

/**
 * Constructor.
 * @param name	Test name.
 */
ReactiveTest::ReactiveTest(string name)
	: ApplicationModel(name), _failed(false), _error_cnt(0)
	{ }

/**
 * Run the current test: this function prepares a simulation and call
 * the @ref test() function to perform the test.
 */
int ReactiveTest::perform() {
	_error_cnt = 0;
	_failed = false;
	err() << "launching " << name() << endl;
	test();
	if(_failed)
		_error_cnt++;
	if(_error_cnt == 0)
		err() << "Success !" << endl;
	else
		err() << "Failed: " << _error_cnt << " error(s) found!" << endl;
	return _error_cnt;
}

/**
 * @fn void ReactiveTest::test();
 * Function to overload to provide the actual implementation of the test.
 * It is called when the test must be performed.
 */

/**
 * @fn void ReactiveTest::setTracing(bool t);
 * Set the tracing mode of the simulator.
 * @param t	If true to set tracing to one, false for off.
 */

/**
 * Perform a step of simulation;
 */
void ReactiveTest::step() {
	if(_failed)
		_error_cnt++;
	sim().step();
	_failed = false;
}

/**
 * @fn void ReactiveTest::check(InputPort<T, N>& x, int ex, int i);
 * Check that the value of an input port is equal to the expected one.
 * If not, display and record the error.
 * @param x		Input port to check.
 * @param ex	Expacted value.
 * @param i		Index in the port of the value to test.
 */


/**
 * @class PeriodicTest
 * Class providing test for periodic models.
 * The function @ref test() is called at each end of period of
 * the periodic model. The output ports (connected to input port of the
 * test mode) is checked against some expected. As soon as one of these
 * check fails, the tests stops and issues an error.
 *
 * Otherwise the test simulation goes to the exhaustion of the test
 * duration and the test is considered as a success.
 */

/**
 * Build a periodic test.
 * @param name		Name of the test.
 * @param model		Periodic model to test.
 * @param duration	Duration of the test.
 */
PeriodicTest::PeriodicTest(string name, PeriodicModel& model, duration_t duration):
	ApplicationModel(name),
	_model(model),
	_duration(duration),
	_failed(false)
	{ }

/**
 * Perform the test.
 * @return	0 for success, non-0 else (can be passed as return of main function).
 */
int PeriodicTest::perform() {
	_failed = false;
	err() << "Launching " << name() << endl;
	while(not _failed and date() < _duration) {
		if(date() % _model.period() == 0)
			test(date());
		sim().step();
	}
	test(date());
	if(_failed)
		err() << "Failure at " << date() << endl;
	else
		err() << "Success!" << endl;
	return _failed;
}

/**
 * @fn void PeriodicTest::test(date_t date);
 * Function to overload to implement the test.
 * @param date	Current date.
 */

/**
 * @fn void PeriodicTest::setTracing(int t);
 * Set the tracing state option.
 * @param t		If true, tracing is on, the tracing is off else.
 */

/**
 * @fn void PeriodicTest::check(InputPort<T, N>& x, int ex, int i = 0);
 * Function to test the value of an input port against an expected value.
 * A failure in this check cause the failure of the whole test.
 * @param x		Input port to test.
 * @param ex	Expected value.
 * @param i		Value index if the port is an array (optional).
 */

}	// physim
