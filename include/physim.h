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
#ifndef INCLUDE_PHYSIM_H_
#define INCLUDE_PHYSIM_H_

#include <initializer_list>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include <physim/type.h>

namespace physim {

using namespace std;

class AbstractPort;
class ComposedModel;
class Model;
class Simulation;
template <class T, int N> class InputPort;
template <class T, int N> class OutputPort;

class Monitor {
public:
	Monitor(ostream& out = cout, ostream& err = cerr);
	virtual ~Monitor();
	virtual void info(const string& msg) = 0;
	virtual void warn(const string& msg) = 0;
	virtual void error(const string& msg) = 0;
	virtual void fatal(const string& msg) = 0;
	inline ostream& out() const { return *_out; }
	inline ostream& err() const { return *_err; }
protected:
	Simulation *_sim;
private:
	ostream *_out, *_err;
};

class TerminalMonitor: public Monitor {
public:
	using Monitor::Monitor;
	void info(const string& msg) override;
	void warn(const string& msg) override;
	void error(const string& msg) override;
	void fatal(const string& msg) override;
};


class AbstractValue {
public:
	AbstractValue(Model *_parent, string name, Type& type, int size, flavor_t flavor);
	virtual ~AbstractValue();
	virtual bool parse(string text);
	virtual void print(ostream& out);
	virtual bool read(istream& in);
	virtual void write(ostream& out);
	virtual void init();

	inline Model *parent() const { return _parent; }
	inline string name() const { return _name; }
	inline Type& type() const { return _type; }
	inline flavor_t flavor() const { return _flavor; }
	inline int size() const { return _size; }
	string fullname();

private:
	Model *_parent;
	string _name;
	Type& _type;
	flavor_t _flavor;
	int _size;
	mutable string _full_name;
};

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
	inline date_t date() const;
	inline bool simEnabled() const { return _sim != nullptr; }
	inline bool isSimulating() const;
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

class AbstractPort {
	friend class Model;
	friend class ComposedModel;
public:
	AbstractPort(Model *model, string name, mode_t mode, const Type& type, int size);
	virtual ~AbstractPort();
	inline string name() const { return _name; }
	inline mode_t mode() const { return _mode; }
	inline const Type& type() const { return _type; }
	inline int size() const { return _size; }
	inline Model& model() const { return _model; }
	inline bool isLinked() const { return _back != nullptr; }
	AbstractPort *source();
	string fullname() const;
	virtual void publish();
	virtual bool supportsReal();
	virtual long double asReal(int i = 0);
protected:
	virtual void finalize(Monitor& mon);
private:
	string _name;
	mode_t _mode;
	const Type& _type;
	int _size;
	Model& _model;
	AbstractPort *_back;
	mutable string _full_name;
};

template <class T, int N>
class Port: public AbstractPort {
	friend class ComposedModel;
public:
	inline Port(Model *model, string name, mode_t mode)
		: AbstractPort(model, name, mode, type_of<T>(), N), t(nullptr) {}
	inline operator const T&() const { return t[0]; }
	inline const T& operator*() const { return t[0]; }
	inline const T& operator[](int i) const { return t[i]; }

	bool supportsReal() override { return supports_real<T>(); }
	long double asReal(int i = 0) override { return as_real(t[i]); }

protected:
	T *t;
};

template <class T, int N = 1>
class OutputPort: public Port<T, N> {
	friend class ComposedModel;
	friend class InputPort<T, N>;
public:
	OutputPort(Model *parent, string name)
		: Port<T, N>(parent, name, OUT), buf(new T[N]), _updated(false) { Port<T, N>::t = buf; }
	OutputPort(ComposedModel *parent, string name)
		: Port<T, N>(parent, name, OUT), buf(nullptr), _updated(false) { }
	OutputPort(PeriodicModel *parent, string name)
		: Port<T, N>(parent, name, OUT), buf(new T[N]), _updated(true) { Port<T, N>::t = new T[N]; }
	~OutputPort() {
		if(not Port<T, N>::isLinked()) {
			if(buf != Port<T, N>::t)
				delete [] Port<T, N>::t;
			if(buf != nullptr)
				delete [] buf;
		}
	}

