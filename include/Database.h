/**
 * Database.h
 *
 * Defines the structure of the whole database
 * Also contains some schedulers
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#pragma once
#include <vector>
#include <iostream>
#include "Relation.h"
#include "LinkedList.h"
#include "Query.h"
#include "tbb/parallel_for_each.h"
/*===============================*/
/* GBL TYPE AND CLASS DEFINITION */
/*===============================*/

class Result{
public:
	struct Elem
	{
		uint64_t result;
		bool isNull;
	};
private:
    size_t id;
    std::vector<Elem> outputs;
public:
	Result() = default;
	Result(size_t i):id(i){}
    size_t &getID(){ return id; }
    std::vector<Elem> &output(){ return outputs;}
	void push_back(const Elem & e){outputs.push_back(e);}
};

class Database
{
	private:
		
		std::vector<Relation> relations;

	public:
		void addRelation(std::string & fileName);
		Relation *getRelation(RelationID r);    

	public:
		void queryExec(Query & q, Result & r);
       	std::vector<Result> batchQueryQxecution(std::vector<Query> v);
};

void initDatabase (Database *d);
Database * getDatabase();


std::ostream & operator<<(std::ostream & o, Result::Elem & e);
