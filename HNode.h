/*
 * Node.h
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_HNODE_H_
#define SRC_HNODE_H_

#include "HDefs.h"
#include "stack.h"
#include "RootTable.h"

class HNode;
class RootTable;

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

const int MaxEntry = (PAGE_SIZE - sizeof(double)*dim*2 - sizeof(bool) - sizeof(int)*2 ) / (sizeof(Entry));

class HNode {
public:

	double bp[dim*2]; //현재 타임에서 살아있는 애들의 BP
	Entry entries[MaxEntry];
	bool isRoot;
	int level;
	int numEntry;
	int NodeNumber;

	HNode();
	virtual ~HNode();

	int _findMinPen(double *key);
	int _findMinPenOver(double *key);

	void copyNode(HNode *node);
	bool _deleteEntry(double* key, int data);
	int _numAlive(double tnow);
	int _numAlive();
	bool _keyAlive(double* key, double tnow);
};

#endif /* SRC_HNODE_H_ */
