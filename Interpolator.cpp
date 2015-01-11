/*
 * Interpolator.cpp
 *
 *  Created on: Nov 1, 2009
 *      Author: arnaud
 */

#include "Interpolator.h"

/*!
 * constructor
 */
Interpolator::Interpolator(double begin_time, Polygon* begin_pts,
		double end_time, Polygon* end_pts) {
	debug("Interpolator::constructor");

	this->begin_time = begin_time;
	this->begin_pts = begin_pts;
	this->end_time = end_time;
	this->end_pts = end_pts;
}

/*!
 * destructor
 */
Interpolator::~Interpolator() {
	debug("Interpolator::destructor");
}

static inline double interpolate(double t1, double x1, double t2, double x2,
		double t) {
	double a = (x2 - x1) / (t2 - t1);
	double b = x1 - a * t1;
	return a * t + b;
}

Polygon Interpolator::get_time(double t) {
	Polygon p;

	vector<CvPoint2D32f>::iterator p1 = begin_pts->points.begin();
	vector<CvPoint2D32f>::iterator p2 = end_pts->points.begin();
	int nb_points = min(begin_pts->points.size(), end_pts->points.size());

	for (int pt_index = 0; pt_index < nb_points; ++pt_index) {
		CvPoint2D32f current;
		current.x = interpolate(begin_time, p1->x, end_time, p2->x, t);
		current.y = interpolate(begin_time, p1->y, end_time, p2->y, t);
		p.points.push_back(current);
		// advance iterators
		++p1;
		++p2;
	}

	return p;
}
