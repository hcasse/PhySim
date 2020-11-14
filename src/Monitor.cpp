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
using namespace std;

/**
 * @class Monitor
 * Class supporting exchanges (mainly display) with human user.
 */

/**
 * Constructor.
 * @param out	Output stream (default cout).
 * @param err	Error stream (default cerr).
 */
Monitor::Monitor(ostream& out, ostream& err): _out(&out), _err(&err), _sim(nullptr) {
}

///
Monitor::~Monitor() {
}

/**
 * @fn void Monitor::info(const string& msg);
 * Display information to the user.
 * @param msg	Message to display.
 */

/**
 * @fn void Monitor::warn(const string& msg);
 * Display a warning.
 * @param msg	Message to display.
 */

/**
 * @fn void Monitor::error(const string& msg);
 * Display an error.
 * @param msg	Message to display.
 */

/**
 * @fn  void Monitor::fatal(const string& msg);
 * Display an error and stop the simulation.
 * @param msg	Message to display.
 */

/**
 * @fn ostream& Monitor::out() const;
 * Get the standard output.
 * @return	Standard output.
 */

/**
 * @fn ostream& Monitor::err() const;
 * Get the standard error output.
 * @return	Standard error output.
 */


/**
 * @class TerminalMonitor
 * Monitor for terminal.
 */

///
void TerminalMonitor::info(const string& msg) {
	err() << "INFO: " << msg << endl;
}

///
void TerminalMonitor::warn(const string& msg) {
	err() << "WARNING: " << msg << endl;
}

///
void TerminalMonitor::error(const string& msg) {
	err() << "ERROR: " << msg << endl;
}

///
void TerminalMonitor::fatal(const string& msg) {
	err() << "ERROR: " << msg << endl;
	_sim->stop();
}

};	// physim
