#ifndef	L2CACHE_H
#define L2CACHE_H

#include "Block.h"
#include "L1Cache.h"

extern int TOTALBITS;
extern int L2reads;
extern int L2readmisses;
extern int L2writes;
extern int L2writemisses;
extern int L2sectormisses;
extern int blockmisses;
extern int L2writebacks;
extern bool L2inuse;

class L2Cache{

	public:
		int size;
		int blocksize;
		int assoc;
		int addrtags;
		int datablocks;
		int numsets;
		int tagsize;
		int indexsize;
		int offsetsize;
		Block **block;
		int **LRU;

	public:
		L2Cache(int SIZE, int BLOCKSIZE, int ASSOC, int DATABLOCKS, int ADDRTAGS);;
		void initLRU();
		int evictLRU(unsigned int index);
		void setLRU(unsigned int index, int column);	
		void readcache(unsigned int tag, unsigned int index);
		void writecache(unsigned int tag, unsigned int index);

};

L2Cache::L2Cache(int SIZE, int BLOCKSIZE, int ASSOC, int DATABLOCKS, int ADDRTAGS){

	 size = SIZE;
        blocksize = BLOCKSIZE;
        assoc = ASSOC;
        datablocks = DATABLOCKS;
	addrtags = ADDRTAGS;
	if(!L2inuse)
		return;	
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

void L2Cache::initLRU(){
	for(int i = 0; i < numsets; i++){
			for(int k = 0; k < assoc; k++){
				LRU[i][k] = k;
			}
		}

	}

	int L2Cache::evictLRU(unsigned int index){
		return LRU[index][assoc-1];
	}

	void L2Cache::setLRU(unsigned int index, int column){
		int temp = -1;
		for(int i = 0; i < assoc; i++){
			if(LRU[index][i] == column){
				temp = i;
				break;
			}

		}

		for(int j = temp; j > 0; j--){

			LRU[index][j] = LRU[index][j-1];
		}
		LRU[index][0] = column;
		return;	
	}

	void L2Cache::readcache(unsigned int tag, unsigned int index){

		L2reads++;

		int column = -1;
		for(int i = 0; i < assoc; i++){

			if(block[index][i].isValid == true && block[index][i].tag == tag){
				column = i;
				this->setLRU(index, column);
				return;
			}

		}
		L2readmisses++;
		for(int k = 0; k < assoc; k++){
			if(block[index][k].isValid == false){
				
				block[index][k].isDirty = false;
				block[index][k].isValid=true;
				block[index][k].tag = tag;
				this->setLRU(index, k);
				return;
			}
		
		}

		int evictColumn = evictLRU(index);
		if(block[index][evictColumn].isDirty){
			L2writebacks++;
		}
		block[index][evictColumn].tag = tag;
		block[index][evictColumn].isValid = true;
		block[index][evictColumn].isDirty = false;
		this->setLRU(index, evictColumn);	
		return;
	}

	void L2Cache::writecache(unsigned int tag, unsigned int index){
		L2writes++;
		for(int i = 0; i < assoc; i++){
			if(block[index][i].tag == tag){
				this->setLRU(index, i);
				block[index][i].isDirty = true;
				block[index][i].isValid = true;
				return;
			}
		}

		L2writemisses++;
		for(int j = 0; j < assoc; j++){

			if(block[index][j].isValid == false){

				block[index][j].tag = tag;
				block[index][j].isDirty=true;
				block[index][j].isValid = true;
				this->setLRU(index, j);
				return;	
			}
		
		}

		int evictColumn = evictLRU(index);
		if(block[index][evictColumn].isDirty == true){
			L2writebacks++;
		}
		block[index][evictColumn].tag = tag;	
		block[index][evictColumn].isDirty = true;
		this->setLRU(index, evictColumn);	
	return;	

}
#endif
