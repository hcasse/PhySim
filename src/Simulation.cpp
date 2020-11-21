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
	_mon_alloc(false),
	_tracing(false),
	_state(STOPPED)
{
	_top.finalize(*this);
}

///
Simulation::~Simulation() {
	stop();
	if(_mon_alloc)
		delete _mon;
}

/**
 * Start the simulation.
 *
 */
void Simulation::start() {
	if(_state == STOPPED) {
		if(tracing())
			_mon->err() << "TRACE: starting the simulation." << endl;
		_date = 0;
		_top.start();
		if(tracing())
			_mon->err() << "TRACE: initializing the simulation." << endl;
		_state = RUNNING;
		_top.init();
		//_top.publish();	TODOs
		stabilize();
		if(tracing())
			_mon->err() << "TRACE: simulation paused." << endl;
		_state = PAUSED;
	}
}

/**
 * Run the function until the simulation is stopped.
 */
void Simulation::run() {
	start();
	_state = RUNNING;
	while(_state == RUNNING)
		step();
	if(_state == RUNNING)
		_state = PAUSED;
}

/**
 * Run the simulation for the given duration or until the simulation
 * is stopped.
 */
void Simulation::run(duration_t duration) {
	start();
	_state = RUNNING;
	if(_tracing)
		_mon->err() << "TRACE: simulation running." << endl;
	while(_state == RUNNING && duration != 0) {
		//cerr << "DEBUG: step " << _date << endl;
		advance();
		duration--;
	}
	if(_state == RUNNING) {
		_state = PAUSED;
		if(_tracing)
			_mon->err() << "TRACE: simulation paused." << endl;
	}
}

/**
 * Run the simulation until this date or until the simulation
 * is stopped.
 */
void Simulation::runUntil(date_t date) {
	start();
	_state = RUNNING;
	while(_state == RUNNING && _date < date)
		advance();
	if(_state == RUNNING)
		_state = PAUSED;
}

/**
 * Perform one step of simulation, that is, simulate until the simulation
 * is stopped, or it stays model to update.
 */
void Simulation::step() {
	_state = RUNNING;
	advance();
	if(_state == RUNNING)
		_state = PAUSED;
}

/*
 * Perform one step of simulation, that is, simulate until the simulation
 * is stopped, or it stays model to update.
 */
void Simulation::advance() {

	// select periodic models
	while(not _sched.empty() and _sched.top().at == _date) {
		//cerr << "DEBUG: pumping " << _sched.top().model->fullname() << " at " << _sched.top().at << endl;
		_pers.push_back(_sched.top().model);
		_sched.pop();
	}

	// update input of periodic models
	for(auto m: _pers) {
		for(auto p: _sched.top().model->ports())
			if(p->mode() == IN) {
				if(tracing())
					_mon->err() << "TRACE: " << _date << ": updating port " << p->fullname() << endl;
				p->update();
			}
	}

	// update periodic models
	for(auto m: _pers) {
		if(tracing())
			_mon->err() << "TRACE: " << _date << ": updating " << m->fullname() << endl;
		m->update();
	}

	// stabilize the simulation
	_pers.clear();
	stabilize();
	_date++;
}


/**
 * Stabilize the state of the simulation, that is, update the triggered
 * models until a fix point is reached or the simulation passes in state
 * STOPPED.
 */
void Simulation::stabilize() {

	// update triggered models
	while(not _todo.empty() and _state != STOPPED) {
		auto m = *_todo.begin();
		_todo.erase(m);
		if(tracing())
			_mon->err() << "TRACE: " << _date << ": updating " << m->fullname() << endl;
		m->update();
	}

	// trigger last models
	for(auto m: _coms) {
		if(tracing())
			_mon->err() << "TRACE: " << _date << ": committing " << m->fullname() << endl;
		m->commit();
	}
	_coms.clear();
}


/**
 * Ask to trigger the given model as soon as possible.
 * @param model	Model to trigger.s
 */
void Simulation::trigger(Model& model) {
	//_mon->err() << "DEBUG: state = " << _state << endl;
	//if(not isStopped())
	_todo.insert(&model);
	if(tracing())
		_mon->err() << "TRACE: " << date() << ": trigger " << model.fullname() << endl;
}

/**
 * Record the given to be commited.
 * @param model	Model to be commited.
 */
void Simulation::commit(Model& model) {
	_coms.insert(&model);
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
	else {
		_sched.push(Date(at, model));
		if(tracing())
			cerr << "DEBUG: " << _date << ": " << model.fullname() << " scheduled at " << at << endl;
	}
}

/**
 * @fn date_t Simulation::date() const;
 * Get the current date.
 * @return	Current date.
 */

/**
 * Pause the current simulation (if any).
 */
void Simulation::pause() {
	_state = PAUSED;
}

/**
 * Stop the current simulation.
 */
void Simulation::stop() {
	if(_state != STOPPED) {
		_state = STOPPED;
		_top.stop();
		_todo.clear();
		while(not _sched.empty())
			_sched.pop();
	}
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
