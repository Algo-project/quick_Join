/**
 * HashTable.h
 */

#pragma once
#include <stdint.h>
#include <cassert>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <map>
#include <unordered_map>
#include <iostream>
#include "tbb/parallel_for.h"
#include "tbb/parallel_for_each.h"
#include "tbb/parallel_do.h"
#include "tbb/spin_mutex.h"
#include "LinkedList.h"
class Bitmap
{
	/* 0-----31 32----64
	 * prefix	bitset
	 */	
	private:
		uint64_t *p;
		size_t s,total;
		void __set(size_t size){
			total = 0;
			s=size;
			auto arrsize = getArraySize();
			p=new uint64_t[arrsize];
			memset(p,0,arrsize*sizeof(*p));
		}

		size_t getArraySize()
		{
			return s/32+1;
		}

		size_t __getOff(size_t off)
		{
			return off+32;
		}

	public:
		Bitmap(size_t size)
		{
			__set(size);
		}

		size_t size(){
			return s;
		}

		uint64_t *pointer(){
			return p;
		}

		/* return true if it is set */
		bool test(size_t bkt)
		{
			//fprintf(stderr,"testing bkt %ld\n",bkt);
			auto off = bkt % 32,
			     ind = bkt / 32;
			return p[ind] & (1ull << (__getOff(off)));
		}

		/* return true if 0->1 */
		bool set(size_t bkt)
		{
			if(test(bkt)) return false;
			//fprintf(stderr,"seting bkt %ld\n",bkt);
			auto off = bkt % 32,
			     ind = bkt / 32;
			p[ind] |= (1ull <<(__getOff(off)));
			++total;
			return true;
		}

		void fillPrefix()
		{
			size_t arrsize = this->getArraySize();
			int pre = 0;
			for(size_t i=0;i<arrsize;i++)
			{
				p[i] |= pre;
				pre += __builtin_popcountll(p[i] >> 32);
			}
		}

		size_t getTotal(){return total;}

		size_t getIndex(size_t bkt)
		{
			assert(test(bkt));
			auto off = bkt % 32,
			     ind = bkt / 32;
			auto prefix = p[ind] & 0xffffffff;
			auto aa = p[ind] >> 32;
			auto mask = ~(-1<<bkt);
			aa &= mask;
			auto append = __builtin_popcountll(aa);
			return prefix + append;
		}

		~Bitmap()
		{
			delete[] p;
		}

};

template <typename K, typename V, 
	 class HashFunc1, class HashFunc2>
	 class HashTable
{
	private:
		typedef struct
		{
			K key;
			V value;
		}__KVpair;
		typedef std::unordered_multimap<K,V,HashFunc2> __OFT;

	private:
		HashFunc1 hf1;
		const static size_t THRESHOLD = 2;
		Bitmap *pbitmap;
		std::vector<__KVpair> array;
		__OFT oft;

	private:
		ssize_t getQueryInd(K key);

	public:
		HashTable() = default;

		~HashTable()
		{
			if(pbitmap) delete pbitmap;
		}

		/* build */
		template <typename _ForwardIt, typename _GK,
			 typename _GV>
				 void	build(_ForwardIt begin, _ForwardIt end, size_t size,
						 _GK getKey, _GV getVal);

		/* query */
		std::vector<V>
			query_range(K key);		
};





/* IMPLEMENTATIONS */
#include <algorithm>

#define COLLISION -2
#define NOT_FOUND -1
/* IMPLEMENTATIONS */

template<typename K,typename V,
	class HF1, class HF2>
	ssize_t
HashTable<K,V,HF1,HF2>::getQueryInd(K key)
{
	size_t bkt = this->hf1(key) % pbitmap->size();
	if(!this->pbitmap->test(bkt))
		return NOT_FOUND;
	for(size_t i=bkt;i<bkt+THRESHOLD;i++)
	{
		if(this->pbitmap->test(bkt))
		{
			auto ind = this->pbitmap->getIndex(bkt);
			__KVpair kvp = this->array[ind];
			if(kvp.key == key)
				return ind;
		}
	}
	return COLLISION;
}


