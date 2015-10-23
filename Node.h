/*
 * Node.h
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

class Node;
class Entry;

#include "hr_defs.h"

class Entry {
public:
	Entry();
	virtual ~Entry();

	double bp[dim*2];
	bool pos;
	Node* child;
	int data;
	int dlen;

private:
};

const int numEnt_MAX = (PAGE_SIZE - sizeof(double)*dim*2 - sizeof(bool) - sizeof(int)*2 ) / (sizeof(Entry));

class Node {
public:

	double bp[dim*2]; //현재 타임에서 살아있는 애들의 BP
	Entry entries[numEnt_MAX];
	bool isRoot;
	unsigned int level;
	int numEntry;


	Node();
	virtual ~Node();

	void copyNode(Node *node);


	int _deleteParent( int trace[], int depth, double tnow);
	int _UpdateParent(Node* Path[], int trace[], int& depth, int& numTrace, int status,
			Node* child1, Node* child2, Node* Root[], int & numRoot,double tnow);



	int _Vsplit(int numKey, double* key1 ,double *key2, double* rkey,double* lkey,
			int rightE[], int& nR,  int leftE[] , int& nL);



	int _Ksplit(int numKey, double* key1 ,double *key2, bool& oneGoesRight, bool& twoGoesRight,
			int rightEntries[], int& nR, double* rkey,double* lkey);

	int _SplitNode(double *rkey,double *lkey,  int rightE[], int nR,  int leftE[] , int nL,
				int& Status, Node* Root[], int & numRoot,Node*& nNode1,Node*& nNode2,Node *self);

	int _findMinPen(double *key);
	int _findMinPenOver(double *key);
	int _numAlive(double tnow);

	double* _keyAlive(double tnow);
	bool _SplitKey(double *oldKey, double *newKey, double tnow);




};



#endif /* SRC_NODE_H_ */
