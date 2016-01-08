/*
 * hrext.h
 *
 *  Created on: Aug 24, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_HREXT_H_
#define SRC_HREXT_H_

#include "HDefs.h"

using namespace std;
class hr_ext;
class hr_rect;

class hr_ext {
public:
	hr_ext();
	virtual ~hr_ext();
};

class hr_rect{
public:
	int dimension;
	double *coord;

	hr_rect();
	virtual ~hr_rect();
	hr_rect(double *key);
	hr_rect(double *key, int a);

	void dealloc();
	bool isEqual( hr_rect *k );
	bool isEqual( double *k );
	bool isAlive( hr_rect *k );
	bool isAlive(double tnow);
	bool isInclude(double* key);
	bool isIncluded(double* key);
	bool isNew( hr_rect *k );

	void copyRect(double *key);

	bool expand( hr_rect *k );
	bool expand( double * key);
	double intersect ( hr_rect *k);
	double overlap (hr_rect *k);
	double span();
	double margin();
	double dist(hr_rect *j);
	double & lo( int d );
	double & hi( int d );

	bool isTimeOverlap(double *bp);
private:
};

double findPen_over(hr_rect *j, hr_rect *k);

double findPen_span(hr_rect *j, hr_rect *k);



#endif /* SRC_HREXT_H_ */