template<typename K, typename V,
	class HF1, class HF2>
	template<typename T,typename _GK, typename _GV>
	void
HashTable<K,V,HF1,HF2>::build(T begin, T end, size_t size,
		_GK getKey, _GV getVal)
{
	size *= 8;
	this->pbitmap = new Bitmap(size);
	std::for_each(begin,end,
			[size,this,&getKey,&getVal](decltype(*begin) _elem)
			{

			auto pb = this->pbitmap;
			auto& hf = this->hf1;
			K key = getKey(_elem);
			auto bkt = hf(key) % size;

			for(int i=0;i<this->THRESHOLD;++i)
			{
			if(pb->set(bkt+i))
			return;
			}

			V value = getVal(_elem);
			auto &oft = this->oft;
			oft.insert(std::make_pair(key,value));
			});

	this->pbitmap->fillPrefix();

	size_t total = this->pbitmap->getTotal();
	this->array.resize(total);
	bool *isset = new bool[total];
	memset(isset, false, total);

	std::for_each(begin,end,
			[size,this,&getKey,&getVal,isset](decltype(*begin) _elem)
			{
			K key = getKey(_elem);
			V val = getVal(_elem);
			auto & hf = this->hf1;
			auto bkt = hf(key) % size;

			for(size_t i = 0;i<this->THRESHOLD;i++)
			{
			size_t index = this->pbitmap->getIndex(bkt+i);
			if(!isset[index])
			{
			isset[index] = true;
			this->array[index] = 
			(__KVpair){key,val};
			return;
			}
			}
			});
}



template<typename K,typename V,
	class HF1, class HF2>
	std::vector<V>
HashTable<K,V,HF1,HF2>::query_range(K key)
{
	std::vector<V> ret;
	size_t bkt = this->hf1(key) % pbitmap->size();
	for(size_t i=bkt;i<bkt+THRESHOLD;i++)
	{
		if(this->pbitmap->test(i))
		{
			auto ind = this->pbitmap->getIndex(i);
			__KVpair kvp = this->array[ind];
			if(kvp.key == key)
				ret.push_back(kvp.value);
		}
	}

	auto pair = this->oft.equal_range(key);
	//	ret.resize(std::distance(pair.first,pair.second));
	for(auto i = pair.first;i!=pair.second;i++)
	{

		ret.push_back(i->second);
	}

	return ret;
}
template <typename HT>
class PartitionHashTable{
	private:
		size_t _numOfHT;
		std::vector<HT> _pht;
		size_t getPartitionIndex(uint64_t key){
			return (key%_numOfHT);
		}
		const static size_t PAGESIZE=(1ULL<<22);	
	public:
		PartitionHashTable()=default;
		template<typename It, typename GetK,typename GetV>
			void build(It begin,It end,
					size_t size,
					GetK getk,GetV getv){
				//partition
				_numOfHT=((size_t)(size*sizeof(uint64_t)/PAGESIZE)+1);			
				std::map<size_t,std::vector<Node>> partition;
				std::map<size_t,tbb::spin_mutex> partitionMutex;
				tbb::parallel_for_each(begin,end,[&](auto node){
						size_t index=this->getPartitionIndex(getk(node));
						partitionMutex[index].lock();
						partition[index].push_back(node);
						partitionMutex[index].unlock();
						});
				//build
				_pht.resize(_numOfHT);
				tbb::parallel_for_each(partition.begin(),partition.end(),
						[&](auto ent)
						{
							auto index = ent.first;
							auto vv = ent.second;
							_pht[index].build(vv.begin(),vv.end(),vv.size(),
									getk,getv);

						});
			}
			
		decltype(_pht.begin()->query_range(0))
			query_range(uint64_t key)
			{
				auto index = key % _numOfHT;
				auto & ht = _pht[index];
				return ht.query_range(key);
			}

};