	class Accessor {
	public:
		inline Accessor(OutputPort<T, N>& port, int index): p(port), i(index) { }
		inline operator const T&() { return p.get(i); }
		inline Accessor& operator=(const T& x)
			{ p.set(i, x); return *this; }
	private:
		OutputPort<T, N>& p;
		int i;
	};

	inline OutputPort<T, N>& operator=(const T& x) { set(0, x); return *this; }
	inline Accessor operator*() { return Accessor(*this, 0); }
	inline Accessor operator[](int i) { return Accessor(*this, i); }

	void publish() override {
		if(_updated) {
			for(int i = 0; i < N; i++)
				buf[i] = Port<T, N>::t[i];
			propagate();
			_updated = false;
		}
	}
	inline void propagate();

private:
	inline bool isDelayed() const { return buf != Port<T, N>::t; }
	inline T *getBuffer() { if(buf == nullptr) Port<T, N>::t = buf = new T[N]; return buf; }
	inline const T& get(int i) const { return Port<T, N>::t[i]; }

	inline void set(int i, const T& x) {
		if(Port<T, N>::t[i] != x) {
			Port<T, N>::t[i] = x;
			if(Port<T, N>::model().sim().tracing()) {
				Port<T, N>::model().err() << "TRACE: " << Port<T, N>::model().sim().date()
					<< ": port " << Port<T, N>::fullname();
				if(N != 1)
					Port<T, N>::model().err() <<  "[" << i << "]";
				Port<T, N>::model().err() << " receives " << x << endl;
			}
			if(isDelayed())
				_updated = true;
			else
				propagate();
		}
	}

	vector<InputPort<T, N> *> _links;
	T *buf;
	bool _updated;
};


template <class T, int N = 1>
class InputPort: public Port<T, N> {
public:
	InputPort(Model *parent, string name)
		: Port<T, N>(parent, name, IN), _needs_update(false) { }
	inline void touch() { _needs_update = true; AbstractPort::model().propagate(*this); }

private:
	void finalize(Monitor& mon) override {
		auto p = Port<T, N>::source();
		if(p == nullptr)
			Port<T, N>::model().error("input port " + Port<T, N>::fullname() + " is dangling!");
		else {
			auto op = static_cast<OutputPort<T, N> *>(p);
			Port<T, N>::t = op->getBuffer();
			op->_links.push_back(this);
			if(Port<T, N>::model().sim().tracing())
				Port<T, N>::model().err() << Port<T, N>::fullname() << " connected to " << op->Port<T, N>::fullname() << endl;
		}
	}

	bool _needs_update;
};


class ComposedModel: public Model {
	friend class Model;
public:
	ComposedModel(string name, ComposedModel *parent = nullptr);

	template <class T, int N> void connect(InputPort<T, N>& ip, OutputPort<T, N>& op)
		{ ip._back = &op; }
	template <class T, int N> void connect(InputPort<T, N>& ips, InputPort<T, N>& ipt)
		{ ipt._back = &ips; }
	template <class T, int N> void connect(OutputPort<T, N>& ops, OutputPort<T, N>& opt)
		{ opt._back = &ops; }
	template <class T, int N> void connect(OutputPort<T, N>& ops, InputPort<T, N>& ips)
		{ ips._back = &ops; }

	inline const vector<Model *>& subModels() const { return subs; }
	bool isComposed() const override { return true; }

protected:
	void init() override;
	void propagate(const AbstractPort& port) override;
	void start() override;
	void stop() override;
	void finalize(Simulation& sim) override;
	void publish() override;

private:
	vector<Model *> subs;
};

class Simulation {
public:
	Simulation(Model& top);
	Simulation(Model& top, Monitor& mon);
	~Simulation();
	void run();
	void run(duration_t duration);
	void runUntil(date_t date);
	void start();
	void step();
	void pause();
	void stop();
	void trigger(Model& model);
	void triggerLast(Model& model);
	void schedule(Model& model, date_t at);
	inline date_t date() const { return _date; }

