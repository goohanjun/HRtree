/*
 * Extension.h
 *
 *  Created on: Sep 10, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_EXTENSION_H_
#define SRC_EXTENSION_H_

#include "Node.h"
#include "hrext.h"
#include <iostream>
#include <string.h>

using namespace std;



int Insert(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot);
int Delete(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot);
int Search(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot);


int _Path(Node* self, Node* paths[], int trace[], int& numTrace, double* key, int klen);
int _Consistent(double *bp, double *key, int klen);



#endif /* SRC_EXTENSION_H_ */
