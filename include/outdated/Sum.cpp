#include "Sum.h"

std::vector<uint64_t> 
getTheSum(Query q,LinkedList l){
	std::vector<QProjection> projections=q.getProjections();
	for(std::vector<QProjection>::iterator i=projections.begin();i!=projections.end();i++){
		RelationID relationID=(*i).sel.first;
		ColumnID columnID=(*i).sel.second;	
	}
}


