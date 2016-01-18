/*
 * Entry.cpp
 *
 *  Created on: Jan 16, 2016
 *      Author: hanjunkoo
 */

#include "Entry.h"


Entry::Entry() {
	// TODO Auto-generated constructor stub
	int i;
	for(i=0;i<dim*2;i++){
		bp[i]=0.0;
	}
	pos = true;
	dlen = 0;
	data= 0;
	child = 0;
}

Entry::~Entry() {
	// TODO Auto-generated destructor stub
}
