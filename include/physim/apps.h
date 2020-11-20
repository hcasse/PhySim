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
#ifndef INCLUDE_PHYSIM_APPS_H_
#define INCLUDE_PHYSIM_APPS_H_

#include <physim/composed.h>

namespace physim {

class ApplicationModel: public ComposedModel {
public:
	ApplicationModel(string name);

	int run(int argc = 1, char **argv = nullptr);
	inline void setTracing(bool t) { _tracing = t; }

protected:
	virtual int perform() = 0;
	virtual int parseOption(int& i, int argc, char **argv);
	virtual void dumpOptions();
	void errorOption(const string& msg);
	inline Simulation& sim() const { return *_sim; }

private:
	Simulation *_sim;
	bool _tracing;
};


class Simulate: public ApplicationModel {
public:
	Simulate(string name, duration_t d = 10);
protected:
	int perform() override;
	int parseOption(int& i, int argc, char **argv) override;
	void dumpOptions() override;
private:
	duration_t _d;
};

#define PHYSIM_RUN(C) int main(int argc, char **argv) { return C().run(argc, argv); }

} // physim

#endif /* INCLUDE_PHYSIM_APPS_H_ */
