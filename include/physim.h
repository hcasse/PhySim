/*
 * physim.h
 *
 *  Created on: 5 nov. 2020
 *      Author: casse
 */
#ifndef INCLUDE_PHYSIM_H_
#define INCLUDE_PHYSIM_H_

#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace physim {

using namespace std;

class AbstractPort;
class ComposedModel;
class Model;
class Simulation;
template <class T, int N> class InputPort;
template <class T, int N> class OutputPort;

typedef unsigned long long date_t;
typedef unsigned long long duration_t;

typedef enum {
	IN,
	OUT
} mode_t;

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


class Type { };
template <class T> const Type&type_of() { static Type t; return t; }

class Model {
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
	inline const vector<AbstractPort *>& ports() const { return _ports; }

	virtual void init();
	virtual void update();
	virtual void propagate(const AbstractPort& port);

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
	string _name;
	ComposedModel *_parent;
	Simulation *_sim;
	vector<AbstractPort *> _ports;
	mutable string _full_name;
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
protected:
protected:
	T *t;
};

template <class T, int N = 1>
class OutputPort: public Port<T, N> {
	friend class ComposedModel;
	friend class InputPort<T, N>;
public:
	OutputPort(Model *parent, string name)
		: Port<T, N>(parent, name, OUT) { }
	~OutputPort()
		{ if(not Port<T, N>::isLinked() and Port<T, N>::t != nullptr) delete [] Port<T, N>::t; }

	class Accessor {
	public:
		inline Accessor(Port<T, N>& port, int index): p(port), i(index) { }
		inline operator const T&() { return p.get(i); }
		inline Accessor& operator=(const T& x)
			{ p.set(i, x); return *this; }
	private:
		Port<T, N>& p;
		int i;
	};

	inline OutputPort<T, N>& operator=(const T& x) { set(0, x); return *this; }
	inline Accessor operator*() { return Accessor(*this, 0); }
	inline Accessor operator[](int i) { return Accessor(*this, i); }

private:
	inline T *getBuffer()
		{ if(Port<T, N>::t == nullptr) Port<T, N>::t = new T[N]; return Port<T, N>::t; }
	inline const T& get(int i) const { return Port<T, N>::t[i]; }
	inline void set(int i, const T& x);
	vector<InputPort<T, N> *> _links;
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


class ReactiveModel: public Model {
public:
	ReactiveModel(string name, ComposedModel *parent = nullptr);
protected:
	void propagate(const AbstractPort& port) override;
};

class PeriodicModel: public Model {
public:
	PeriodicModel(string name, duration_t period = 1, ComposedModel *parent = nullptr);
	PeriodicModel(string name, ComposedModel *parent = nullptr);
	inline duration_t period() const { return _period; }
protected:
	void start() override;
	void propagate(const AbstractPort& port) override;
	void update();
	virtual void update(date_t data) = 0;
private:
	duration_t _period;
};

class ComposedModel: public Model {
	friend class Model;
public:
	ComposedModel(string name, ComposedModel *parent = nullptr);
protected:
	void init() override;
	void propagate(const AbstractPort& port) override;
	void start() override;
	void stop() override;
	void finalize(Simulation& sim) override;

	template <class T, int N> void connect(InputPort<T, N>& ip, OutputPort<T, N>& op)
		{ ip._back = &op; }
	template <class T, int N> void connect(InputPort<T, N>& ips, InputPort<T, N>& ipt)
		{ ipt._back = &ips; }
	template <class T, int N> void connect(OutputPort<T, N>& ops, OutputPort<T, N>& opt)
		{ opt._back = &ops; }
	template <class T, int N> void connect(OutputPort<T, N>& ops, InputPort<T, N>& ips)
		{ ips._back = &ops; }
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
			{ return at < d.at or (at == d.at && model < d.model); }
	};

	Model& _top;
	set<Model *> _todo;
	priority_queue<Date> _sched;
	date_t _date;
	Monitor *_mon;
	bool _mon_alloc, _tracing;
	state_t _state;
};

inline date_t Model::date() const { return sim().date(); }

template <class T, int N>
inline void OutputPort<T, N>::set(int i, const T& x) {
	if(Port<T, N>::t[i] != x) {
		Port<T, N>::t[i] = x;
		if(Port<T, N>::model().sim().tracing()) {
			Port<T, N>::model().err() << "TRACE: " << Port<T, N>::model().sim().date()
				<< ": port " << Port<T, N>::fullname();
			if(N != 1)
				Port<T, N>::model().err() <<  "[" << i << "]";
			Port<T, N>::model().err() << " receives " << x << endl;
		}
		for(auto p: _links) p->touch();
	}
}

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
