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

#include <fstream>
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


/**
 * @class Report: public ReactiveModel
 * Generator of report.
 *
 * This class can connect several output port and to report the port values
 * changes along time.
 *
 * Its output may be performed on any stream or to
 * a file and follows the CSV format. The first line contains the name of
 * the columns and the first column is the date following by the value of
 * each port connected to the report.
 */

/**
 * Constructor of a report to a stream or to cout.
 * @param name		Report name.
 * @param parent	Parent model.
 * @param out		Output stream (default to cout).
 */
Report::Report(string name, ComposedModel *parent, ostream& out):
	Model(name, parent), _out(&out) { }

/**
 * Constructor of a report to a named file.
 * @param name		Name of the report.
 * @param parent	Parent of the report.
 * @param path		Path of the file to report to.
 */
Report::Report(string name, ComposedModel *parent, string path):
	Model(name, parent), _out(nullptr), _path(path) { }

///
Report::~Report() {
	for(auto r: _reps)
		delete r;
}

/**
 * @fn void Report::add(OutputPort<T, N>& port);
 * Add the given output port for reporting.
 * @param port	Added port.
 */

///
void Report::start() {
	if(_out == nullptr) {
		_out = new ofstream(_path);
		if(_out->fail())
			fatal("cannot open '" + _path + "'");
	}
	(*_out) << "date";
	for(auto r: _reps)
		(*_out) << '\t' << r->name();
	(*_out) << endl;
}

///
void Report::stop() {
	if(_path != "") {
		delete _out;
		_out = nullptr;
	}
}

///
void Report::update() {
	(*_out) << date();
	for(auto r: _reps) {
		(*_out) << '\t';
		r->print(*_out);
	}
	(*_out) << endl;
}

///
void Report::propagate(const AbstractPort& port) {
	sim().triggerLast(*this);
}

} // physim

