/**
 * main.cpp
 *
 * MAIN ROUTINE WHICH CALLED BY TEST-HARNESS
 *
 * @author Aposta
 */

/*==========*/
/* INCLUDES */
/*==========*/
#include <iostream>
#include <string>
#include "Query.h"
#include "Database.h"
#include "tbb/tbb.h"

using namespace std;
using namespace tbb;
/*==============================*/
/* HELPER FUNCTION DECLARATIONS */
/*==============================*/

/* nothing here _(:_ <)_ */

/*==============*/
/* MAIN ROUTINE */
/*==============*/

int main(int argc, char * argv[])
{

	/* Database Initialization */
	/*	-- get filename from input */
	/*	-- open file and load relations */

	Database database;
    initDatabase(&database);
	string line;
	while(getline(cin, line))
	{
		if(line == "Done") break;
		/* TODO: load relations from file */
		database.addRelation(line);
	}	

	/* Environment Initialization */
	/*	-- initialize the query buffer */
	/*	-- get device info (maybe)	*/

	

	/* Parser Awaken */
	/*	-- class QueryParser (maybe) */

	
	std::vector<Query> qs;
	int totID = 0;
	while(getline(cin,line))
	{
		/* meet the end of a query-batch */
		if(line == "F"){
			totID = 0;
			vector<Result> ress = database.batchQueryQxecution(qs);
			for(auto res : ress)
			{
				auto & outputs = res.output();
				for(size_t i=0;i<outputs.size()-1;++i)
					cout<<outputs[i]<<" ";
				cout<<outputs.back();
				cout<<endl;
			}
			qs.clear();
			continue;
		   	
		}

		/* TODO: parse Queries */
		Query q(line);
		q.setID(totID++);
		qs.push_back(q);
	}

	/* Event Dispatcher Awaken */
	/*	-- class EventDispatcher */
	/*	-- initialize some workers according to env */


	/* Waiting for Execution */
	
	return 0;
}
