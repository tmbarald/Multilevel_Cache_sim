#ifndef L1CACHE_h
#define L1CACHE_h

#include "Block.h"
#include "L2Cache.h"

extern int L1reads;
extern int L1readmisses;
extern int L1writes;
extern int L1writemisses;
extern int L1writebacks;
extern int TOTALBITS;
extern bool L2inuse;
extern int L2writebacks;

class L1Cache{
		//cache class that keeps track of sizes, sets, block matrix and LRU matrix
	public:
		int size;
		int blocksize;
		int assoc;
		int numsets;
		int tagsize;
		int indexsize;
		int offsetsize;
		Block **block;
		int **LRU;
	public://all methods 
		L1Cache(int SIZE, int BLOCKSIZE, int ASSOC);
		void readcache(unsigned int tag, unsigned int index, L2Cache L2, unsigned int hexaddress);
		void writecache(unsigned int tag, unsigned int index, L2Cache L2, unsigned int hexaddress);		
		void setLRU(unsigned int index, int column);				
		int evictLRU(unsigned int index);
		void initLRU();
};

void L1Cache::initLRU(){
	//initializes LRU can be set to anything
	for(int i = 0; i < numsets; i++){
		for(int k = 0; k < assoc; k++){
			LRU[i][k] = k;

			}
	
	}

}

int L1Cache::evictLRU(unsigned int index){
	//finds LRU block and sends it back
	return LRU[index][assoc-1];

}

void L1Cache::setLRU(unsigned int index, int column){
	//updates LRU
	int temp = -1;
	//finds column that was last read/wrote to
	for(int i = 0; i < assoc; i++){
		if(LRU[index][i] == column){
			temp = i;	
			break;	
		}
	}
	//shifts used row to front of array. shifts other vals back
	for(int j = temp; j > 0; j--){
		LRU[index][j] = LRU[index][j-1];
	}
	LRU[index][0] = column;
 

	return;
	
}
L1Cache::L1Cache(int SIZE, int BLOCKSIZE, int ASSOC){
	//initializes cache vals
	size = SIZE;
	blocksize = BLOCKSIZE;
	assoc = ASSOC;
	numsets = size/(blocksize*assoc);
	indexsize = log2(numsets);
	
	offsetsize = log2(blocksize);
	tagsize = TOTALBITS - indexsize - offsetsize;
	block = new Block*[numsets];
	for(int i = 0; i < numsets; i++){
		block[i] = new Block[assoc];
	}
	LRU = new int*[numsets];
	for(int i = 0; i < numsets; i++){
		LRU[i] = new int[assoc];
	}			

	this->initLRU();	
}

void L1Cache::readcache(unsigned int tag, unsigned int index, L2Cache L2, unsigned int hexaddress){



	L1reads++;	
		
	// reads for direct hit
	int column = -1;
        for(int i = 0; i < assoc; i++){

                if(block[index][i].isValid == true && block[index][i].tag == tag){
			column = i;
			
			this->setLRU(index, column);
			return; //found it all good
//		block[index][i].isDirty = false;
       

                }
        }
	unsigned int L2tag;
	unsigned int L2index;
       if(L2inuse){
		L2tag = hexaddress>>(L2.offsetsize + L2.indexsize);
		if(L2.numsets > 1){
			L2index = (hexaddress<<L2.tagsize)>>(L2.tagsize+L2.offsetsize);
		}
		else{
			L2index = 0;
		}
	}
	else{
		L2index = 0;
		L2tag = 0;	
	}	
				
	// read is not a hit, read miss up, check for empty blocks
	L1readmisses++;
	for(int k = 0; k<assoc; k++){	
		if(block[index][k].isValid == false){
			block[index][k].isDirty = false;
			block[index][k].isValid = true;
			block[index][k].tag = tag; 
			block[index][k].fulladdress=hexaddress;
			this->setLRU(index, k);
			if(L2inuse)
				L2.readcache(L2tag, L2index);
			return;
			//all good
		}

	}		
//	int dummyvar = L2.size;
	// No empty blocks, look for LRU block and evict
	int evictColumn = evictLRU(index);
	if(block[index][evictColumn].isDirty){
		L1writebacks++;
		if(L2inuse){
			unsigned int writebackaddress = block[index][evictColumn].fulladdress;
			unsigned int writebacktag = writebackaddress>>(L2.offsetsize + L2.indexsize);
			unsigned int writebackindex;
			if(L2.numsets>1){
				writebackindex = (writebackaddress<<L2.tagsize)>>(L2.tagsize+L2.offsetsize);
			}
			else{
				writebackindex = 0;
			}
			L2.writecache(writebacktag, writebackindex);
		//	L2writebacks++;
//			L2.readcache(L2tag, L2index);		
		}
		
	}
	if(L2inuse)
		L2.readcache(L2tag, L2index);

	block[index][evictColumn].tag=tag;
	block[index][evictColumn].isValid=true;
	block[index][evictColumn].isDirty=false;
	block[index][evictColumn].fulladdress = hexaddress;
	this->setLRU(index, evictColumn);
	return; 
}

void L1Cache::writecache(unsigned int tag, unsigned int index, L2Cache L2, unsigned int hexaddress){



	L1writes++;
	// check for write hit
	for(int i = 0 ; i < assoc; i++){

		if(block[index][i].tag==tag){
			this->setLRU(index, i);
			block[index][i].isDirty = true;
			block[index][i].isValid = true;
			//hit found all good
			return;
		}

	}
	//no hit found. write miss
	L1writemisses++;
	unsigned int L2tag;
	unsigned int L2index;

	 if(L2inuse){
                L2tag = hexaddress>>(L2.offsetsize + L2.indexsize);
                if(L2.numsets > 1){
                        L2index = (hexaddress<<L2.tagsize)>>(L2.tagsize+L2.offsetsize);
                }
                else{
                        L2index = 0;
                }
        }
        else{
                L2index = 0;
                L2tag = 0;
        }
	
	//check for unused block and enter tag data
	for(int j = 0; j < assoc; j++){

		if(block[index][j].isValid == false){

			block[index][j].tag = tag;
			block[index][j].isDirty=true;
			block[index][j].isValid = true;
			block[index][j].fulladdress = hexaddress;
			if(L2inuse)
				L2.readcache(L2tag, L2index);
			this->setLRU(index, j);
			return;	
		}
	
	}	
	
	//not empty block find LRU block and evict it	
	int evictColumn = evictLRU(index);
	if(block[index][evictColumn].isDirty == true){
		L1writebacks++;
		  if(L2inuse){
                        unsigned int writebackaddress = block[index][evictColumn].fulladdress;
                        unsigned int writebacktag = writebackaddress>>(L2.offsetsize + L2.indexsize);
                        unsigned int writebackindex;
                        if(L2.numsets>1){
                                writebackindex = (writebackaddress<<L2.tagsize)>>(L2.tagsize+L2.offsetsize);
                        }
                        else{
                                writebackindex = 0;
                        }
                        L2.writecache(writebacktag, writebackindex);
//			L2writebacks++;
//                      L2.readcache(L2tag, L2index);
		}

	}
	if(L2inuse)
		L2.readcache(L2tag, L2index);
	block[index][evictColumn].fulladdress = hexaddress;
	block[index][evictColumn].tag = tag;	
	block[index][evictColumn].isDirty = true;
	this->setLRU(index, evictColumn);	
	return;	
}

#endif
