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
#ifndef INCLUDE_PHYSIM_MODELS_H_
#define INCLUDE_PHYSIM_MODELS_H_

#include <physim/simulation.h>

namespace physim {

class AbstractPort;
class AbstractValue;
class ComposedModel;

class Model {
	friend class AbstractValue;
	friend AbstractPort;
	friend class Simulation;
	friend class ComposedModel;
public:
	Model(string name, ComposedModel *parent = nullptr);
	virtual ~Model();
	inline string name() const { return _name; }
	string fullname() const;
	inline Simulation& sim() const { return *_sim; }
	inline date_t date() const { return sim().date(); }
	inline bool simEnabled() const { return _sim != nullptr; }
	inline bool isSimulating() const { return _sim != nullptr and not _sim->isStopped(); }
	inline const vector<AbstractPort *>& ports() const { return _ports; }
	inline ComposedModel *parent() const { return _parent; }

	virtual bool isComposed() const { return false; }
	virtual void init();
	virtual void update();
	virtual void propagate(const AbstractPort& port);
	virtual void publish();

	void info(const string& msg) const;
	void warn(const string& msg) const;
	void error(const string& msg) const;
	void fatal(const string& msg) const;
	ostream& out() const;
	ostream& err() const;

protected:

	virtual void start();
	virtual void stop();
	virtual void finalize(Simulation& sim);

private:
	void add(AbstractValue *val);
	string _name;
	ComposedModel *_parent;
	Simulation *_sim;
	vector<AbstractPort *> _ports;
	vector<AbstractValue *> _vals;
	mutable string _full_name;
};

class ReactiveModel: public Model {
public:
	ReactiveModel(string name, ComposedModel *parent = nullptr);
protected:
	void propagate(const AbstractPort& port) override;
};

class PeriodicModel: public Model {
	friend class Simulation;
public:
	PeriodicModel(string name, duration_t period = 1, ComposedModel *parent = nullptr);
	PeriodicModel(string name, ComposedModel *parent = nullptr);
	inline duration_t period() const { return _period; }
protected:
	void start() override;
	void propagate(const AbstractPort& port) override;
	void update();
	virtual void update(date_t date) = 0;
	void publish() override;
private:
	duration_t _period;
};

}	// physim

#endif /* INCLUDE_PHYSIM_MODELS_H_ */