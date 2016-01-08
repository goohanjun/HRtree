/*
 * Verify.h
 *
 *  Created on: Jan 3, 2016
 *      Author: hanjunkoo
 */


#ifndef SRC_VERIFY_H_
#define SRC_VERIFY_H_

#include "HNode.h"

bool _isMinMaxCorrect(HNode* HNode);
bool _isUnderflow(HNode* HNode);
bool _isAliveBPCorrectNode(HNode* HNode);
bool _isDeadBPCorrectNode(HNode* HNode, double tnow);
bool _isAliveBPCorrectEntry(Entry* Entry, double tnow);
bool _isDeadBPCorrectEntry(Entry* Entry, double tnow);


#endif /* SRC_VERIFY_H_ */
