/*
 * HNode.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */
#include "hrext.h"
#include "HNode.h"

#include <iostream>
#include <algorithm>
#include <string.h>

using namespace std;

HNode::HNode() {
	// TODO Auto-generated constructor stub
	bp[0]=DBL_MAX;
	bp[1]=DBL_MAX;
	bp[2]=0.0;
	bp[3]=0.0;
	bp[4]=0.0;
	bp[5]=DBL_MAX;
	isRoot = false;
	level = 1;
	numEntry = 0;
	NodeNumber = 0;
}

HNode::~HNode() {
	// TODO Auto-generated destructor stub
}

int HNode::_findMinPen(double *key) {

	int index = 0;
	double minPenalty = DBL_MAX;
	double tempPenalty = 0.0;
	double minPenArea = DBL_MAX;
	double tempSpan = 0.0;

	hr_rect r;
	r.copyRect(key);
	for (int i = 0; i < numEntry; i++) {
		//check
		if (entries[i].bp[5] > key[4]) { //살아있는 애들 중에 ㅇㅇ
			hr_rect r1,r2;
			r1.copyRect(entries[i].bp);
			r2.copyRect(entries[i].bp);
			r2.expand(&r);
			tempSpan = r1.span();
			tempPenalty = r2.span() - tempSpan;

			if (tempPenalty <= minPenalty) {
				if (tempPenalty == minPenalty) {
					if (minPenArea > tempSpan) {
						index = i;
						minPenArea = tempSpan;
					}
				} else {
					index = i;
					minPenalty = tempPenalty;
					minPenArea = tempSpan;
				}
			}
			r1.dealloc();r2.dealloc();
		}
	}
	r.dealloc();
	return index;
}

int HNode::_findMinPenOver(double *key) {
	int index = 0;
	double minPenalty = DBL_MAX;
	double tempPenalty;
	double minPenArea = DBL_MAX;
	double tempArea;

	hr_rect k;
	k.copyRect(key);
	//cout<<"FindMinPenOver numEntry = "<<numEntry<<endl;
	for (int i = 0; i < numEntry; i++) {
		//check
		tempPenalty = 0.0;
		tempArea = 0.0;
		if (entries[i].bp[5] > key[4]) { //살아있으면!
			hr_rect r1,r;
			r1.copyRect(entries[i].bp);
			r.copyRect(entries[i].bp);
			r.expand(&k);
			for (int j = 0; j < numEntry; j++) {
				hr_rect r2;
				r2.copyRect(entries[j].bp);
				if (i != j) {
					tempPenalty += r.intersect(&r2) - r1.intersect(&r2);
				}
				r2.dealloc();
			}
			//cout<<"Temp Penalty "<<i<<" "<<tempPenalty<<endl;
			if (tempPenalty <= minPenalty) {
				if (tempPenalty == minPenalty) {
					tempArea = r.span() - r1.span();
					if (tempArea <= minPenArea) {
						minPenArea = tempArea;
						index = i;
					}
				} else {
					index = i;
					minPenalty = tempPenalty;
					minPenArea = r.span() - r1.span();
				}
			}
			//delete r1; delete r;
			r1.dealloc();
			r.dealloc();
		}
	}
	k.dealloc();
	return index;
}

int HNode::_numAlive(double tnow) {
	int num = 0;
	for (int i = 0; i < numEntry; i++) {
		if (tnow < entries[i].bp[5])
			num++;
	}
	return num;
}

int HNode::_numAlive() {
	int num = 0;
	for (int i = 0; i < numEntry; i++) {
		if (entries[i].bp[5] == DBL_MAX)
			num++;
	}
	return num;
}

bool HNode::_keyAlive(double* key, double tnow) {

	hr_rect r1;
	bool flag = false;

	for (int i = 0; i < numEntry; i++) {
		if (tnow >= entries[i].bp[4] && tnow < entries[i].bp[5]) { //살아있음
			memcpy(key, entries[i].bp, klen);
			flag = true;
			break;
		}
	}
	if (flag) { // When the node is alive
		r1.copyRect(key);
		for (int i = 0; i < numEntry; i++) {
			if (tnow >= entries[i].bp[4] && tnow < entries[i].bp[5]) { //살아있음
				r1.expand(entries[i].bp);
			}
		}
	}
	else{ // When the node is deleted (closed)
		r1.copyRect(entries[0].bp);
		for (int i = 1; i < numEntry; i++) {
			r1.expand(entries[i].bp);
		}
	}
	memcpy(key, r1.coord, klen);
	r1.dealloc();
	//살아있는 엔트리가 없다면 false를 Return.
	return flag;
}

void HNode::copyNode(HNode *HNode) {
	memcpy(bp, HNode->bp, klen);
	for (int i = 0; i < HNode->numEntry; i++) {
		memcpy(&entries[i], &HNode->entries[i], sizeof(Entry));
	}
	numEntry = HNode->numEntry;
	isRoot = HNode->isRoot;
	level = HNode->level;
}

bool HNode::_deleteEntry(double* key, int data) {
	bool flag = false;
	for (int i = 0; i < numEntry; i++) {
		if (entries[i].data == data) {
			if (entries[i].bp[4] == key[5]) { //Physically Deleted
				if (i != numEntry - 1) {
					memcpy(&entries[i], &entries[numEntry - 1], sizeof(Entry));
				}
				numEntry--;
			} else {  // Logically Deleted
				entries[i].bp[5] = key[5]; // 시간을 닫아주고

			}
		}
	}
	return flag;
}

Entry::Entry() {
	// TODO Auto-generated constructor stub
	int i;
	for(i=0;i<dim*2;i++){
		bp[i]=0.0;
	}
	pos = true;
	dlen = 0;
	data= 0;
	child = 0;
}

Entry::~Entry() {
	// TODO Auto-generated destructor stub
}
