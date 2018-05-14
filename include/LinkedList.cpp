#include "LinkedList.h"
#include "Database.h"
#include "HashTable.h"

/* HELPERS */

/*hash function -- uint64_t*/
template<int l,typename T>
struct myHash
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

template<typename T>
void contactVec(std::vector<T> & target, const std::vector<T> & src)
{
	for(auto i : src)
		target.push_back(i);
};
template<typename T>
std::vector<T> connectVec(std::vector<T> first,
						std::vector<T> second)
{
	for(auto i:second) first.push_back(i);
	return first;
}

/**
 * return ture if fu he tiao jian
 */ 
bool 
LinkedList::testFilter(const Node & nd, const QFilter & qf,Query & q)
{
	auto rid = q.getRelationID(qf.sel);	
	auto pos = this->rePos(rid);
	auto row = nd.row[pos];
	auto data = this->_pdb->getRelation(rid)->getEle(qf.sel.second, row);
	auto constant = qf.constant;
	switch(qf.comparison)
	{
		case '>': return data>constant;
		case '<': return data<constant;
		case '=': return data==constant;
	}
}

/* IMPLEMENTATIONS */


LinkedList::__K_TYPE 
LinkedList::_GetK::operator()(Node & n)
{
	auto pos = pl->rePos(rid);
	int row = n.row[pos];
	return pl->_pdb->getRelation(rid)->getEle(cid,row);
}

LinkedList::__V_TYPE 
LinkedList::_GetV::operator()(Node & n)
{
	return n;
}

LinkedList::LinkedList(const Relation & r, RelationID rid, Database * _p)
{
	this->addNewRelation(rid);
	for(size_t i = 0;i<r.getSize();++i)
	{
		this->emplace_back(Node());
		this->back().row.push_back(i);
	}
	this->__num_relations = 1;
	this->_pdb = _p;
}

bool
LinkedList::addNewRelation(RelationID reid)
{
	bool ret = std::count_if(reids.begin(),reids.end(),
	     [reid](auto it){return it == reid;}
	);
	if(ret == false) reids.push_back(reid);
	return ret;
}

size_t 
LinkedList::rePos(RelationID rid){
	for(uint64_t i=0;i<reids.size();i++)
		if(reids[i] == rid) return i;
	return -1;
}

void 
LinkedList::applyFilter(std::vector<QFilter> & qfs, Query & q)
{
	auto end = std::remove_if(this->begin(),this->end(),
			[&](auto node)
			{
				bool flag = true;
				for(auto filter : qfs)
				{
					flag &= this->testFilter(node,filter,q);
				}
				return !flag;
				
			}
			);
	auto dis = end - this->begin();
	this->resize(dis);

}

uint64_t 
LinkedList::addSum(RelationID rid, int cid)
{
	auto pos = this->rePos(rid);
	uint64_t sum = 0;
	for(auto node : *this)
	{
		int row = node.row[pos];
		sum += this->_pdb->getRelation(rid)->getEle(cid,row);
	}
	return sum;
}

void
LinkedList::join(LinkedList * other, QJoin & qj, Query & q)
{
	/* add relation for this */
	contactVec(this->reids,other->reids);

	if(this->size() == 0 || other->size() == 0)
	{
		this->resize(0);
		return;
	}

	/* initialize */
	_GetK getKey(q.getRelationID(qj.right),qj.right.second,other);
	_GetV getVal;
	/* overflow table */
	std::vector<Node> overflow;

	/* build patition HT for other */
	size_t __num_partitions=(size_t)(other->size()*sizeof(uint64_t)/page_size)+1;
	typedef HashTable<__K_TYPE,__V_TYPE,
		myHash<30,__K_TYPE>,std::hash<__K_TYPE>> HT;
//	HT table;
	PartitionHashTable<HT> table;
	table.build(other->begin(),other->end(),other->size(),
				getKey,getVal);


	/* std::remove_if */
	_GetK getThisKey(q.getRelationID(qj.left),qj.left.second,this);
	auto end = std::remove_if(this->begin(),this->end(),
			[&](auto & node)
			{
				auto key = getThisKey(node);
				std::vector<Node> vv = table.query_range(key);
				if(vv.size() != 0)
				{
					auto nnode = node;
					contactVec(node.row, vv.back().row);
					vv.pop_back();
					for(auto v:vv)
					{
						auto temp = nnode;
						contactVec(temp.row, v.row);
						overflow.push_back(temp);
					}
					return false;
				}
				else return true;
			});
	auto dis = end - this->begin();
	this->resize(dis);
	

	/* push overflow table */
	contactVec(*this, overflow);

}

void
LinkedList::selfJoin(QJoin & qj, Query & q)
{

	_GetK leftKey(q.getRelationID(qj.left),qj.left.second,this),
		  rightKey(q.getRelationID(qj.right),qj.right.second,this);

	auto end = std::remove_if(this->begin(),this->end(),
			[&](auto node)
			{
				auto lk = leftKey(node);
				auto rk = rightKey(node);
				return lk != rk;
			});

	auto dis = end  - this->begin();
	this->resize(dis);
}
