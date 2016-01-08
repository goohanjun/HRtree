/*
 * pCursor.h
 *
 *  Created on: Jan 5, 2016
 *      Author: hanjunkoo
 */

#ifndef SRC_PCURSOR_H_
#define SRC_PCURSOR_H_

#include "HDefs.h"
#include "HNode.h"
#include <cstring>
#include <iostream>
class Entry;

class pCursor {
public:
	pCursor();
	virtual ~pCursor();

	Entry entries[MaxEntry*2];
	int size;

	bool InsertNode(HNode *nNode);
	bool InsertEntry(HNode *nNode);
	int numAliveEntry();

	void printCursor();
private:

};


#endif /* SRC_PCURSOR_H_ */
