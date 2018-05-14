/**
 * Query.h
 *
 * DEFINES QUERIES PARSER AND QUERY 
 *
 */

/* INCLUDES AND MACROS */
#pragma once
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include "Relation.h"

/* GLB TYPE & CLASS DEFINITIONS */

/**
 * RelationID:	indicates the relation file id
 * QSelection:	basic tuple contains relation file
 *				and column info -- {rid,cid}
 */
typedef std::pair<unsigned,unsigned> QSelection;
typedef std::vector<std::string> StringArray;
/**
 * QFilter:		For filter predications
 * QJoin:		For join predications
 * QProjection:	For projections
 */
class Query;
class QFilter;
class QJoin;
class QProjection;
class QueryParser
{
	private:
		static void
			splitString(const std::string & raw, char delimiter,
					StringArray & buffer);
		static bool 
			isJoin(const std::string & raw);
	public:
		static void 
			parseQuery(Query & q, const std::string & r);
		static void 
			parseSelection(QSelection & qs, const std::string &r);
		static void 
			parseFilter(QFilter & qf, const std::string & r);
		static void 
			parseJoin(QJoin & qj, const std::string & r);
		static void 
			parseProjection(QProjection & qpj, const std::string & r);
};

class QFilter
{
	public:
		enum Comparison : char { Less='<', Greater='>', Equal='='};
		QSelection sel;
		uint64_t constant;
		Comparison comparison;

	public:
		QFilter() = default;
		QFilter(const std::string & raw)
		{
			QueryParser::parseFilter(*this,raw);
		}
		
		friend std::ostream &
			operator<<(std::ostream & o, const QFilter & q);
	
};

class QJoin
{
	public:
		QSelection left;
		QSelection right;
		
	public:
		QJoin() = default;
		QJoin(const std::string & raw)
		{
			QueryParser::parseJoin(*this, raw);
		}

		friend std::ostream &
			operator<<(std::ostream & o, const QJoin & q);
};

class QProjection
{
	public:
		QSelection sel;

	public:
		QProjection() = default;
		QProjection(const std::string & raw)
		{
			QueryParser::parseProjection(*this, raw);
		}

		friend std::ostream &
			operator<<(std::ostream & o, const QProjection & q);
};

/**
 * Query
 * 
 * Global queries storing object
 * One instance of Query stores info for
 * only ONE query
 */
class Query
{
	friend class QueryParser;
	private:	
		/**
		 * @field reids
		 *
		 * "reids[i] = r" 
		 * means "i.c0" uses column 0 in the ith relation file 
		 *
		 * For exp, a predication ({3,c0},<,3000) uses relation
		 * file indicated by reids[3]
		 */
		size_t id;
		std::vector<RelationID> reids;
		std::vector<QJoin> joins;
		std::vector<QFilter> filters;
		std::vector<QProjection> projections;

	private:
	
	public:
		/* constructors */
		Query() = default;
		Query(std::string & rawString);

		/* getters (waiting for database design) */
		RelationID getRelationID(const QSelection &sel);
		std::vector<RelationID> getReids();
		std::vector<QProjection> getProjections();
		std::vector<QJoin>		getJoins();
		std::vector<QFilter> getFilters();
		size_t getID(){return this->id;}
		/* modifiers */
		void clear();
		void setID(size_t id){this->id = id;}

		/* for debug */
		friend std::ostream &
			operator<<(std::ostream & o, const Query & q);	
};




