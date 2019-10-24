#ifndef BLOCK_H
#define BLOCK_H
//#include "L1Cache.h"
//#include "L2Cache.h"
class Block{
	// block class that holds dirty and valid bits and manages tags	
	public:
		bool isDirty;
		bool isValid;
		unsigned int tag;
		unsigned int fulladdress;

	public:
		Block();

};
 
Block::Block(){

	//initilizes block
	isDirty = false;
	isValid = false;
	tag = 0;
	fulladdress = 0;
}
#endif
