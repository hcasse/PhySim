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

} // physim

#endif /* INCLUDE_PHYSIM_VALUES_H_ */
