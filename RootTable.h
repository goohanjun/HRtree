/*
 * RootTable.h
 *
 *  Created on: Nov 6, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_ROOTTABLE_H_
#define SRC_ROOTTABLE_H_

#include "HNode.h"
#include "HDefs.h"

class RootTable {
public:
	RootTable();
	virtual ~RootTable();

	int StampQueryBinarySearch();
	int IntervalBinarySearch();

	HNode* Root[MAX_ROOT];
	int numRoot;

	//int currentTime;
};


#endif /* SRC_ROOTTABLE_H_ */
