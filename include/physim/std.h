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
#ifndef INCLUDE_PHYSIM_STD_H_
#define INCLUDE_PHYSIM_STD_H_

#include <physim.h>

namespace physim {

template <class T>
class Constant: public ReactiveModel {
public:
	OutputPort<T> y;
	Constant(const T& y_, ComposedModel *parent)
		: ReactiveModel(to_string(y_), parent), _y(y_), y(this, "y") { }
protected:
	void init() override {
		y = _y;
		y.propagate();
	}
private:
	T _y;
};

template <class T, int N = 1>
class Display: public ReactiveModel {
public:
	InputPort<T, N> x;
	Display(string name, ComposedModel *parent, ostream& out = cout):
		ReactiveModel(name, parent), x(this, "x"), _out(out) { }

protected:
	void update() override {
		_out << name() << ": "  << date() << ":";
		if(N == 1)
			_out << " " << x;
		else
			for(int i = 0; i < N; i++)
				_out << " " << x[i];
		_out << endl;
	}
	ostream& _out;
};

class Report: public Model {
	class AbstractReporter {
	public:
		virtual ~AbstractReporter() { }
		virtual string name() = 0;
		virtual void print(ostream& out) = 0;
	};

	template <class T, int N = 1>
	class Reporter: public AbstractReporter {
	public:
		Reporter(Report *report, OutputPort<T, N>& port)
			: out(port), in(report, "") { }
		string name() override { return out.fullname(); }
		void print(ostream& out) {
			out << in[0];
			for(int i = 1; i < N; i++) out << ' ' << in[i];
		}
		OutputPort<T, N>& out;
		InputPort<T, N> in;
	};

public:

	Report(string name, ComposedModel *parent, ostream& out = cout);
	Report(string name, ComposedModel *parent, string path);
	~Report();

	template <class T, int N>
	void add(OutputPort<T, N>& port) {
		auto r = new Reporter<T, N>(this, port);
		_reps.push_back(r);
		parent()->connect(port, r->in);
	}

protected:
	void start() override;
	void stop() override;
	void update() override;
	void propagate(const AbstractPort& port) override;

private:
	ostream *_out;
	string _path;
	vector<AbstractReporter *> _reps;
};

} // physim

#endif /* INCLUDE_PHYSIM_STD_H_ */
