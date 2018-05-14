#include <iostream>
#include <algorithm>
#include <string>
#include "Relation.h"
#include "HashTable.h"
using namespace std;
int a[15] = {1,3,2,6,5,7,4,8,0,9,3,7,11,3,2};
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
int main(int argc,char * argv[])
{
	HashTable<int,int*,HF<30,int>,std::hash<uint64_t>> ht;
	
	HF<30,int> hf;
	ht.build(a,a+15,15,
			[](int a)->int{return a;},
			[](int &a){cout<<&a<<endl;return &a;}
			);
	auto v = ht.query_range(3);
	std::for_each(v.begin(),v.end(),
			[](auto i){
				std::cout<<i<<" "<<*(int*)i<<std::endl;}
			);
	return 0;
}
