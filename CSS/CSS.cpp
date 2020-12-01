/*
 * CSS.cpp
 *
 *  Created on: 1 déc. 2020
 *      Author: casse
 */

#include <physim.h>
#include <physim/test.h>
#include <physim/std.h>
#ifndef NO_QT
#	include <physim/qtcharts.h>
#endif
using namespace physim;

class World: public Simulate {
public:

	World():
		Simulate("CSS", 25)
	{
	}
};

PHYSIM_RUN(World);
