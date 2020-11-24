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
 * @class AbstractModel
 * Abstract class to represent a model. Provides the main interface and
 * basics ressources.
 */

Model::Model(string name, ComposedModel *parent)
	: _name(name), _parent(parent), _sim(nullptr), flags(0)
{
	if(_parent != nullptr)
		_parent->subs.push_back(this);
}


///
Model::~Model() {
}


/**
 * Function called to update the model. The default implementation does nothing.
 */
void Model::update() {
}


/**
 * Function called by the simulator just before starting
 * to let the system initialize itself. The default implementation does nothing.
 */
void Model::init() {
	for(auto s: states())
		s->init();
}


/**
 * Called when the simulation is started.
 * @param sim	Current simulator.
 */
void Model::start() {
}


/**
 * Called when the simulation is stopped.
 * @param sim	Current simulator.
 */
void Model::stop() {
}

/**
 * Function called to finalize the model network.
 * @param sim	Current simulator.
 */
void Model::finalize(Simulation& sim) {
	_sim = &sim;
	for(auto p: _ins)
		p->finalize(sim.monitor());
	for(auto p: _outs)
		p->finalize(sim.monitor());
}


/**
 * Add an abstract value.
 * @param val	Added value.
 */
void Model::add(AbstractValue *val) {
	if(val->flavor() == STATE) {
		_states.push_back(val);
		flags |= HAS_STATE;
	}
	else
		_params.push_back(val);
}


/**
 * Get the dot-separated full name of the model.
 * @return 	Full name.
 */
string Model::fullname() const {
	if(_full_name == "") {
		if(_parent == nullptr)
			_full_name = _name;
		else
			_full_name = _parent->fullname() + "." + _name;
	}
	return _full_name;
}

/**
 * @fn date_t Model::date() const;
 * Get the current simulation date.
 * @return	Current date.
 */

/**
 * Display information to the user.
 * @param msg	Message to display.
 */
void Model::info(const string& msg) const {
	sim().monitor().info(msg);
}

/**
 * Display warning to the user.
 * @param msg	Message to display.
 */
void Model::warn(const string& msg) const {
	sim().monitor().warn(msg);
}

/**
 * Display error to the user.
 * @param msg	Message to display.
 */
void Model::error(const string& msg) const {
	sim().monitor().error(msg);
}

/**
 * Display error to the user and stop the simulation.
 * @param msg	Message to display.
 */
void Model::fatal(const string& msg) const {
	sim().monitor().fatal(msg);
}

/**
 * Get the standard output stream.
 * @return	Standard output stream.
 */
ostream& Model::out() const {
	return sim().monitor().out();
}

/**
 * Get the standard error stream.
 * @return	Standard error stream.
 */
ostream& Model::err() const {
	return sim().monitor().err();
}

/**
 * Function called when a model has been commited in the simulation.
 * This function is called at the end of a simulation cycle to let
 * commit the state variables inside the model. This is the main
 * work of this default implementation.
 */
void Model::commit() {
	for(auto s: _states) {
		s->commit();
	}
}


/**
 * @class ReactiveModel
 * A reactive model reacts immediately to any change on its input ports.
 * Just overload update() function to change its behavior.
 */

///
ReactiveModel::ReactiveModel(string name, ComposedModel *parent)
	: Model(name, parent) { }


/**
 * @class class PeriodicModel
 * A model that is triggered periodically.
 */

/**
 * Construct a periodic model.
 * @param name		Model name.
 * @param period	Period of update (default to 1).
 * @param parent	Parent model (optional).
 */
PeriodicModel::PeriodicModel(string name, duration_t period, ComposedModel *parent)
	: Model(name, parent), _period(period)
{
	flags |= IS_PERIODIC;
	flags &= ~HAS_STATE;
}

/**
 * Construct a periodic model.
 * @param name		Model name.
 * @param parent	Parent model (optional).
 */
PeriodicModel::PeriodicModel(string name, ComposedModel *parent)
	: Model(name, parent), _period(1)
{
	flags |= IS_PERIODIC;
	flags &= ~HAS_STATE;
}

/**
 * @fn void PeriodicModel::update();
 * This function is called each time the model needs to be updated.
 * @param date	Current date.
 */

///
void PeriodicModel::start() {
	Model::start();
	sim().schedule(*this, date());
}

///
void PeriodicModel::update() {
	update(date());
	sim().schedule(*this, date() + _period);
}


/**
 * @class ComposedMode
 * A composed model is made of other models but is never activated by
 * itself. Yet, it provides input and output ports and ensures
 * propagation of changes to sub-models.
 */

/**
 * Build a composed model.
 * @param name		Name of the model.
 * @param parent	Parent of the model (if any).
 */
