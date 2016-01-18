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
#include "Entry.h"


class HNode;
class RootTable;

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


	bool _isMinMaxCorrect();
	bool _isUnderflow();
	bool _isAliveBPCorrectNode();
	bool _isDeadBPCorrectNode(double tnow);
	bool _isAliveBPCorrectEntry(Entry* Entry, double tnow);
	bool _isDeadBPCorrectEntry(Entry* Entry, double tnow);


};

#endif /* SRC_HNODE_H_ */
