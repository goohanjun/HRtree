/*
 * hrext.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: hanjunkoo
 */

#include "hrext.h"

#include "HDefs.h"

hr_ext::hr_ext() {

}

hr_ext::~hr_ext() {

}

hr_rect::hr_rect(){ // Make own memory space
	dimension = dim;
	coord = new double[dimension*2];
}

hr_rect::hr_rect(double *key) { // Make own memory space
	coord = new double[dimension*2];
	dimension = dim;
	int i;
	for (i = 0; i< dimension*2;i++){
		coord[i]=key[i];
	}
}

hr_rect::hr_rect(double *key, int a) { // use key's memory space
	coord = key;
	dimension = dim;
}

hr_rect::~hr_rect() {

}

void hr_rect::dealloc() {
	delete[] coord;
}

double findPen_over(hr_rect *j, hr_rect *k) { //Span 을 계산한것. 나중에 고쳐야함.

	hr_rect nRect(j->coord); // 확장시킬거 복사해놓고
	nRect.expand(k);
	double result = nRect.span() - k->span();
	nRect.dealloc();
	return result;
}

double findPen_span(hr_rect *j, hr_rect *k) { // 커진거 - 원래 꺼 부피로 계산!
	hr_rect nRect(j->coord); // 확장시킬거 복사해놓고

	nRect.expand(k);
	double result = nRect.span() - k->span();
	nRect.dealloc();

	return result;
}

double& hr_rect::lo(int d) {
	if (d == 0)
		return coord[0];
	if (d == 1)
		return coord[1];
	if (d == 2)
		return coord[4];
	return coord[0]; // To remove warning
}

double& hr_rect::hi(int d) {
	if (d == 0)
		return coord[2];
	if (d == 1)
		return coord[3];
	if (d == 2)
		return coord[5];
	return coord[0]; // To remove warning
}

bool hr_rect::isAlive(double tnow) {
	if (coord[5] <= tnow)
		return false;
	return true;
}

bool hr_rect::isInclude(double* key) {
	bool flag = true;
	if(coord[0] > key [0] ||  coord[1] > key[1])
		flag = false;
	if(coord[2] < key [2] || coord[3] < key[3])
		flag = false;
	return flag;
}

bool hr_rect::isIncluded(double* key) {
	bool flag = true;
	if(key[0] > coord [0] ||  key[1] > coord[1])
		flag = false;
	if(key[2] < coord [2] || key[3] < coord[3])
		flag = false;
	return flag;
}

double hr_rect::span() {
	double result = 1.0;
	for (int i = 0; i < dimension - 1; i++) { // 시간 제외하고 면적을 리턴해주자
		if (lo(i) > hi(i)) {
			result = 0.0;
			break;
		}
		result = result * (hi(i) - lo(i));
	}
	return result;
}

double hr_rect::margin() {
	double result = 0;
	for (int i = 0; i < dimension - 1; i++) { //시간 제외한 마진값임
		result = result + (hi(i) - lo(i));
	}
	return result;
}

double hr_rect::intersect(hr_rect *j) { //
	double result = 1.0;
	for (int i = 0; i < dimension - 1; i++) { //시간은 상관없음!
		if (lo(i) > j->hi(i) || hi(i) < j->lo(i)) //No intersection!
			result = result * 0;
		if (lo(i) > j->lo(i) && hi(i) < j->hi(i))
			result = result * (hi(i) - lo(i));

		if (lo(i) < j->lo(i) && hi(i) > j->hi(i))
			result = result * (j->hi(i) - j->lo(i));

		if (lo(i) > j->lo(i) && hi(i) > j->hi(i))
			result = result * (j->hi(i) - lo(i));

		if (lo(i) < j->lo(i) && hi(i) < j->hi(i))
			result = result * (hi(i) - j->lo(i));
	}
	return result > 0.0 ? result : 0.0;
}

double hr_rect::overlap(hr_rect *j) {
	double result = 1.0;
	double temp_x = 0.0;
	for (int i = 0; i < dimension - 1; i++) { //시간은 고려 ㄴㄴ
		if (hi(i) > j->hi(i))
			temp_x = j->hi(i);
		else
			temp_x = hi(i);
		if (lo(i) > j->lo(i))
			temp_x -= lo(i);
		else
			temp_x -= j->lo(i);
		if (temp_x < 0)
			temp_x = 0;
		result *= temp_x;
	}
	return result > 0.0 ? result : 0.0;
}

bool hr_rect::expand(hr_rect *j) {
	bool flag = false;
	for (int i = 0; i < dimension; i++) {
		if (j->lo(i) < lo(i)) {
			lo(i) = j->lo(i);
			flag = true;
		}
		if (j->hi(i) > hi(i)) {
			hi(i) = j->hi(i);
			flag = true;
		}
	}
	return flag;
}

bool hr_rect::expand(double * key) { //Expand the area only.
	bool flag = false;
	if (coord[0] > key[0]) {
		flag = true;
		coord[0] = key[0];
	}
	if (coord[1] > key[1]) {
		flag = true;
		coord[1] = key[1];
	}
	if (coord[2] < key[2]) {
		flag = true;
		coord[2] = key[2];
	}
	if (coord[3] < key[3]) {
		flag = true;
		coord[3] = key[3];
	}
	return flag;
}

void hr_rect::copyRect(double *key) {
	int i;
	for(i=0;i<dimension*2;i++){
		coord[i] = key[i];
	}
}

bool hr_rect::isTimeOverlap(double *bp) {
	double t_start =bp[4];
	double t_end =bp[5];
	bool flag;
	if(t_end <= coord[4] || t_start >= coord[5])
		flag = false;
	else
		flag = true;
	return flag;
}

bool hr_rect::isEqual(hr_rect *rect){
	bool flag = true;
	for(int i = 0;i <dim*2-2;i++){ // Compare BP only. not time
		if( coord[i] != rect->coord[i] )
			flag = false;
	}
	return flag;
}

bool hr_rect::isEqual(double *bp){
	bool flag = true;
	for(int i = 0;i <dim*2-2;i++){ // Compare BP only. not time
		if( coord[i] != bp[i] )
			flag = false;
	}
	return flag;
}