ComposedModel::ComposedModel(string name, ComposedModel *parent)
	: Model(name, parent) { }

///
void ComposedModel::init() {
	Model::init();
	if(sim().tracing())
		err() << "TRACE: " << date() << ": init " << fullname() << endl;
	for(auto m: subs) {
		if(sim().tracing())
			err() << "TRACE: " << date() << ": init " << m->fullname() << endl;
		m->init();
	}
}

///
void ComposedModel::start() {
	Model::start();
	for(auto m: subs)
		m->start();
}

///
void ComposedModel::stop() {
	for(auto m: subs)
		m->stop();
	Model::stop();
}

///
void ComposedModel::finalize(Simulation& sim) {
	for(auto m: subs)
		m->finalize(sim);
	Model::finalize(sim);
}


/**
 * @class ApplicationModel
 * Special composed model that may be run as an application.
 *
 * The following function may be specialized:
 * * @ref perform() to provide the actual work.
 * * @ref parseOption() to customize the options passed to the application.
 * * @ref dumpOptions() to display possible argument options.
 *
 * An easy to use an ApplicationModel is with the macro PHYSIM_RUN. Let MyModel
 * a model extending ApplicationModel, the following macro call replaces the
 * main() function:
 * @code
 * PHYSIM_RUN(MyModel);
 * @endcode
 */

/**
 * Constructor.
 * @param name	Name of the application.
 */
ApplicationModel::ApplicationModel(string name)
	: ComposedModel(name), _sim(nullptr), _tracing(false)
	{ }

/**
 * Run the application: manage the parameters, prepare a simulator and
 * invoke the @ref run() method.
 */
int ApplicationModel::run(int argc, char **argv) {

	// parse the arguments
	for(int i = 1; i < argc; i++) {
		auto r = parseOption(i, argc, argv);
		if(r < 0)
			return 0;
		else if(r > 0)
			return r;
	}

	// prepare the simulation
	_sim = new Simulation(*this);
	_sim->setTracing(_tracing);

	// perform the simulation
	_sim->start();
	auto r = perform();
	_sim->stop();

	// finalize the simulation
	delete _sim;
	_sim = nullptr;
	return r;
}

/**
 * @fn int ApplicationModel::perform();
 * Perform the simulation action.
 * @return	0 for success, non-zero for error (as returned to the OS).
 */


/**
 * Parse the given option.
 * @param i		Current option index (increment it if the option requires an argument).
 * @param argc	Argument count.
 * @parma argv	Argument values.
 * @return		0 for success, non-zero for failure (as returned to the OS).
 */
int ApplicationModel::parseOption(int& i, int argc, char **argv) {
	auto opt = string(argv[i]);
	if(opt == "-h" or opt == "--help") {
		dumpOptions();
		return -1;
	}
	else if(opt == "--tracing")
		_tracing = true;
	else {
		errorOption("unknown option '" + opt + "'!");
		return 1;
	}
	return 0;
}

/**
 * Dump the option message.
 */
void ApplicationModel::dumpOptions() {
	cerr << "Model simulation: " << name() << " [OPTIONS]" << endl << endl;
	cerr << "OPTIONS includes:" << endl;
	cerr << "-h, --help  display this message." << endl;
	cerr << "--tracing   enable internal work tracing" << endl;
}

/**
 * Display an option error.
 * @param msg	Message to display.
 */
void ApplicationModel::errorOption(const string& msg) {
	dumpOptions();
	cerr << endl << "ERROR: " << msg << endl;
}

/**
 * @fn Simulation& ApplicationModel::sim() const;
 * Get the current simulation.
 * @return	Current simulation.
 */


/**
 * @class Simulate;
 * An application model that run a simulation during some duration.
 *
 * It provides the following additional options:
 * @param -d, --duration INT -- specify the duration in time unit.
 */

/**
 * Build a simulate model application.
 * @param name
 */
Simulate::Simulate(string name, duration_t d)
	: ApplicationModel(name), _d(d) { }

///
int Simulate::perform() {
	sim().run(_d);
	return 0;
}

///
int Simulate::parseOption(int& i, int argc, char **argv) {
	string arg = argv[i];
	if(arg == "-d" || arg == "--duration") {
		i++;
		if(i == argc) {
			errorOption("-d or --duration requires an INT argument!");
			return 1;
		}
		try {
			_d = stoi(argv[i]);
			return 0;
		}
		catch(invalid_argument&) {
			errorOption("invalid duration: " + string(argv[i]));
			return 1;
		}
	}
	else
		return ApplicationModel::parseOption(i, argc, argv);
}

///
void Simulate::dumpOptions() {
	ApplicationModel::dumpOptions();
	cerr << "-d, --duration INT  perform during the given time (default " << _d << ")" << endl;
}

} // physim
