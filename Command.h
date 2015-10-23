/*
 * Command.h
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_

#include "Node.h"


int CommandInsert(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot);
int CommandDelete(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot);
int CommandSearch(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot);
void CommandNewRoot();
void CommandPrint(Node* node);
void CommandDump(Node* node);



#endif /* SRC_COMMAND_H_ */
