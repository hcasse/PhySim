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
 * @class AbstractValue
 * Class providing common features of valued entities of Physim:
 * ports, parameters and states.
 */

/**
 * Constructor.
 * @param parent	Parent model.
 * @param name		Value name.
 * @param type		Value type.
 * @param size		Value size (for arrays).
 * @param flavor	Value flavor.
 */
AbstractValue::AbstractValue(Model *parent, string name, Type& type, int size, flavor_t flavor)
	: _parent(parent), _name(name), _type(type), _size(size), _flavor(flavor)
{
	parent->add(this);
}

///
AbstractValue::~AbstractValue() {
}

/**
 * Parse the value from a textual representation.
 * The default implementation always fails.
 * @param text	Text to parse value from.
 * @return		True for success, false else.
 */
bool AbstractValue::parse(string text) {
	return false;
}

/**
 * Print the value in a human readable way.
 * The default implementation displays nothing.
 * @param out	Stream to output to.
 */
void AbstractValue::print(ostream& out) {

}

/**
 * Read the value from a file where it has been previously
 * written by a call to write(). The default implementation always
 * fails.
 */
bool AbstractValue::read(istream& in) {
	return false;
}

/**
 * Write the current value to a file. The default implementation
 * does nothing.
 * @param out	Stream to write to.
 */
void AbstractValue::write(ostream& out) {
}

/**
 * Initialize the abstract value. The default implementation does nothing.
 */
void AbstractValue::init() {
}


/**
 * @fn Model *AbstractValue::parent() const;
 * Get the parent model.
 * @return	Parent model.
 */

/**
 * @fn string AbstractValue::name() const;
 * Get the value name.
 * @return	Value name.
 */

/**
 * @fn Type& AbstractValue::type() const;
 * Get the type of the value.
 * @return	Value type.
 */

/**
 * @fn flavor_t AbstractValue::flavor() const;
 * Get the flavor of the value.
 * @return	Value flavor.
 */

/**
 * Get full-qualified name of the value, that is, its name prefixed
 * by the hierarchy of models containing it.
 * @return	Fully-qualified name.
 */
string AbstractValue::fullname() {
	if(_full_name == "")
		_full_name = parent()->fullname() + "." + name();
	return _full_name;
}


/**
 * @class Parameter
 * Class describing a value used as a parameter: it can only be read during
 * the simulation and only written when the simulation is stopped.
 *
 * @param T		Type of value in the parameter.
 * @param N		Number of values in the parameter.
 */

/**
 * @fn Parameter::Parameter(Model *parent, string name);
 * Constructor without the initialization of the parameter.
 * @param parent	Parent model.
 * @param name		Parameter name.
 */

/**
 * @fn Parameter::Parameter(Model *parent, string name, const T& x);
 * Constructor of parameter with only one initialization value. If the
 * parameter is an array, the value is copied on the whole array.
 * @param parent	Parent model.
 * @param name		Parameter name.
 * @param x			Parameter initial value.
 */

/**
 * @fn Parameter::Parameter(Model *parent, string name, const initializer_list<T>& l);
 * Constructor with an initialization list.
 * @param parent	Parent model.
 * @param name		Parameter name.
 * @param l			List of initial values for the parameter array.
 */

/**
 * @fn Parameter::operator const T&() const;
 * Get the parameter value (if not an array).
 * @return	Parameter value.
 */

/**
 * @fn const T& Parameter::operator*() const;
 * Get the parameter value (if not an array).
 * @return	Parameter value.
 */

/**
 * @fn const T& Parameter::operator[](int i) const;
 * Get the value at the given index in an array parameter.
 * @param i		Index of looked value.
 * @return		Corresponding value.
 */

/**
 * @fn void Parameter::set(const T& x);
 * Set the parameter value if the parameter is scalar, or initialize
 * the array with the given value if the parameter is an array.
 * This function does nothing if the simulation is running.
 * @param x		Initial value.
 */

/**
 * @fn void Parameter::set(const initializer_list<T>& l);
 * Set the parameter value for a array parameter.
 * @param l		List of initial values.
 */


/**
 * @class State
 * Class representing a state item of a model. A state can be viewed,
 * displayed, saved, loaded, etc. In addition, a state is able to
 * re-initialize itself.
 *
 * @param T		Type of value in the parameter.
 * @param N		Number of values in the parameter.
 */

/**
 * @fn State::State(Model *parent, string name);
 * Constructor without initialization.
 * @param parent	Parent model.
 * @param name		Parameter name.
 */

/**
 * @fn State::State(Model *parent, string name, const T& x);
 * Constructor with a single initial value. If the state is an array,
 * this value is copied all along the array.
 * @param parent	Parent model.
 * @param name		Parameter name.
 * @param x			Initial value.
 */

/**
 * @fn State::State(Model *parent, string name, const initializer_list<T>& l);
 * Constructor with a list of value for an array state.
 * @param parent	Parent model.
 * @param name		Parameter name.
 * @param l			Initialization list.
 */

/**
 * @fn State::operator const T&() const;
 * Get the value of the state (for a non-array state).
 * @return	State value.
 */

/**
 * @fn const T& State::operator*() const;
 * Get the value of the state (for a non-array state).
 * @return	State value.
 */

/**
 * @fn const T& State::operator[](int i) const;
 * Get the value at given index for an array state.
 * @param i		Index of the looked value.
 * @return		Value at given index.
 */

/**
 * @fn State::operator T&();
 * Get the reference to the state value (for a non-array state).
 * @return	State value reference.
 */

/**
 * @fn T& State::operator*();
 * Get the reference to the state value (for a non-array state).
 * @return	State value reference.
 */

/**
 * @fn T& State::operator[](int i);
 * Get the reference to a value at given index for an array state.
 * @param i		Index of the looked value reference.
 * @return		Value reference at given index.
 */

} // physim