	inline Model& top() const { return _top; }
	inline Monitor& monitor() const { return *_mon; }
	inline bool tracing() const { return _tracing; }
	inline void setTracing(bool t) { _tracing = t; }
	inline bool isStopped() const { return _state == STOPPED; }
	inline bool isRunning() const { return _state == STOPPED; }
	inline bool isPaused() const { return _state == STOPPED; }

private:

	void advance();

	typedef enum {
		STOPPED,
		PAUSED,
		RUNNING
	} state_t;

	class Date {
	public:
		inline Date(date_t _at, Model& _model): at(_at), model(&_model) { }
		date_t at;
		Model *model;
		inline bool operator==(const Date& d) const
			{ return at == d.at and  model == d.model; }
		inline bool operator<(const Date& d) const
			{ return at > d.at or (at == d.at && model < d.model); }
	};

	Model& _top;
	set<Model *> _todo;
	set<Model *> _last;
	priority_queue<Date> _sched;
	vector<Model *> _pers;
	date_t _date;
	Monitor *_mon;
	bool _mon_alloc, _tracing;
	state_t _state;
};

inline date_t Model::date() const { return sim().date(); }
template <class T, int N>
inline void OutputPort<T, N>::propagate()
	{ for(auto p: _links) p->touch(); }
bool Model::isSimulating() const { return _sim != nullptr and not _sim->isStopped(); }

template <class T, int N>
class Parameter: public AbstractValue {
public:
	inline Parameter(Model *parent, string name)
		: AbstractValue(parent, name, type_of<T>(), N, PARAM) { }
	inline Parameter(Model *parent, string name, const T& x)
		: AbstractValue(parent, name, type_of<T>(), N, PARAM)
		{ set(x); }
	inline Parameter(Model *parent, string name, const initializer_list<T>& l)
		: AbstractValue(parent, name, type_of<T>(), N, PARAM)
		{ set(l); }
	inline operator const T&() const { return t[0]; }
	inline const T& operator*() const { return t[0]; }
	inline const T& operator[](int i) const { return t[i]; }

	inline void set(const T& x)
		{ if(not parent()->isSimulating()) for(int i = 0; i < N; i++) t[i] = x; }
	inline void set(const initializer_list<T>& l)
	{ if(not parent()->isSimulating()) { int i = 0; for(const auto& x: l) { t[i] = x; i++; } } }

private:
	T t[N];
};

template <class T, int N>
class State: public AbstractValue {
public:
	inline State(Model *parent, string name)
		: AbstractValue(parent, name, type_of<T>(), N, PARAM) { }
	inline State(Model *parent, string name, const T& x)
		: AbstractValue(parent, name, type_of<T>(), N, PARAM)
		{ for(int i = 0; i < N; i++) it[i] = x; }
	inline State(Model *parent, string name, const initializer_list<T>& l)
		: AbstractValue(parent, name, type_of<T>(), N, PARAM)
		{ auto i = 0; for(const auto& x: l) { it[i] = x; i++; } }

	inline operator const T&() const { return t[0]; }
	inline const T& operator*() const { return t[0]; }
	inline const T& operator[](int i) const { return t[i]; }

	inline operator T&() { return t[0]; }
	inline T& operator*() { return t[0]; }
	inline T& operator[](int i) { return t[i]; }

	virtual void init() { for(int i = 0; i < N; i++) t[i] = it[i]; }

private:
	T t[N], it[N];
};


class ApplicationModel: public ComposedModel {
public:
	ApplicationModel(string name);

	int run(int argc = 1, char **argv = nullptr);
	inline void setTracing(bool t) { _tracing = t; }

protected:
	virtual int perform() = 0;
	virtual int parseOption(int& i, int argc, char **argv);
	virtual void dumpOptions();
	void errorOption(const string& msg);
	inline Simulation& sim() const { return *_sim; }

private:
	Simulation *_sim;
	bool _tracing;
};


class Simulate: public ApplicationModel {
public:
	Simulate(string name, duration_t d = 10);
protected:
	int perform() override;
	int parseOption(int& i, int argc, char **argv) override;
	void dumpOptions() override;
private:
	duration_t _d;
};

#define PHYSIM_RUN(C) int main(int argc, char **argv) { return C().run(argc, argv); }

}	// physim

#endif /* INCLUDE_PHYSIM_H_ */
