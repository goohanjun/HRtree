/*
 * stack.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: hanjunkoo
 */

#include "stack.h"

stack::stack(){
	curr = 0;
	depth = 0;
	for(int i = 0;i<MAX_STACK_SIZE;i++)
		isChanged[i]=false;
};
stack::~stack(){

};

