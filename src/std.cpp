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

#include <physim/std.h>

namespace physim {

/**
 * @class Constant
 * Model delivering a constant value.
 *
 * This model is made of an output port always delivering the sale constant
 * value.
 *
 * @param T		Type of value to deliver.
 */

/**
 * @fn Constant::Constant(ComposedModel *parent, const T& y_);
 * Constructor of a constant model.
 * @param parent	Parent composed model.
 * @param y_		Value of the consyant.
 */

/**
 * @var Constant::InputPort<T, N> x;
 * Port delivering the constant value.
 */


/**
 * @class Display
 * Model displaying the value of an output port.
 *
 * This model can be connected to any port to display its values
 * while their are changing.
 *
 * @param T		Type of values in the port.
 * @param N		Number of values in the port (default to N).
 */

/**
 * @var InputPort<T, N> Display::x;
 * Port to connect the output to display.
 */

/**
 * @fn Display::Display(ComposedModel *parent, string name, ostream& out);
 * Constructor of a port displayer.
 * @param parent	Parent composed model.
 * @param name		Name of the model.
 * @param out		Output stream to perform the display (default to cout).
 */

} // physim

