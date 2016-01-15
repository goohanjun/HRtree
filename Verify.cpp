/*
 * Verify.cpp
 *
 *  Created on: Jan 3, 2016
 *      Author: hanjunkoo
 */

#include "Verify.h"
#include "HDefs.h"
#include "hrext.h"
#include <iostream>



// Check the order of BP
bool _isMinMaxCorrect(HNode* HNode) {
	bool flag = true;
	//For BP in the Node
	if (HNode->bp[0] > HNode->bp[2]) // if X_min > X_max, then it is wrong
		flag = false;
	if (HNode->bp[1] > HNode->bp[1]) // if Y_min > Y_max, then it is wrong
			flag = false;
	if (HNode->bp[4] >= HNode->bp[5]) // if t_start >= t_end, then it is wrong
			flag = false;

	//For BP in the entry
	for (int i = 0; i < HNode->numEntry; i++) {
		if (HNode->entries[i].bp[0] > HNode->entries[i].bp[2]) // if X_min > X_max, then it is wrong
			flag = false;
		if (HNode->entries[i].bp[1] > HNode->entries[i].bp[1]) // if Y_min > Y_max, then it is wrong
				flag = false;
		if (HNode->entries[i].bp[4] >= HNode->entries[i].bp[5]) // if t_start >= t_end, then it is wrong
				flag = false;
	}
	return flag;
}

//Return true it the node is underflow.
bool _isUnderflow(HNode* HNode) {
	bool flag = true;
	int numAlive = HNode->_numAlive(DBL_MAX - 1); //tnow = DBL_MAX-1
	if (0 < numAlive && numAlive <= MaxEntry * UF_Ratio
			&& HNode->isRoot == false)
		flag = false;
	return flag;
}

//Return false if the node's bp is wrong
bool _isAliveBPCorrectNode(HNode* HNode) {
	hr_rect Rect,tempRect;
	Rect.copyRect(HNode->bp);

	bool flag = false;
	for (int i = 0; i < HNode->numEntry; i++) {
		if( HNode->entries[i].bp[5] == DBL_MAX ){
			if(flag == false){
				tempRect.copyRect(HNode->entries[i].bp);
				flag = true;
			}
			tempRect.expand(HNode->entries[i].bp);
		}
	}
	return Rect.isEqual(&tempRect);
}

bool _isDeadBPCorrectNode(HNode* HNode, double tnow) {
	hr_rect Rect,tempRect;
	Rect.copyRect(HNode->bp);

	bool flag = false;
	for (int i = 0; i < HNode->numEntry; i++) {
		if (flag == false && Rect.isTimeOverlap(HNode->entries[i].bp)) {
			tempRect.copyRect(HNode->entries[i].bp);
			flag = true;
		}
		if (Rect.isTimeOverlap(HNode->entries[i].bp))
			tempRect.expand(HNode->entries[i].bp);
	}
	return Rect.isEqual(&tempRect);
}

bool _isAliveBPCorrectEntry(Entry* Entry, double tnow) {
	hr_rect Rect,tempRect;
	Rect.copyRect(Entry->bp);

	bool flag = false;
	for (int i = 0; i < Entry->child->numEntry; i++) {
		if( Entry->child->entries[i].bp[5] == DBL_MAX ){
			if(flag == false){
				tempRect.copyRect(Entry->child->entries[i].bp);
				flag = true;
			}
			tempRect.expand(Entry->child->entries[i].bp);
		}
	}
	return Rect.isEqual(&tempRect);
}

bool _isDeadBPCorrectEntry(Entry* Entry, double tnow) {
	hr_rect Rect, tempRect;
	Rect.copyRect(Entry->bp);
	bool flag = false;
	for (int i = 0; i < Entry->child->numEntry; i++) {
		if (flag == false && Rect.isTimeOverlap(Entry->child->entries[i].bp)) {
			tempRect.copyRect(Entry->child->entries[i].bp);
			flag = true;
		}
		if (Rect.isTimeOverlap(Entry->child->entries[i].bp)) {
			tempRect.expand(Entry->child->entries[i].bp);
		}
	}
	return Rect.isEqual(&tempRect);
}

