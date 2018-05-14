/**
 * Relation.h
 *
 * Defines the data structure used by the database
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/

#pragma once
#include <stdint.h>
#include <string>
#include <stdlib.h>
#include <vector>

/*================================*/
/* GLB TYPE AND CLASS DEFINITIONS */
/*================================*/
typedef size_t RelationID; 
struct SortedColumnElement
{
	uint64_t key,pos;
};

typedef SortedColumnElement SCE;
typedef uint64_t			UCE; /* UnsortedColumnElement */

class Relation
{
	private:
		size_t size;
		std::vector<UCE*> cols;
		std::vector<SCE*> sortedCols;
		std::vector<bool> readyCols;

	private:
		void loadFile(const char * fileName);

	public:	
		/* Constructors */
		Relation() = default;
		Relation(const std::string & fileName);
		Relation(const Relation & r) = delete;
		Relation(Relation && r) = default;
		
		/* getter and modifiers */
		size_t getSize() const;
		size_t getWidth() const;
		bool isReady(unsigned col) const;
		SCE * getSortedColumn(unsigned col);
		UCE * getColumn(unsigned col);
		uint64_t getEle(unsigned col, size_t pos);


		void sortCol(unsigned col);

		/* destructor */
		~Relation();
};

/* FUNCTION DEFINITIONS */
inline bool
operator<(const SCE & l, const SCE & r)
{
	return l.key<r.key;
}
