#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "L1Cache.h"
#include "Block.h"
#include "L2Cache.h"

using namespace std;

int Blocksize = 0;
int L1Size = 0;
int L1Assoc = 0;
int L2Size = 0;
int L2Assoc = 0;
int L2DataTags = 0;
int L2AddrTags = 0;
char* tracefile;

int L1reads = 0;
int L1readmisses = 0;
int L1writes = 0;
int L1writemisses;
float L1MissRate = 0.0;
int L1writebacks = 0;
int totalmemtraffic = 0;
int TOTALBITS = 32;
int totalcount = 0;
bool L2inuse;

int L2reads = 0;
int L2readmisses = 0;
int L2writes = 0;
int L2writemisses = 0;
int L2sectormisses = 0;
int L2blockmisses = 0;
float L2missrate = 0.0000;
int L2writebacks = 0;


//take command line args and save them into global variables
void initVals(char** args){
		Blocksize = atoi(args[1]);
                L1Size = atoi(args[2]);
                L1Assoc = atoi(args[3]);
                L2Size = atoi(args[4]);
                L2Assoc = atoi(args[5]);
                L2DataTags = atoi(args[6]);
                L2AddrTags = atoi(args[7]);
                tracefile = args[8];
}

// read the trace files for commands
void readfile(char *file, L1Cache L1, L2Cache L2){

	char readstore;
	string address;
	string line;
	unsigned int hexaddress;
	unsigned int tag;
	unsigned int index;
//	unsigned int offset; 

	ifstream infile(file);
	
	while(infile >> readstore >> address){
		// read file for hex address
		stringstream ss;
		ss << address;
		ss >> hex >> hexaddress;
                tag = hexaddress>>(L1.offsetsize + L1.indexsize);
		if(L1.numsets > 1){	       
        	       	index = (hexaddress<<L1.tagsize)>>(L1.tagsize+L1.offsetsize);
		}
		else{// there is only 1 est, we know the index is 0
			index = 0;
		}
	//	offset = (hexaddress<<(TOTALBITS-L1.offsetsize))>>(TOTALBITS-L1.offsetsize);
 		totalcount++;
		
		//perform read	              
		if(readstore == 'r'){
			L1.readcache(tag, index, L2, hexaddress);
		
		}//perform write
		else if(readstore == 'w'){

			L1.writecache(tag, index, L2, hexaddress);
		}
		else{//debugger
			printf("strange error\n");
		}

	}
}

void printAll(L1Cache L1, L2Cache L2){
	//print all according to validation run txt
	printf("  ===== Simulator configuration =====\n");
	printf("  BLOCKSIZE:                        %d\n", L1.blocksize);
	printf("  L1_SIZE:                          %d\n", L1.size);
	printf("  L1_ASSOC:                         %d\n", L1.assoc);
	printf("  L2_SIZE:                          %d\n", L2.size);
	printf("  L2_ASSOC:                         %d\n", L2.assoc);
	printf("  L2_DATA_BLOCKS:                   %d\n", L2.datablocks);
	printf("  L2_ADDRESS_TAGS:                  %d\n", L2.addrtags);
	printf("  trace_file:                       %s\n\n", tracefile);
	printf("===== L1 contents =====\n");
	for(int i = 0; i < L1.numsets; i++){

		printf("set	%d:", i);
		for(int k = 0; k < L1.assoc; k++){
		
			int column = L1.LRU[i][k];
			char dirtychar;
			if(L1.block[i][column].isDirty){
				dirtychar = 'D';
			}
			else{
				dirtychar = 'N';
			}
			printf("	%x %c	||", L1.block[i][column].tag, dirtychar);  

		}
			printf("\n");
		
	}	
	if(L2inuse){
	printf("\n");
	printf("===== L2 contents =====\n");
	 for(int i = 0; i < L2.numsets; i++){

                printf("set     %d:", i);
                for(int k = 0; k < L2.assoc; k++){

                        int column = L2.LRU[i][k];
                        char dirtychar;
                        if(L2.block[i][column].isDirty){
                                dirtychar = 'D';
                        }
                        else{
                                dirtychar = 'N';
                        }
                        printf("        %x %c   ||", L2.block[i][column].tag, dirtychar);

                }
                        printf("\n");

        }

	}	
	printf("\n");
	printf("===== Simulation Results =====\n");
	printf("a. number of L1 reads:			%d\n", L1reads);
	printf("b. number of L1 read misses:		%d\n", L1readmisses);
	printf("c. number of L1 writes:			%d\n", L1writes);
	printf("d. number of L1 write misses:		%d\n", L1writemisses);
	L1MissRate = ((float)L1readmisses+(float)L1writemisses)/((float)L1writes+(float)L1reads);
//	printf("e. L1 miss rate:			%f\n", L1MissRate);
	cout << fixed;
	cout << "e. L1 miss rate:			" <<  setprecision(4) << L1MissRate << "\n";
	printf("f. number of writebacks from L1 memory:	%d\n", L1writebacks);
	if(!L2inuse){
		totalmemtraffic = L1readmisses+L1writemisses;
		 printf("g. total memory traffic:                %d\n", totalmemtraffic);
	}
	if(L2inuse){
	printf("g. number of L2 reads:			%d\n", L2reads);
	printf("h. number of L2 read misses:		%d\n", L2readmisses);
	printf("i. number of L2 writes:			%d\n", L2writes);
	printf("j. number of L2 write misses:		%d\n", L2writemisses);
	L2missrate = ((float)L2readmisses/(float)L2reads);
	cout << fixed;
	cout << "k. L2 miss rate:                 " << setprecision(4) << L2missrate << "\n";
	printf("l. number of writebacks from L2 memory:       %d\n", L2writebacks);
	totalmemtraffic = L2readmisses + L2writemisses + L2writebacks;

	printf("m. total memory traffic:		%d\n", totalmemtraffic);
	}
	return;		

}

void printbasics(L1Cache L1, L2Cache L2){

	printf("Blocksize: %d\n", L1.blocksize);
	printf("L1 szie: %d\n", L1.size);
	printf("L1 Assoc: %d\n", L1.assoc);
	printf("L2 size: %d\n", L2.size);
	printf("l2 assoc: %d\n", L2.assoc);
	printf("L2 datablcok size: %d\n", L2.datablocks);
	printf("L2 addrtags: %d\n", L2.addrtags);


}
int main(int argc, char** argv){

	//set command line arguments for our variable
	if(argc != 9){
		//input is not correct, stop program
		printf("Input args are not correct\n");
		return 0;
	}
	else{
		//get vals from input line
		initVals(argv);
	}


	//initialize cache l1
	L1Cache L1 = L1Cache(L1Size, Blocksize, L1Assoc);

	if(L2Size==0 || L2Assoc==0){
                L2inuse = false;
        }
        else{
                L2inuse = true;
        }

	
	L2Cache L2 = L2Cache(L2Size, Blocksize, L2Assoc, L2DataTags, L2AddrTags);		
	
//	printbasics(L1, L2);
	
	readfile(tracefile, L1, L2);		

//	printf("%d reads %d read misses and %d writes and %d write misses\n", L1reads, L1readmisses, L1writes, L1writemisses);
 //	printf("%d reads %d read misses  and %d writes %d write misses\n", L2reads, L2readmisses, L2writes, L2writemisses);	
	printAll(L1,L2);	

	return 0;


}


