#pragma once

/**
 * InterRelation.h
 *
 * Defines a class used to store the intermediate value
 * of Relations during join operation, using Bitmap
 *
 * @see Bitmap.h Relation.h
 */


/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#include <vector>
#include <map>
#include "Bitmap.h"


/*===============================*/
/* GLB TYPE AND VAR DECLARATIONS */
/*===============================*/

/**
 * class InterRelation
 */
class InterRelation
{
	/*----------------*/
	/* private fields */
	/*----------------*/
	private:
		std::vector<Bitmap> maps;
		std::vector<int>	reids;
		std::map<int,size_t>	reid2ind;
		
	/*----------------*/
	/* public methods */
	/*----------------*/
	public:

		/* constructors */
		InterRelation() = default;

		/* modifiers and getters */
		void addRelation(Relation & r, int reid, 
						bool isFilled = false);
		Bitmap & getBitmap(int reid);


		/* destructors */
		~InterRelation() = default;



};


/*=======================*/
/* FUNCTION DECLARATIONS */
/*=======================*/
