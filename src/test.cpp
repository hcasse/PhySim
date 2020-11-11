/*
 * test.h
 *
 *  Created on: 11 nov. 2020
 *      Author: casse
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
	: ComposedModel(name), _sim(nullptr), _tracing(false), _failed(false), _error_cnt(0)
	{ }

/**
 * Run the current test: this function prepares a simulation and call
 * the @ref test() function to perform the test.
 */
int ReactiveTest::run() {
	_sim = new Simulation(*this);
	_sim->setTracing(_tracing);
	err() << "launching " << name() << endl;
	_error_cnt = 0;
	_failed = false;
	test();
	if(_failed)
		_error_cnt++;
	if(_error_cnt == 0)
		err() << "Success !" << endl;
	else
		err() << "Failed: " << _error_cnt << " error(s) found!" << endl;
	delete _sim;
	_sim = nullptr;
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
	_sim->step();
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

}	// physim
