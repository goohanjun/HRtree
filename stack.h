/*
 * stack.h
 *
 *  Created on: Nov 6, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_STACK_H_
#define SRC_STACK_H_

#include "HDefs.h"

class HNode;

class stack {
public:
	stack();
	virtual ~stack();

	bool isChanged[MAX_STACK_SIZE];
	int choice[MAX_STACK_SIZE];
	HNode* trace[MAX_STACK_SIZE];

	int depth; //Actual depth
	int curr; //Current depth
};

#endif /* SRC_STACK_H_ */
