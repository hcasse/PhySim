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
	friend class AbstractPort;
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
	inline const vector<AbstractPort *>& inputs() const { return _ins; }
	inline const vector<AbstractPort *>& outputs() const { return _outs; }
	inline const vector<AbstractValue *>& params() const { return _params; }
	inline const vector<AbstractValue *>& states() const { return _states; }
	inline ComposedModel *parent() const { return _parent; }

	virtual bool isComposed() const { return false; }
	virtual void init();
	virtual void update();
	virtual void commit();

	void info(const string& msg) const;
	void warn(const string& msg) const;
	void error(const string& msg) const;
	void fatal(const string& msg) const;
	ostream& out() const;
	ostream& err() const;

	inline bool hasState() const { return (flags & HAS_STATE) != 0; }
	inline bool isPeriodic() const { return (flags & IS_PERIODIC) != 0; }

protected:

	virtual void start();
	virtual void stop();
	virtual void finalize(Simulation& sim);

	static const int
		HAS_STATE = 0x01,
		IS_PERIODIC = 0x02;
	int flags;

private:
	void add(AbstractValue *val);
	string _name;
	ComposedModel *_parent;
	Simulation *_sim;
	vector<AbstractPort *> _ins;
	vector<AbstractPort *> _outs;
	vector<AbstractValue *> _params;
	vector<AbstractValue *> _states;
	mutable string _full_name;
};

class ReactiveModel: public Model {
public:
	ReactiveModel(string name, ComposedModel *parent = nullptr);
};

class PeriodicModel: public Model {
	friend class Simulation;
public:
	PeriodicModel(string name, duration_t period = 1, ComposedModel *parent = nullptr);
	PeriodicModel(string name, ComposedModel *parent = nullptr);
	inline duration_t period() const { return _period; }
protected:
	void start() override;
	void update();
	virtual void update(date_t date) = 0;
private:
	duration_t _period;
};

}	// physim

#endif /* INCLUDE_PHYSIM_MODELS_H_ */
