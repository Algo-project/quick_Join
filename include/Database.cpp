/**
 * Database.cpp
 *
 * @see Database.h
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#include <iostream>
#include <unordered_map>
#include <limits.h>
#include <string.h>
#include "Database.h"

static Database * _pdatabase;
/*=================*/
/* IMPLEMENTATIONS */
/*=================*/

	void 
Database::addRelation(std::string & fileName)
{
	relations.emplace_back(Relation(fileName));
}

	Relation *
Database::getRelation(RelationID rid)
{
	return &(relations[rid]);
}


void initDatabase (Database *d){
	_pdatabase = d;
}

Database* getDatabase(){return _pdatabase;}

void
Database::queryExec(Query & q, Result & results)
{
	/* make linked list */
	std::vector<LinkedList*> plists;
	std::unordered_map<RelationID,LinkedList*> rid2list;

	auto reids = q.getReids();
	for(auto rid : reids)
	{
		auto & relation = relations[rid];
		plists.push_back(new LinkedList(relation,rid,this));
		rid2list.insert({rid,plists.back()});
	}

	typedef std::vector<QFilter> _Filters;
	std::unordered_map<RelationID,_Filters> rid2filters;
	for(auto filter : q.getFilters())
	{
		auto rid = q.getRelationID(filter.sel);
		rid2filters[rid].push_back(filter);
	}
	for(auto & ent : rid2filters)
	{
		auto rid = ent.first;
		rid2list[rid]->applyFilter(ent.second, q);
	}


	auto joins = q.getJoins();
	int cnt = 0,tot = joins.size();
	bool vis[tot];
	memset(vis,false,sizeof(vis));
	while(cnt<tot)
	{
		uint64_t minscore = 1ull<<63;
		QJoin target;
		size_t targetPos = -1;
		/* scan qjoins and score them */

		for(size_t i=0;i<joins.size();++i)
		{
			if(vis[i]) continue;
			auto & _qj = joins[i];

			auto leftid = q.getRelationID(_qj.left),
				 rightid = q.getRelationID(_qj.right);

			auto leftsize = this->getRelation(leftid)->getSize(),
				 rightsize = this->getRelation(rightid)->getSize();

			if(leftsize+rightsize < minscore)
			{
				minscore = leftsize+rightsize;
				target = _qj;
				targetPos = i;
			}
		}
		vis[targetPos] = true;


		/* join or selfjoin? */
		auto leftid = q.getRelationID(target.left),
			 rightid = q.getRelationID(target.right);

		auto llist = rid2list[leftid],
		rlist = rid2list[rightid];

		/* exec */
		if(llist == rlist) llist->selfJoin(target,q);
		else
		{
			//TODO: didn't swap left and right by size
			llist->join(rlist,target,q);

			/* change map */
			for(auto rid : rlist->getReids())
				rid2list[rid] = llist;
		}
		++cnt;
	}

	/* get ans */
	for(auto proj : q.getProjections())
	{
		auto ans = rid2list[q.getRelationID(proj.sel)];
		auto rid = q.getRelationID(proj.sel);
		auto cid = proj.sel.second;
		auto value = ans->addSum(rid,cid);
		results.push_back((Result::Elem){value,value == 0});
	}

	/* clean up */
	for(auto pl : plists)
		delete pl;
}


std::vector<Result> Database::batchQueryQxecution(std::vector<Query> v){
	std::vector<Result> result;
	for(uint64_t i=0;i<v.size();++i)
		result.emplace_back(Result(i));
	tbb::parallel_for_each(v.begin(),v.end(),[&](Query q){
				queryExec(q,result[q.getID()]);	
			});
	return result;
}

std::ostream & operator<<(std::ostream & o, Result::Elem & e)
{
	if(e.isNull)
		o<<"NULL";
	else o<<e.result;
	return o;
}
