#pragma once
#include "HashTable.h"
#include "Database.h"
#include "LinkedList.h"
#include "Query.h"
#include "tbb/atomic.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_for_each.h"
#include <map>
using namespace tbb;


const size_t page_size=(1ULL<<22);

template<int l,typename T>
struct HF
{
	typedef __int128 _I;
	static const _I MR = 0x1f08e0435fc22213+l*0xfe5577876fa335;
	uint64_t operator()(const T & t)
	{
		_I ans = MR * t;
		ans = ans * ans;
		ans >>= l;
		return (uint64_t)ans;
	}
};


LinkedList join(LinkedList l1,LinkedList l2);

typedef size_t RowID;
typedef HashTable<uint64_t,RowID,HF<30,int>,std::hash<uint64_t>> HT;
class PartitionHT:public HT{
	public:
		size_t id;
		PartitionHT(size_t i){
			id=i;
		}
};
