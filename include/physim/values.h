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
#ifndef INCLUDE_PHYSIM_VALUES_H_
#define INCLUDE_PHYSIM_VALUES_H_

#include <string>
#include <iostream>
#include <physim/models.h>

namespace physim {

using namespace std;

class Model;

class AbstractValue {
public:
	AbstractValue(Model *_parent, string name, const Type& type, int size, flavor_t flavor);
	virtual ~AbstractValue();
	virtual bool parse(string text);
	virtual void print(ostream& out);
	virtual bool read(istream& in);
	virtual void write(ostream& out);
	virtual void init();
	virtual void commit();
	virtual void restore();

	inline Model *parent() const { return _parent; }
	inline string name() const { return _name; }
	inline const Type& type() const { return _type; }
	inline flavor_t flavor() const { return _flavor; }
	inline int size() const { return _size; }
	string fullname();

private:
	Model *_parent;
	string _name;
	const Type& _type;
	flavor_t _flavor;
	int _size;
	mutable string _full_name;
};

template <class T, int N = 1>
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

template <class T, int N = 1>
class State: public AbstractValue {
public:
	State(Model *parent, string name)
		: AbstractValue(parent, name, type_of<T>(), N, STATE), st(nullptr), _changed(false)
		{ complete(); }
	State(Model *parent, string name, const T& x)
		: AbstractValue(parent, name, type_of<T>(), N, STATE), st(nullptr), _changed(false)
		{ for(int i = 0; i < N; i++) it[i] = x; complete(); }
	State(Model *parent, string name, const initializer_list<T>& l)
		: AbstractValue(parent, name, type_of<T>(), N, STATE), st(nullptr), _changed(false)
		{ auto i = 0; for(const auto& x: l) { it[i] = x; i++; } complete(); }
	~State() { if(st != nullptr) delete [] st; }

	inline operator const T&() const { return t[0]; }
	inline const T& operator*() const { return t[0]; }
	inline const T& operator[](int i) const { return t[i]; }

	class Access {
	public:
		inline Access(State<T, N>& s, int i): _s(s), _i(i) { }
		inline operator const T&() const { return _s.get(_i); }
		inline Access& operator=(const T& x) { _s.set(_i, x); return *this; }
	private:
		State<T, N>& _s;
		int _i;
	};

	inline Access operator*() { return Access(*this, 0); }
	inline Access operator[](int i) { return Access(*this, i); }
	inline State<T, N>& operator=(const T& x) { set(0, x); return *this; }

	void init() override { for(int i = 0; i < N; i++) t[i] = it[i]; }
	void commit() override { _changed = false; }
	void restore() override { if(_changed) { for(int i = 0; i < N; i++) t[i] = st[i]; _changed = false; } }

private:

	const T& get(int i) const { return t[i]; }
	void set(int i, const T& x) {
		if(st != nullptr and not _changed)
			save();
		t[i] = x;
	}
	void save() {
		for(int i = 0; i < N; i++) st[i] = t[i];
		_changed = true;
		auto m = parent();
		m->sim().commit(*m);
	}

	void complete()
		{ if(not parent()->isPeriodic()) st = new T[N]; }

	T t[N], it[N];
	T *st;
	bool _changed;
};

} // physim

#endif /* INCLUDE_PHYSIM_VALUES_H_ */
