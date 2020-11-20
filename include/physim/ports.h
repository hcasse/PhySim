/*
 * ports.h
 *
 *  Created on: 20 nov. 2020
 *      Author: casse
 */

#ifndef INCLUDE_PHYSIM_PORTS_H_
#define INCLUDE_PHYSIM_PORTS_H_

#include <physim/type.h>
#include <physim/monitor.h>

namespace physim {

using namespace std;

class Model;
template <class T, int N> class InputPort;

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
		: Port<T, N>(parent, name, OUT), _updated(false) { }
	OutputPort(ComposedModel *parent, string name)
		: Port<T, N>(parent, name, OUT), _updated(false) { }
	OutputPort(PeriodicModel *parent, string name)
		: Port<T, N>(parent, name, OUT), _updated(true) { }

	class Accessor {
	public:
		inline Accessor(OutputPort<T, N>& port, int index): p(port), i(index) { }
		inline operator const T&() { return p.get(i); }
		inline Accessor& operator=(const T& x) { p.set(i, x); return *this; }
	private:
		OutputPort<T, N>& p;
		int i;
	};

	inline OutputPort<T, N>& operator=(const T& x) { set(0, x); return *this; }
	inline Accessor operator*() { return Accessor(*this, 0); }
	inline Accessor operator[](int i) { return Accessor(*this, i); }

	inline void commit() {
		if(_updated) {
			propagate();
			_updated = false;
		}
	}

	inline void propagate() { for(auto m: _trigger_list) Port<T, N>::model().sim().trigger(*m); }

private:
	inline void recordTrigger(Model *m) { _trigger_list.push_back(m); }
	inline T *buffer() { if(Port<T, N>::t == nullptr) Port<T, N>::t = new T[N]; return Port<T, N>::t; }

	inline const T& get(int i) const { return Port<T, N>::t[i]; }

	inline void set(int i, const T& x) {
		if(Port<T, N>::t[i] != x) {
			_updated = true;
			Port<T, N>::t[i] = x;
			if(Port<T, N>::model().sim().tracing()) {
				Port<T, N>::model().err() << "TRACE: " << Port<T, N>::model().sim().date()
					<< ": port " << Port<T, N>::fullname();
				if(N != 1)
					Port<T, N>::model().err() <<  "[" << i << "]";
				Port<T, N>::model().err() << " receives " << x << endl;
			}
		}
	}

	vector<Model *> _trigger_list;
	bool _updated;
};


template <class T, int N = 1>
class InputPort: public Port<T, N> {
public:
	InputPort(Model *parent, string name)
		: Port<T, N>(parent, name, IN), buf(nullptr) { }

	void update()
		{ for(int i = 0; i < N; i++) Port<T, N>::t[i] = buf[i]; }

private:
	void finalize(Monitor& mon) override {
		auto p = Port<T, N>::source();
		if(p == nullptr)
			Port<T, N>::model().error("input port " + Port<T, N>::fullname() + " is dangling!");
		else {
			Model *parent =  &Port<T, N>::model();
			auto op = static_cast<OutputPort<T, N> *>(p);
			auto buf = op->buffer();
			if(parent->isPeriodic())
				Port<T, N>::t = new T[N];
			else {
				Port<T, N>::t = buf;
				op->recordTrigger(parent);
			}
			if(Port<T, N>::model().sim().tracing())
				Port<T, N>::model().err() << Port<T, N>::fullname() << " connected to " << p->fullname() << endl;
		}
	}

	T *buf;
};

} // physim

#endif /* INCLUDE_PHYSIM_PORTS_H_ */
