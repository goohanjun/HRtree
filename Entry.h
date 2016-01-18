/*
 * Entry.h
 *
 *  Created on: Jan 16, 2016
 *      Author: hanjunkoo
 */

#ifndef SRC_ENTRY_H_
#define SRC_ENTRY_H_

#include "HDefs.h"

class HNode;

class Entry {
public:
	Entry();
	virtual ~Entry();

	double bp[dim*2];
	bool pos;
	HNode* child;
	int data;
	int dlen;
};

#endif /* SRC_ENTRY_H_ */
