/*
 * Monitor.cpp
 *
 *  Created on: 10 nov. 2020
 *      Author: casse
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
