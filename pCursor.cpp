/*
 * pCursor.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: hanjunkoo
 */

#include "pCursor.h"


using namespace std;
pCursor::pCursor() {
	// TODO Auto-generated constructor stub
	size = 0;
}

pCursor::~pCursor() {
	// TODO Auto-generated destructor stub
}


bool pCursor::InsertNode(HNode *nNode){
	for (int i =0;i<nNode->numEntry;i++){
		memcpy(&entries[size], &nNode->entries[i],sizeof(Entry));
		size++;
	}
	return true;
}

bool pCursor::InsertEntry(HNode *nNode){
	memcpy( entries[size].bp, nNode->bp,klen);
	entries[size].child = nNode;
	size++;
	return true;
}

int pCursor::numAliveEntry(){
	int num = 0;
	for (int i= 0; i<size; i++){
		if(entries[i].bp[5] ==DBL_MAX)
			num++;
	}
	return num;
}

void pCursor::printCursor(){
	for (int j = 0; j<size;j++){
		for (int i = 0; i < dim * 2; i++) {
			if (entries[j].bp[i] == DBL_MAX)
				cout << "* ";
			else
				cout << entries[j].bp[i] << " ";
		}
		cout<<endl;
	}
}
