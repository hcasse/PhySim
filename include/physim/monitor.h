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
#ifndef INCLUDE_PHYSIM_MONITOR_H_
#define INCLUDE_PHYSIM_MONITOR_H_

#include <string>
#include <iostream>

namespace physim {

using namespace std;

class Simulation;

class Monitor {
public:
	Monitor(ostream& out = cout, ostream& err = cerr);
	virtual ~Monitor();
	virtual void info(const string& msg) = 0;
	virtual void warn(const string& msg) = 0;
	virtual void error(const string& msg) = 0;
	virtual void fatal(const string& msg) = 0;
	inline ostream& out() const { return *_out; }
	inline ostream& err() const { return *_err; }
protected:
	Simulation *_sim;
private:
	ostream *_out, *_err;
};

class TerminalMonitor: public Monitor {
public:
	using Monitor::Monitor;
	void info(const string& msg) override;
	void warn(const string& msg) override;
	void error(const string& msg) override;
	void fatal(const string& msg) override;
};

} // physim

#endif /* INCLUDE_PHYSIM_MONITOR_H_ */
