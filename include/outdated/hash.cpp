/**
 * hash.cpp
 * 
 * OFFER THE FUCTION USED TO HASH
 * 
 * @author haonan
 * 
 */
 
/*==========*/
/* INCLUDES */
/*==========*/
#include "hash.h"
using namespace std;

static Database * pdb;
const unsigned CACHE_SIZE=25*1024*1024;
const UCE h_i=25976;
const UCE hashConstant216=(1<<16);
const UCE hashConstant231=((UCE)(1<<31)-1);
/*===========*/
/* HASH PART */
/*===========*/


size_t popCount(size_t elements){
	size_t count = 0;
        while(elements!=0){
            elements=(elements-1)&elements;
            count++;
        }
	return count;  	
}
void initDatabase(Database *dataBase){
	pdb=dataBase;
}
UCE hash_function(UCE elements){
	UCE tempElements=elements%hashConstant216;
	return 	((tempElements+h_i)*tempElements)%hashConstant231;
}
/*This is the first version of hash,use almost no tricks*/
void hash_alpha(QJoin joinInstruction){
	auto leftColumn=pdb->getRelation(joinInstruction.left.first).getColumn(joinInstruction.left.second),
	     rightColumn=pdb->getRelation(joinInstruction.right.first).getColumn(joinInstruction.right.second);
	/*assinment ,which is the smaller column*/
	bool leftIsBigger=false;
	unsigned smallColumnSize;
	{
		unsigned leftSize=pdb->getRelation(joinInstruction.left.first).getSize(),
			 rightSize=pdb->getRelation(joinInstruction.right.first).getSize();
		auto smallColumn=leftColumn,bigColumn=rightColumn;
		if(leftSize>rightSize){
			smallColumn=rightColumn;
			bigColumn=leftColumn;
			leftIsBigger=true;
			smallColumnSize=rightSize;
		}
		else smallColumnSize=leftSize;
	}
	//here we need a function to create the hashtable
	unsigned sizeOfBitmap=8*smallColumnSize;
	//here need a walk through
	
	{
		bkt=hash_function()%sizeOfBitmap;
		


	}
	
}
void Query::execute(){
	/*Dealing with the Filter predicatates at first*/
	for(vector<QFilter>::iterator iter=filters.begin();iter!=filters.end();iter++){
		const auto filterNum=(*iter).constant;
		const auto sel=(*iter).sel;
		auto columnPointer=pdb->getRelation(sel.first).getColumn(sel.second);
		auto columnSize=pdb->getRelation(sel.first).getSize();
		for(unsigned i=0;i<)
		/*could have problem here*/
		
	}
	/**/
	for(vector<QJoin>::iterator iter=joins.begin();iter!=joins.end();iter++) 
		hash_alpha(*iter);

	return;
}
