/*
 * HR_DEFS.h
 *
 * Save all definitions
 */
#ifndef SRC_HR_DEFS_H_
#define SRC_HR_DEFS_H_


//Parameters
#define PAGE_SIZE 600 //8192

#define dim 3 // [x, y, t]
#define klen 6*8 // 6*sizeof(double)
#define DBL_MAX 1.7976931348623158e+308 // Use it to show inserted object is not deleted.

#define MAX_DEPTH_SIZE 20 // Max depth of tree
#define MAX_ROOT 100 // Max number of root nodes
#define MAX_STACK_SIZE 20 // Max length of stack to save path from root to leaf

#define SVO_Ratio 0.8 // Strong Version Overflow Condition
#define UF_Ratio 0.35 // Underflow Condition
#define minSplitRatio 0.35


//For validation
#define isValidate 0

// Print Options
#define printOption 1 // Print all TODO
#define printTime 0 //TODO

//Insertion
#define isPrintPath 0 //Choose Subtree
#define isPrintOverflow 0 // TreatOverflow

//Split
#define isPrintSplit 0 //Key, Version, SVO Splits



//Print
#define isVerifyAnswer 0
#define isVerifyTree 0

#endif /* SRC_HR_DEFS_H_ */
