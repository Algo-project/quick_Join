/**
 * InterRelation.cpp
 *
 * @see InterRelation.h
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#include <exception>
#include <iostream>
#include "InterRelation.h"


/*==========================*/
/* FUNCTION IMPLEMENTATIONS */
/*==========================*/


/*=======================*/
/* CLASS IMPLEMEMTATIONS */
/*=======================*/

void 
InterRelation::addRelation(Relation & r, int reid,
							bool isFilled)
{
	if(reid2ind.count(reid)) return;
	reid2ind[reid] = reids.size();
	reids.push_back(reid);
	maps.emplace_back(r,isFilled);
}

Bitmap & 
InterRelation::getBitmap(int reid)
{
	if(reid2ind.count(reid) == 0)
	{
		std::cerr<<"no Bitmap for relation "<<reid<<std::endl;
		throw std::exception();
	}

	return maps[reid2ind[reid]];
}
