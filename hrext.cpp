/*
 * hrext.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: hanjunkoo
 */

#include "hrext.h"
#include "hr_defs.h"


hr_ext::hr_ext() {
	// TODO Auto-generated constructor stub

}

hr_ext::~hr_ext() {
	// TODO Auto-generated destructor stub
}

hr_rect::hr_rect(double *key, int klen) {
	coord = new double[dim*2];
	for (int i =0;i<dim*2;i++){
		coord[i]=key[i];
	}
	dimension = dim;
}

hr_rect::~hr_rect(){
	//if ( coord != nullptr )
		//delete [] coord;
}
void
hr_rect::dealloc(){
	delete [] coord;
}

double findPen_over(hr_rect *j, hr_rect *k){ //Span 을 계산한것. 나중에 고쳐야함.

	hr_rect nRect(j->coord, sizeof(double)*j->dimension*2); // 확장시킬거 복사해놓고
	nRect.expand(k);
	double result = nRect.span()- k->span();
	nRect.dealloc();
	return result;
};
double findPen_span(hr_rect *j, hr_rect *k){ // 커진거 - 원래 꺼 부피로 계산!
	hr_rect nRect(j->coord, sizeof(double)*(j->dimension)*2 ); // 확장시킬거 복사해놓고

	nRect.expand(k);
	double result = nRect.span()- k->span();
	nRect.dealloc();

	return result;
};

double& hr_rect::lo( int d ){
	double a=0;
	if(d==0)
		return coord[0];
	if(d==1)
		return coord[1];
	if(d==2)
		return coord[4];
	return a;
};

double& hr_rect::hi( int d ){
	double a=0;
	if(d==0)
		return coord[2];
	if(d==1)
		return coord[3];
	if(d==2)
		return coord[5];
	return a;
};

bool hr_rect::isAlive(double tnow){
	if (coord[5] <= tnow)
		return false;
	return true;
}



double hr_rect::span(){
	double result = 1.0;
	for(int i=0; i< dimension-1;i++){ // 시간 제외하고 면적을 리턴해주자
		if(lo(i) > hi(i)){
			result = 0.0;
			break;
		}
		result = result* (hi(i)-lo(i));
	}
	return result;
};
double hr_rect::margin(){
	double result = 0;
	for(int i =0;i<dimension-1; i++){ //시간 제외한 마진값임
		result = result +(hi(i)-lo(i));
	}
	return result;
};


double hr_rect::intersect(hr_rect *j){ //
	double result = 1.0;
	for(int i = 0; i<dimension-1;i++){ //시간은 상관없음!
		if( lo(i) > j->hi(i) || hi(i) < j->lo(i)) //No intersection!
			result = result *0;
		if( lo(i) > j->lo(i) && hi(i) < j->hi(i))
			result = result* (hi(i)-lo(i));

		if( lo(i) < j->lo(i) && hi(i) > j->hi(i))
			result = result* (j->hi(i)-j->lo(i));

		if( lo(i) > j->lo(i) && hi(i) > j->hi(i))
			result = result* (j->hi(i)-lo(i));

		if( lo(i) < j->lo(i) && hi(i) < j->hi(i))
			result = result* (hi(i)-j->lo(i));
	}

	return result > 0.0 ? result : 0.0;
};

double hr_rect::overlap(hr_rect *j){
	double result = 1.0;
	double temp_x = 0.0;
	for(int i = 0; i<dimension-1;i++){ //시간은 고려 ㄴㄴ
		if( hi(i)> j->hi(i) )
			temp_x = j->hi(i);
		else
			temp_x = hi(i);
		if( lo(i)> j->lo(i) )
			temp_x -= lo(i);
		else
			temp_x -= j->lo(i);
		if (temp_x<0)
			temp_x = 0;
		result *=temp_x;
	}

	return result > 0.0 ? result : 0.0;
};

bool hr_rect::expand(hr_rect *j){ //시간 고려 ㄴㄴ
	bool flag = false;
	for (int i =0;i<dimension;i++){
		if(j->lo(i) < lo(i)){
			lo(i) = j->lo(i);
			flag= true;
		}
		if(j->hi(i) > hi(i)){
			hi(i) = j->hi(i);
			flag= true;
		}
	}
	return flag;
}
bool hr_rect::expand( double * key){
	//space
	bool flag = false;
	if( coord[0] > key[0]){
		flag= true;
		coord[0] = key[0];
	}
	if( coord[1] > key[1]){
		flag= true;
		coord[1] = key[1];
	}
	if( coord[2] < key[2]){
		flag= true;
		coord[2] = key[2];
	}
	if( coord[3] < key[3]){
		flag= true;
		coord[3] = key[3];
	}
	if( coord[4] > key[4]){
		flag= true;
		coord[4] = key[4];
	}
	if( coord[5] < key[5]){
		flag= true;
		coord[5] = key[5];
	}
	return flag;
}
void copyRect(hr_rect *j, hr_rect *k){

};




















