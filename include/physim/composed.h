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
#ifndef INCLUDE_PHYSIM_COMPOSED_H_
#define INCLUDE_PHYSIM_COMPOSED_H_

#include <vector>
#include <physim/ports.h>

namespace physim {

class ComposedModel: public Model {
	friend class Model;
public:
	ComposedModel(string name, ComposedModel *parent = nullptr);

	template <class T, int N> void connect(InputPort<T, N>& ip, OutputPort<T, N>& op)
		{ ip._back = &op; }
	template <class T, int N> void connect(InputPort<T, N>& ips, InputPort<T, N>& ipt)
		{ ipt._back = &ips; }
	template <class T, int N> void connect(OutputPort<T, N>& ops, OutputPort<T, N>& opt)
		{ opt._back = &ops; }
	template <class T, int N> void connect(OutputPort<T, N>& ops, InputPort<T, N>& ips)
		{ ips._back = &ops; }

	inline const vector<Model *>& subModels() const { return subs; }
	bool isComposed() const override { return true; }

protected:
	void init() override;
	//void propagate(const AbstractPort& port) override;	TODO
	void start() override;
	void stop() override;
	void finalize(Simulation& sim) override;
	//void publish() override;

private:
	vector<Model *> subs;
};

} // physim

#endif /* INCLUDE_PHYSIM_COMPOSED_H_ */
