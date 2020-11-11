/*
 * Simulation.cpp
 *
 *  Created on: 10 nov. 2020
 *      Author: casse
 */

#include <physim.h>

namespace physim {

/**
 * @class Simulation
 * Class in charge of driving the simulation. It takes a top-level model and
 * provides different options to manage the simulation: running some time,
 * running until some date, running for ever, etc.
 */

/**
 * Constructor with default monitor.
 * @param top	Top-level model.
 */
Simulation::Simulation(Model& top):
	Simulation(top, *(new TerminalMonitor()))
{
	_mon_alloc = true;
}

/**
 * Constructor with a custom monitor.
 * @param top	Top-level model.
 * @param mon	Monitor to use.
 */
Simulation::Simulation(Model& top, Monitor& mon):
	_top(top),
	_date(0),
	_mon(new TerminalMonitor()),
	_running(false),
	_mon_alloc(false),
	_tracing(false)
{
	_top.finalize(*this);
}

///
Simulation::~Simulation() {
	if(_mon_alloc)
		delete _mon;
}

/**
 * Run the function until the simulation is stopped.
 */
void Simulation::run() {
	_running = true;
	_top.start();
	while(_running == true)
		step();
	_top.stop();
}

/**
 * Run the simulation for the given duration or until the simulation
 * is stopped.
 */
void Simulation::run(duration_t duration) {
	_running = true;
	_top.start();
	while(_running == true && duration != 0) {
		step();
		duration--;
	}
	_top.stop();
	_running = false;
}

/**
 * Run the simulation until this date or until the simulation
 * is stopped.
 */
void Simulation::runUntil(date_t date) {
	_running = true;
	_top.start();
	while(_running == true && _date < date)
		step();
	_top.stop();
	_running = false;
}

/*
 * Perform one step of simulation, that is, simulate until the simulation
 * is stopped, or it stays model to update.
 */
void Simulation::step() {

	// pump read dates
	while(not _sched.empty() and _sched.top().at == _date) {
		_todo.insert(_sched.top().model);
		_sched.pop();
	}

	// pump models that needs to be updated
	while(not _todo.empty()) {
		auto m = *_todo.begin();
		_todo.erase(m);
		_mon->err() << "TRACE: updating " << m->fullname() << endl;
		m->update();
	}

	// next date
	_date++;
}

/**
 * Ask to trigger the given model as soon as possible.
 * @param model	Model to trigger.s
 */
void Simulation::trigger(Model& model) {
	_todo.insert(&model);
}

/**
 * Schedule the trigger of the given model at the given date.
 * The at date must be bigger than the current date.
 * @param model		Model to trigger.
 * @param at		Date of trigger.
 */
void Simulation::schedule(Model& model, date_t at) {
	if(at <= _date)
		_mon->warn("model " + model.fullname() + " ask scheduling at date in the past: " + to_string(at));
	else
		_sched.push(Date(at, model));
}

/**
 * @fn date_t Simulation::date() const;
 * Get the current date.
 * @return	Current date.
 */

/**
 * Stop the current simulation (if any).
 */
void Simulation::stop() {
	_running = false;
}

/**
 * @fn Model& Simulation::top() const;
 * Get the top model.
 * @return	Top model.
 */

/**
 * @fn Monitor& Simulation::monitor() const;
 * Get the current monitor.
 * @return	Current monitor.
 */

/**
 * @fn bool Simulation::tracing() const;
 * Get the state of the tracing method.
 * @return	True if tracing is on, false else.
 */

/**
 * @fn void Simulation::setTracing(bool t);
 * Set the tracing option.
 * @param t	True for enabling tracing, false for disabling.
 */

} // physim
