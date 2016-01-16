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

#include "Command.h"
#include "RootTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
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
using std::ifstream;
using std::ofstream;
using std::string;
using std::atoi;
using std::set;
using std::string;

int main(int argc, char *argv[]){

	/*
	if(argc !=2) {
		cout<<"Error : No input\n";
		exit(1);
	}
	char buf[1024];
	FILE *fin = fopen( argv[1], "r" );
	sprintf( buf, "%s", argv[1] );
	sprintf( buf, "result.out" );
	FILE *fout = fopen( buf, "w" );
	*/

	char buf[1024];
	FILE *fin = fopen( "input_3", "r" );
	//FILE *fin = fopen( "input_3D", "r" );
	FILE *fout = fopen( "result.out", "w" );

	int nObj;
	fscanf( fin, "%d", &nObj );

	set<int> ans;
	RootTable *RT = new RootTable(); //RT->numRoot = 1 in the beginning
	RT->Root[0]->bp[5]=DBL_MAX; // First root is alive

	double key[6];  /// X_min, Y_min, X_max, Y_max, t_start, t_end
	int id; // Data

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
			CommandInsert(key, id, sizeof(int), RT);


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
				CommandDump(RT->Root[RT->numRoot-1]);

			fscanf( fin, "%d", &id );
			for ( int i=0; i<4; ++i ) {
				fscanf( fin, "%s", buf );
				key[i] = atof(buf);
			}
			if (! CommandDelete( key, id, sizeof(int), RT)){
				//CommandDump(RT->Root[RT->numRoot-2]);
				//break;
				cout<<"Deletion failed with"<<id <<endl;
			}

			//Debug
			if(isPrintDelete){
				cout<< id << " is Deleted at "<<key[4]<<endl;
				CommandDump(RT->Root[RT->numRoot-1]);
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
			fscanf( fin, "%s", buf ); // te = ts + 1
			key[5] = key[4]; // Time stamp query

			CommandSearch(RT, key, id, &ans, 0, 0);

			// Write a result
			for ( set<int>::iterator itr = ans.begin(); itr != ans.end(); ++itr ) {
				fprintf( fout, "%d ", *itr );
			}
			fprintf( fout, "\n" );
		}
		else if ( strcmp( buf, "Time" ) == 0 ) {
			fscanf( fin, "%s", buf );
			key[4] =double (atoi(buf));
		}
		else if ( strcmp( buf, "Stop" ) == 0 ) {
			break;
		}
		if (isVerifyTree) {
			if (!CommandVerifyTree(RT, (int) key[4])) {
				cout << "Verification Failed" << endl;
				CommandDump(RT->Root[RT->numRoot-1]);
				//CommandView(RT);
				break;
			}
		}
	}

	if (isPrintRootInterval) {
		for (int i = 0; i < RT->numRoot; i++)
			cout << "[" << RT->Root[i]->bp[4] << " , " << RT->Root[i]->bp[5]<< "]" << endl;
	}

	cout<<"Done!"<<endl;

	fclose(fin);	fclose(fout);
	return 0;
}
