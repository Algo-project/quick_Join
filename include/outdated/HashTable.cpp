#include "HashTable.h"
#include <algorithm>

#define COLLISION -2
#define NOT_FOUND -1
/* IMPLEMENTATIONS */
template<typename K,typename V,
		class HF1, class HF2>
ssize_t
HashTable<K,V,HF1,HF2>::getQueryInd(K key)
{
	size_t bkt = this->hf1(key);
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

	fprintf(stderr,"HERE!\n");
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

				auto ind = this->pbitmap->getIndex(bkt);
				for(size_t i = 0;i<this->THRESHOLD;i++)
				{
					size_t index = this->pbitmap->getIndex(bkt+i);
					if(!isset[index])
					{
						isset[index] = true;
						this->array[index] = 
						(__KVpair){key,val};
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
	auto ind = this->getQueryInd(key);
	if(ind == NOT_FOUND) 
		return std::make_pair(NULL,NULL);

	auto pair = this->oft.equal_range(key);
	ret.resize(std::distance(pair.first,pair.second));
	std::copy(pair.first,pair.second,ret.begin());	

	if(ind >= 0)
		ret.push_back(this->array[ind].value);
	return ret;
}
