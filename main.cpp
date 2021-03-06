/*
 * =============================================================================
 * HR+ Tree
 *
 * This is a implementation of Efficient Historical R-trees [SSDBM'01]
 *
 * HR-tree keeps an R-tree for each time stamp in history, but allow
 * consecutive trees to share branches when the underlying objects do not change
 *
 *
 * =============================================================================
 */
#include "RootTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <set>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::atoi;
using std::set;
using std::string;

static double timediff( timeval start, timeval end ) {
	return ((end.tv_sec-start.tv_sec)*1000000u + end.tv_usec-start.tv_usec)/1.e6;
}

int main(int argc, char *argv[]){

	// For time measurement
	double tInsert=0, tDelete=0, tSearchStamp=0, tSearchIntv=0;
	struct timeval ts, te;

	if(argc !=2) {
		cout<<"Error : No input\n";
		exit(1);
	}
	char buf[1024];
	FILE *fin = fopen( argv[1], "r" );
	sprintf( buf, "%s", argv[1] );
	sprintf( buf, "result.out" );
	FILE *fout = fopen( buf, "w" );
	/*

	char buf[1024];
	FILE *fin = fopen( "input_3", "r" );
	//FILE *fin = fopen( "input_3D", "r" );
	FILE *fout = fopen( "result.out", "w" );
	*/
	int nObj;
	fscanf( fin, "%d", &nObj );

	set<int> ans;
	RootTable *RT = new RootTable(); //RT->numRoot = 1 in the beginning
	RT->Root[0]->bp[5]=DBL_MAX; // First root is alive

	double key[6];  /// X_min, Y_min, X_max, Y_max, t_start, t_end
	int id; // Data
	int globalTic  = 0;

	for ( int i=0; i<4; ++i ) fscanf( fin, "%s", buf );
	while ( fscanf( fin, "%s", buf ) != EOF ) {
		/*
		 * Insertion
		 * Key = [x_min y_min x_max y_max t_start t_end=*] (double array)
		 * Data = Object_ID (integer)
		 */
		if ( strcmp( buf, "Insert" ) == 0 ) {
			fscanf( fin, "%d", &id );
			for ( int i=0; i<4; ++i ) {
				fscanf( fin, "%s", buf );
				key[i] = atof(buf);
			}
			key[5] = DBL_MAX;

			gettimeofday(&ts, NULL);
			RT->CommandInsert(key, id, sizeof(int));
			gettimeofday(&te, NULL);
			tInsert += timediff(ts,te);
			//CommandInsert(key, item_id, sizeof(int), RT);
			//cout<< id << " is Inserted"<<endl;
		}

		/*
		 * Deletion
		 * Key = [x_min y_min x_max y_max t_start t_end = t_start]
		 * Data = Object_ID
		 */
		else if ( strcmp( buf, "Delete" ) == 0 ) {
			//Debug
			//cout<< id << " is Deleting at "<<key[4]<<endl;
			if(isPrintDelete)
				RT->CommandViewLast();

			fscanf( fin, "%d", &id );
			for ( int i=0; i<4; ++i ) {
				fscanf( fin, "%s", buf );
				key[i] = atof(buf);
			}
			gettimeofday(&ts, NULL);
			if (! RT->CommandDelete( key, id, sizeof(int))){
				//CommandDump(RT->Root[RT->numRoot-2]);
				//break;
				cout<<"Deletion failed with"<<id <<endl;
			}
			gettimeofday(&te, NULL);
			tDelete+= timediff(ts,te);
			//Debug
			if(isPrintDelete){
				cout<< id << " is Deleted at "<<key[4]<<endl;
				RT->CommandViewLast();
			}
		}

		else if (strcmp( buf, "Search" ) == 0 ) {
			for ( int i=0; i<4; ++i ) {
				fscanf( fin, "%s", buf );
				key[i] = atof(buf);
			}
			ans.clear();
			fscanf( fin, "%s", buf );
			key[4] = atoi(buf);

			fscanf( fin, "%s", buf );
			key[5] = atoi(buf); // Time Interval Query


			// Determine which Query type is.
			int areaQueryType = 0, timeQueryType = 0;
			fscanf( fin, "%s", buf );
			if(	strcmp(buf,"sOVERLAP")==0 )
				areaQueryType = 0;
			else if( strcmp(buf,"sCONTAIN") ==0 )
				areaQueryType = 1;

			fscanf( fin, "%s", buf );
			if (strcmp(buf, "tOVERLAP") == 0)
				timeQueryType = 0;
			else if (strcmp(buf, "tCONTAIN") == 0){
				if(key[4]+1 != key[5])
					timeQueryType = 1;

				if(key[5] > globalTic )
					key[5] = DBL_MAX;
			}

			gettimeofday(&ts, NULL);
			RT->CommandSearch(key, id, &ans, areaQueryType, timeQueryType);
			gettimeofday(&te, NULL);

			if(key[4]+1 ==key[5])
				tSearchStamp += timediff(ts,te);
			else
				tSearchIntv += timediff(ts,te);


			// Write a result
			for ( set<int>::iterator itr = ans.begin(); itr != ans.end(); ++itr ) {
				fprintf( fout, "%d ", *itr );
			}
			fprintf( fout, "\n" );
		}
		else if ( strcmp( buf, "Time" ) == 0 ) {
			fscanf( fin, "%s", buf );
			key[4] =double (atoi(buf));
			globalTic = (int) key[4];
		}
		else if ( strcmp( buf, "Stop" ) == 0 ) {
			break;
		}
		if (isVerifyTree) {
			if (!RT->CommandVerifyTree((int) key[4])) {
				cout << "Verification Failed" << endl;
				RT->CommandViewLast();
				break;
			}
		}
	}
	//RT->CommandView();
	if (isPrintRootInterval) {
		for (int i = 0; i < RT->numRoot; i++)
			cout << "[" << RT->Root[i]->bp[4] << " , " << RT->Root[i]->bp[5]<< "]" << endl;
	}


	cout<<"     "<<tInsert<<"     "<<tDelete<<"     "<<tSearchStamp<<"     "<<tSearchIntv<<endl;

	fclose(fin);	fclose(fout);
	return 0;
}
