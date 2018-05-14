#pragma once
#include <vector>
#include <list>
#include <cstdlib>
#include <map>
#include "Relation.h"
#include "Query.h"
const size_t page_size=(1ULL<<22);


class Database;

class Node
{
	public:
		std::vector<size_t> row;
		Node() = default;
		
};

class LinkedList:public std::vector<Node> 
{
	private:
		/*
		 * map relationID to index in node
		 * For exp : node is
		 * Node a:
		 *	row[0] row[1] row[2]
		 *	r0	   r1	  r3
		 * reids:
		 *	re[0]  re[1]  re[2]
		 *	0	   1	  3
		 */ 
		std::vector<RelationID> reids;
//		std::map<RelationID,size_t> reids;
		int __num_relations = 0;
		Database * _pdb;

	public:
		typedef uint64_t __K_TYPE;
		typedef Node  __V_TYPE;

		struct _GetK
		{
			private:
				int rid,cid;
				LinkedList * pl;
			public:
				_GetK(int r,int c,LinkedList *p):rid(r),cid(c),pl(p){}
				__K_TYPE operator()(Node & n);
		};

		struct _GetV
		{
			private:
			public:
				__V_TYPE operator()(Node & n);
		};
	private:
		bool testFilter(const Node & nd, const QFilter & qf, Query & q);
	
	public:
		/* constructors */
		LinkedList(const Relation & r, RelationID rid, Database * _p);
		
		/* addNewRelation :
		 *	returns true if reid doesn't exist in
		 *	reids set
		 */
		bool addNewRelation(RelationID reid);
		size_t rePos(RelationID rid);

		std::vector<RelationID> & getReids()
		{
			return this->reids;
		}
		
		void applyFilter(std::vector<QFilter> & qfs,Query & q);
		uint64_t addSum(RelationID rid,int cid);

		/* join operations */
		void join(LinkedList * other, QJoin & qj, Query & q);
		void selfJoin(QJoin & qj, Query & q);
};		
