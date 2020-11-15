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
#ifndef INCLUDE_PHYSIM_QTCHARTS_H_
#define INCLUDE_PHYSIM_QTCHARTS_H_

#include <physim.h>
#include <QColor>

namespace physim {

class QtLineDisplay: public Model {

	class AbstractReporter {
	public:
		AbstractReporter(QtLineDisplay *d);
		virtual ~AbstractReporter() { }
		virtual string name() = 0;
		virtual void record() = 0;
		QColor color;
		vector<double> vals;
	};

	template <class T, int N = 1>
	class Reporter: public AbstractReporter {
	public:
		Reporter(QtLineDisplay *d, OutputPort<T, N>& port, int i = 0)
			: AbstractReporter(d), out(port), in(d, ""), _i(i) { }
		string name() override {
			if(N == 1)
				return out.fullname();
			else
				return out.fullname() + "[" + to_string(_i) + "]";
		}
		void record() override { vals.push_back(out[_i]); }
		OutputPort<T, N>& out;
		InputPort<T, N> in;
		int _i;
	};

public:

	QtLineDisplay(string name, ComposedModel *parent);
	~QtLineDisplay();

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
	vector<AbstractReporter *> _reps;
	vector<date_t> _dates;
	int _color;
};

} // physim

#endif /* INCLUDE_PHYSIM_QTCHARTS_H_ */
