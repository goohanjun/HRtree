/*
 * RootTable.h
 *
 *  Created on: Nov 6, 2015
 *      Author: hanjunkoo
 */

#include "RootTable.h"


RootTable::RootTable(){
	Root[0] = new HNode();
	Root[0]->isRoot = true;
	numRoot = 1;
	currentTime = 0;
}

//TODO
RootTable::~RootTable(){
	for (int i = 0; i<numRoot;i++)
		delete Root[i];
}

//TODO
int RootTable::StampQueryBinarySearch(){
	return 1;
}

//TODO
int RootTable::IntervalBinarySearch(){
	return 1;
}
