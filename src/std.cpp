/*
 * std.cpp
 *
 *  Created on: 14 nov. 2020
 *      Author: casse
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

