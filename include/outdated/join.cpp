#include "join.h"

uint64_t getTheNum(LinkedList l,LinkedList::iterator i,RelationID rid,size_t cid){
	RowID rowID=(*i).row[l.rePos(rid)];
	uint64_t num=(pdb->getRelation(rid)->getColumn(cid))[rowID];
	return num;
}
//l1 build hashtable,l2 probe
LinkedList join(QJoin q,LinkedList l1,LinkedList l2){
/*getTheOrder*/
	LinkedList *smallLinkedList,*bigLinkedList;
	QSelection smallSel,bigSel;
	if(l1.size()>l2.size())
	{
		smallLinkedList = &l2; smallSel=q.right;
		bigLinkedList=&l1; bigSel=q.left;
	}
	else
	{
		smallLinkedList=&l1; smallSel=q.left;
		bigLinkedList=&l2; bigSel=q.right;
	}
/*patition build*/ 
	/*patition the inner*/
	size_t __num_partitions=(size_t)((*smallLinkedList).size()*sizeof(uint64_t)/page_size)+1;
	std::map<size_t,std::vector<uint64_t>> partition;
	for(auto i=(*smallLinkedList).begin();i!=(*smallLinkedList).end();i++){
		uint64_t num=getTheNum((*smallLinkedList),i,smallSel.first,smallSel.second);
		partition[(num%__num_partitions)].push_back(num);
	}
	//partition HT build
	//hashtable store which and is not sure;
	std::vector<PartitionHT> v;//!!!!!!!!!
	for(size_t i=0;i<__num_partitions;i++) v.push_back(PartitionHT(i));
	tbb::parallel_for_each(v.begin(),v.end(),[&](PartitionHT h){
	h.build(partition[h.id].begin(),partition[h.id].end(),partition[h.id].size(),
     [](uint64_t a)->uint64_t{return a;},
     [](uint64_t &a){return &a;}
			);
	//???????
/*probe(nonpartition)*/
//!!!!!here we can use pthread to do a parallel but need do a comparision;	
	LinkedList result;
	for(auto i=(*bigLinkedList).begin();i!=(*bigLinkedList).end();i++){
		u_int64_t num=getTheNum((*bigLinkedList),i,bigSel.first,bigSel.second);
		std::vector<RowID> ans=v[(num%__num_partitions)].query_range(num);//???	
	}
		
}
