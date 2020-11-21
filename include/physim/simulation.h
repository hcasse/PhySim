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
#ifndef INCLUDE_PHYSIM_SIMULATION_H_
#define INCLUDE_PHYSIM_SIMULATION_H_

#include <queue>
#include <set>
#include <vector>
#include <physim/type.h>
#include <physim/monitor.h>

namespace physim {

class Model;

class Simulation {
public:
	Simulation(Model& top);
	Simulation(Model& top, Monitor& mon);
	~Simulation();
	void run();
	void run(duration_t duration);
	void runUntil(date_t date);
	void start();
	void step();
	void pause();
	void stop();

	void schedule(Model& model, date_t at);
	void trigger(Model& model);
	void commit(Model& model);

	inline date_t date() const { return _date; }
	inline Model& top() const { return _top; }
	inline Monitor& monitor() const { return *_mon; }
	inline bool tracing() const { return _tracing; }
	inline void setTracing(bool t) { _tracing = t; }
	inline bool isStopped() const { return _state == STOPPED; }
	inline bool isRunning() const { return _state == STOPPED; }
	inline bool isPaused() const { return _state == STOPPED; }

private:

	void advance();
	void stabilize();

	typedef enum {
		STOPPED,
		PAUSED,
		RUNNING
	} state_t;

	class Date {
	public:
		inline Date(date_t _at, Model& _model): at(_at), model(&_model) { }
		date_t at;
		Model *model;
		inline bool operator==(const Date& d) const
			{ return at == d.at and  model == d.model; }
		inline bool operator<(const Date& d) const
			{ return at > d.at or (at == d.at && model < d.model); }
	};

	priority_queue<Date> _sched;
	vector<Model *> _pers;
	set<Model *> _todo;
	set<Model *> _coms;

	Model& _top;
	date_t _date;
	Monitor *_mon;
	bool _mon_alloc, _tracing;
	state_t _state;
};

} // physim

#endif /* INCLUDE_PHYSIM_SIMULATION_H_ */
