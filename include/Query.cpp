/**
 * Query.cpp
 *
 * @see Query.h
 */

/* INCLUDES AND MACROS */
#include "Query.h"

/* HELPER DECLARATIONS */


/* CLASS COMPELETIONS */



/*=================*/
/* IMPLEMENTATIONS */
/*=================*/


/*------------------------*/
/* functions of executions*/
/*------------------------*/


/*--------------------------------*/
/* functions in class QueryParser */
/*--------------------------------*/

bool 
QueryParser::isJoin(const std::string & raw)
{
	return std::count(raw.begin(),raw.end(),'.')==2;
}

void
QueryParser::splitString(const std::string & raw, char delimiter,
						StringArray & tokens)
{
	std::string token;
	std::istringstream tokenStream(raw);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
}

void	/* parse string like "1.2" to QSelection obj */
QueryParser::parseSelection(QSelection & qs, const std::string & r)
{
	StringArray vs;
	splitString(r,'.',vs);
	qs.first = stoul(vs[0]);
	qs.second = stoul(vs[1]);
}

void	/* parse string like "0.0>3000" to the QFilter obj */
QueryParser::parseFilter(QFilter & qf, const std::string & r)
{
	StringArray vs;
	static auto arr = {QFilter::Comparison::Equal,
						QFilter::Comparison::Greater,
						QFilter::Comparison::Less};
	for(auto ct : arr)
	{
		if(r.find(ct)!=std::string::npos)
		{
			splitString(r,ct,vs);
			parseSelection(qf.sel,vs[0]);
			qf.constant = stoul(vs[1]);
			qf.comparison = ct;
		}	
	}
}

void	/* parse string like "1.1=2.2" to QJoin obj */
QueryParser::parseJoin(QJoin & qj, const std::string & r)
{
	StringArray vs;
	splitString(r,'=',vs);
	parseSelection(qj.left, vs[0]);
	parseSelection(qj.right, vs[1]);
}

void	/* parse string like "0.0" to QProjection obj */
QueryParser::parseProjection(QProjection & qp, const std::string & r)
{
	parseSelection(qp.sel, r);
}

void
QueryParser::parseQuery(Query & q, const std::string & r)
{
	q.clear();
	StringArray parts;
	splitString(r,'|',parts);
	
	/* for relationIDs */
	std::string & re = parts[0];
	StringArray res;
	splitString(re,' ',res);
	for_each(res.begin(),res.end(),
			[&q](auto & s){
				q.reids.push_back(stoul(s));
				//printf("%d",q.reids.back());
			});

	/* for filters and joins */
	std::string & mid = parts[1];
	StringArray preds;
	splitString(mid,'&',preds);	
	for_each(preds.begin(),preds.end(),
			[&q](auto & s)
			{
				if(isJoin(s))
					q.joins.emplace_back(s);
				else
					q.filters.emplace_back(s);
			});

	/* for projections */
	std::string last = parts[2];
	StringArray projs;
	splitString(last,' ',projs);
	for_each(projs.begin(),projs.end(),
			[&q](auto & s)
			{
				q.projections.emplace_back(s);
			});
}

/*--------------------------*/
/* functions in class Query */
/*--------------------------*/

Query::Query(std::string & rawString)
{
	QueryParser::parseQuery(*this,rawString);
}

void
Query::clear()
{
	reids.clear();
	joins.clear();
	filters.clear();
	projections.clear();
}

RelationID 
Query::getRelationID(const QSelection &sel)
{
	return reids[sel.first];
}

std::vector<RelationID> 
Query::getReids()
{
	return this->reids;
}

std::vector<QFilter>
Query::getFilters()
{
	return this->filters;
}

std::vector<QJoin>
Query::getJoins()
{
	return this->joins;
}

std::vector<QProjection>
Query::getProjections()
{
	return this->projections;
}
/*--------------------------------*/
/* overloaded ostream operator << */
/*--------------------------------*/

std::ostream & operator<<(std::ostream & o, const Query & q)
{
	o<<"SELECT ";
	for(auto proj : q.projections) o<<proj<<", ";
	o<<std::endl;

	o<<"FROM ";
	for(size_t i=0;i<q.reids.size();i++)
	{
		auto & sel = q.reids[i];
		o<<"r"<<sel<<" "<<'"'<<i<<'"'<<", ";
	}
	o<<std::endl;

	o<<"WHERE ";
	for(auto jo : q.joins) o<<jo<<" and ";
	for(auto fi : q.filters) o<<fi<<" and ";

	return o;
}

std::ostream & operator<<(std::ostream & o, const QSelection & q)
{
	o<<q.first<<".c"<<q.second;
	return o;
}

std::ostream & operator<<(std::ostream & o, const QFilter & q)
{
	o<<q.sel<<(char)q.comparison<<q.constant;
	return o;
}

std::ostream & operator<<(std::ostream & o, const QJoin & q)
{
	o<<q.left<<'='<<q.right;
	return o;
}

std::ostream & operator<<(std::ostream & o, const QProjection & q)
{
	o<<"SUM("<<q.sel<<")";
	return o;
}
