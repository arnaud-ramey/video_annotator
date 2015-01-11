/*
 * Interpolator.h
 *
 *  Created on: Nov 1, 2009
 *      Author: arnaud
 */

#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_

#include "XmlFile.h"

class Interpolator {
public:
	Interpolator(double begin_time, Polygon* begin_pts, double end_time,
			Polygon* end_pts);
	~Interpolator();
	Polygon get_time(double t);

	Polygon* begin_pts;
	Polygon* end_pts;
	double begin_time;
	double end_time;
};

#endif /* INTERPOLATOR_H_ */
