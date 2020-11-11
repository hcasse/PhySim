/*
 * AbstractModel.cpp
 *
 *  Created on: 5 nov. 2020
 *      Author: casse
 */

#include <physim.h>

namespace physim {

/**
 * @class AbstractModel
 * Abstract class to represent a model. Provides the main interface and
 * basics ressources.
 */

Model::Model(string name, ComposedModel *parent)
	: _name(name), _parent(parent), _sim(nullptr)
{
	if(_parent != nullptr)
		_parent->subs.push_back(this);
}


///
Model::~Model() {
}


/**
 * Called to update the model according to a propagation along
 * the given port. The default implementation does nothing.
 * @param port	Updated port.
 */
void Model::propagate(const AbstractPort& port) {
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
	_sim = nullptr;
}

/**
 * Function called to finalize the model network.
 * @param sim	Current simulator.
 */
void Model::finalize(Simulation& sim) {
	_sim = &sim;
	//err() << "DEBUG: finalize " << name() << endl;
	for(auto p: _ports) {
		p->finalize(sim.monitor());
		//err() << "DEBUG: finalize port " << p->fullname() << endl;
	}
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
 * @class ReactiveModel
 * A reactive model reacts immediately to any change on its input ports.
 * Just overload update() function to change its behavior.
 */

///
ReactiveModel::ReactiveModel(string name, ComposedModel *parent)
	: Model(name, parent) { }


/**
 * @fn void update();
 * Function to overload to compute the output port because of a change in the
 * input ports.
 */

/**
 *
 */
void ReactiveModel::propagate(const AbstractPort& port) {
	sim().trigger(*this);
}


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
	: Model(name, parent), _period(period) { }

/**
 * Construct a periodic model.
 * @param name		Model name.
 * @param parent	Parent model (optional).
 */
PeriodicModel::PeriodicModel(string name, ComposedModel *parent)
	: Model(name, parent), _period(1) { }

/**
 * @fn void PeriodicModel::update();
 * This function is called each time the model needs to be updated.
 * @param date	Current date.
 */

///
void PeriodicModel::start() {
	Model::start();
	sim().schedule(*this, date() + _period);
}

///
void PeriodicModel::propagate(const AbstractPort& port) {
}


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
void ComposedModel::propagate(const AbstractPort& port) {
	// TODO
}

///
void ComposedModel::init() {
	Model::init();
	for(auto m: subs)
		m->init();
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


} // physim



