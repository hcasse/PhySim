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
 * @class AbstractPort
 * Abstract class for ports, gathering all non-generic definition of ports.
 */

/**
 * Constructor of an abstract port.
 * @param model		Container model.
 * @param name		Port name.
 * @param mode		Port mode (input, output).
 * @param type		Port type (result of @ref type_of<T>()).
 * @param size		Size of the port.
 */
AbstractPort::AbstractPort(Model *model, string name, mode_t mode, const Type& type, int size)
	: _model(*model), _name(name), _mode(mode), _type(type), _size(size), _back(nullptr)
{
	model->_ports.push_back(this);
}

///
AbstractPort::~AbstractPort() {
}

/**
 * This function is called on output ports connected on periodic models
 * to publish an internal update to connected models. The default
 * implementation does nothing.
 */
void AbstractPort::publish() {
	cerr << "DEBUG: default publish for " << fullname() << endl;
}

/**
 * Test if the port supports the expression of its content as real.
 * Default implementation returns false.
 * @return	True if the port data can be expressed as real value, false.
 */
bool AbstractPort::supportsReal() {
	return false;
}

/**
 * Get the port indexed by i as a real value (if possible).
 * @param i		Index of the accessed value (default 0).
 * @return		Value of the port (corresponding at the given index for array port).
 */
long double AbstractPort::asReal(int i) {
	return 0;
}


/**
 * @fn string AbstractPort::AbstractPort::name() const;
 * Get the name of the port.
 * @return Port name.
 */

/**
 * @fn mode_t AbstractPort::AbstractPort::mode() const
 * Get the mode of the port.
 * @return Port mode.
 */

/**
 * @fn const Type& AbstractPort::AbstractPort::type() const;
 * Get the type of the port.
 * @return Port type.
 */

/**
 * @fn int AbstractPort::AbstractPort::size() const;
 * Get the size of the port.
 * @return Port size.
 */

/**
 * @fn const Model& AbstractPort::AbstractPort::model() const;
 * Get the containder model of the port.
 * @return	Container model.
 */

/**
 * Get the source of the port.
 * @return	Source port or null if the port is itself an unlinked output
 * or if it is not linked input.
 */
AbstractPort *AbstractPort::source() {
	if(_back == nullptr)
		return nullptr;
	else {
		AbstractPort * p = this;
		while(p->_back != nullptr)
			p = p->_back;
		return p;
	}
}


/**
 * Get the full name of the port.
 */
string AbstractPort::fullname() const {
	if(_full_name == "")
		_full_name = model().fullname() + "." + name();
	return _full_name;
}

/**
 * This function is called by the simulator before starting the simulation
 * to ensure that the model network is completed: mainly that each input
 * port has found its output port provider of values.
 * @param mon	Current monitor.
 */
void AbstractPort::finalize(Monitor& mon) {
}


/**
 * @class Port
 * Class representing a port providing data. This is the templated version
 * of AbstractPort.
 * @param T	Type of values in the port.
 * @param N	Size of the value in the port (default to 1).s
 */

/**
 * @fn Port::Port(const Model *model, string name, mode_t mode);
 * Port constructor.
 * @param model	Container model.
 * @param name	Port name.
 * @param mode	Port mode (input, output).
 */

/**
 * @fn Port::operator const T&() const;
 * Get the value of the port (when N = 1).
 * @return	Port value.
 */

/**
 * @fn const T& Port::operator*() const;
 * Get the value of the port (when N = 1).
 * @return	Port value.
 */

/**
 * @fn const T& Port::operator[](int i) const;
 * Get the value of the port of index i.
 * @param i		Looked index.
 * @return		Value at index i.
 */

/**
 * @class OutputPort
 * Class representing an output port. In the opposite to InputPort,
 * an output port can be written.
 *
 * Output are usually written using assignment or indexes if the port size is
 * bigger than one.
 * @code
 * OutputPort<int> ip;
 * ip = 666;
 * OutputPort<float, 2> tp;
 * tp[0] = 0.1;
 * tp[1] = 1e3;
 * @endcode
 *
 * @param T	Type of values in the port.
 * @param N	Size of the value in the port (default to 1).s
 */

/**
 * @fn OutputPort::OutputPort(const Model *parent, string name);
 * Constructor.
 * @param model	Container model.
 * @param name	Port name.
 */

/**
 * @fn OutputPort<T, N>& OutputPort::operator=(const T& x);
 * Assign the value x to the port (if it is of size 1).
 * @param x	Assigned value.
 * @return	Port itself (to chain assignments).
 */

/**
 * @fn Accessor OutputPort::operator*();
 * Get an assignable reference to the port value (if it is of size 1).
 * @return	Assignable reference.
 */

/**
 * @fn Accessor OutputPort::operator[](int i);
 * Get an assignable reference to a port value by its index.
 * @param i		Index of the value to assign.
 * @return		Reference to the indexed value.
 */

/**
 * @class InputPort;
 * Class representing an input port. Input port values can only be read.
 *
 * @code
 * InputPort<int> ip;
 * int x = ip;
 * InputPort<float, 3> tp;
 * cout << tp[0] << ", " << tp[1] << ", " << tp[3] << endl;
 * @encode
 *
 * @param T	Type of values in the port.
 * @param N	Size of the value in the port (default to 1).s
 */

/**
 * @fn InputPort::InputPort(const Model *parent, string name);
 * Constructor.
 * @param model	Container model.
 * @param name	Port name.
 */

/**
 * @fn void InputPort::touch();
 * Inform the port that the corresponding OutputPort has been updated.
 * This function is used to wake up a possible reactive model containing
 * the port.
 */

} // physim
