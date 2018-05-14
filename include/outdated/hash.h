
/**
 * hash.hpp
 * 
 * OFFER THE FUNCTION USED TO HASH
 *
 * @author haonan 
 */

/*includes and macros*/
#pragma once 
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <unordered_map>
#include "Relation.h"
#include "Query.h"
#include "Database.h"
using namespace std;

uint64_t mask=((uint64_t)1<<63);
/* Function */
void initDatabase(Database *dataBase);
void hash_alpha(QJoin joinInstruction);
UCE hash_function(UCE elements);
size_t popCount(size_t elements);
/*data structure*/
class Bitmap{
	private:
		uint64_t *p;
		size_t s;
	public:
		void set(size_t size){
			s=size;
			p=new uint64_t[size];
		}
		size_t size(){
			return s;
		}
		uint64_t *pointer(){
			return p;
		}
		Bitmap(size_t ss){
			p=new uint64_t[(size_t)(ss/32)+1];
			s=ss;
		}
};

template <class p>
//p need a 
class Hashtable{
	private:
                size_t threshold=2;
		Bitmap bitmap;
		vector<pair<uint64_t,p>> array;
		std::unordered_map<UCE,p> overflowTable;
		template <class ForwardIterator>
		void scan1(p elements){
			uint64_t *bitmapWordPointer=bitmap.pointer()+(uint64_t)(bkt)/32;
			uint64_t offset=bkt%32;
			for(unsigned i=0;i<threshold;i++){
				bool set=((*bitmapWordPointer)>>(64-(offset+threshold)))&1;
				if(!set){
					(*bitmapWordPointer)&=(((uint64_t)1)<<(64-offset-threshold));
				}
			}
			overflowTable[p]=
		}
		//???????
		void scan2(){
			uint64_t *bitmapWordPointer=bitmap.pointer()+(uint64_t)(bkt)/32;
                        uint64_t offset=bkt%32; 
			size_t prefixPopcount=((uint64_t)(~0))&word;
                        size_t bitsUptoBkt=word&~((~0)>>(bkt%32));
                        size_t pos=prefixPopcount+popCount(bitsUptoBkt);
			
		}
	public:
		template <class ForwardIterator>
		void inline build(ForwardIterator begin,ForwardIterator end,size_t size){
			bitmap.set(((size/8)+1));
			for_each(begin,end,scan1);
		}
		inline p *query(UCE elements){
			UCE bkt=hash_function(elements)%(bitmap.size());
		        uint64_t *bitmapWordPointer,word;
			bool set=false;
			{

				bitmapWordPointer=bitmap.pointer()+(uint64_t)(bkt)/32;
				uint64_t offset=bkt%32;  
				word=*bitmapWordPointer;
				if(((word<<offset)>>63)&1) set=true;
			}
			if(!set)
				return NULL;
			size_t prefixPopcount=((uint64_t)(~0))&word;
			size_t bitsUptoBkt=word&~((~0)>>(bkt%32));
			size_t pos=prefixPopcount+popCount(bitsUptoBkt);
			for(unsigned i=0;i<threshold;i++){
				//?????
				if(array[pos+i]==elements) 
					return &array[pos+i];
			}
			//????
			return &overflowTable[elements];
		}
};
