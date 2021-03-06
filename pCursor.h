/*
 * pCursor.h
 *
 *  Created on: Jan 5, 2016
 *      Author: hanjunkoo
 */
#ifndef SRC_PCURSOR_H_
#define SRC_PCURSOR_H_

#include "HNode.h"
#include "Entry.h"
#include "HDefs.h"

#include <cstring>
#include <iostream>

class pCursor {
public:
	pCursor();
	virtual ~pCursor();

	Entry entries[MaxEntry*2];
	int size;

	bool InsertNode(HNode *nNode);
	bool InsertEntry(HNode *nNode);
	bool InsertEntry(Entry *nEntry);
	int numAliveEntry();
	void printCursor();
private:
};


#endif /* SRC_PCURSOR_H_ */
