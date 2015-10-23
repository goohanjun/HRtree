/*
 * hrext.h
 *
 *  Created on: Aug 24, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_HREXT_H_
#define SRC_HREXT_H_

using namespace std;
class hr_ext;
class hr_rect;

class hr_ext {
public:
	hr_ext();
	virtual ~hr_ext();

	//double findPenalty(hr_rect *k);


};

class hr_rect{
public:
	hr_rect();
	hr_rect(double *key, int klen);
	hr_rect(double key[]);
	virtual ~hr_rect();

	void dealloc();
	bool isEqual( hr_rect *k );
	bool isAlive( hr_rect *k );
	bool isAlive(double tnow);
	bool isNew( hr_rect *k );

	bool expand( hr_rect *k );
	bool expand( double * key);
	double intersect ( hr_rect *k);
	double overlap (hr_rect *k);
	double span();
	double margin();
	double dist(hr_rect *j);
	double & lo( int d );
	double & hi( int d );


	int dimension;
	double *coord;

private:
};

double findPen_over(hr_rect *j, hr_rect *k);

double findPen_span(hr_rect *j, hr_rect *k);


#endif /* SRC_HREXT_H_ */
