#pragma once
/**
 * Bitmap.h
 *
 * A bitmap to record the existance of rows in a relation
 * and give some access methods to a specified relation
 *
 * @see Relation.h
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#include <vector>
#include "Relation.h"

/*===============================*/
/* GLB TYPE AND VAR DECLARATIONS */
/*===============================*/

/**
 * class Bitmap
 *
 * @TODO: add "size" filed into Bitmap
 */
class Bitmap
{
	/*----------------*/
	/* private fields */
	/*----------------*/
	private:
		Relation & relation;

		typedef uint64_t __Ele_t;
		typedef std::vector<uint64_t> __Row_t;
		__Ele_t * bits;

		const size_t __slots;

		size_t __count;		/* total "1"s in whole bitmap */
		size_t startPos;//,endPos;
							/* where the first "1" is, 
							   only for performance enhancement
							   the minimum number s.t.
									bits[startpos] != 0
							  */
	public:
		/**
		 * class Bitmap::iterator -- based on ForwardIterator
		 */
		class iterator
		{
			friend class Bitmap;
			private:
				Bitmap * pb;
				size_t pos;
			public:
				static const size_t npos = -1;
			public:

				/* constructor */
				iterator() = default;
				iterator(Bitmap * pb, size_t _pos);

				/* operators */
				std::vector<uint64_t> operator*();
				iterator operator++(int);
				iterator & operator++();

				bool operator==(const iterator & other);
				bool operator!=(const iterator & other);
		};

	private:
		iterator __begin,__end;

	private:
		std::pair<uint64_t,uint64_t>
			row2pair(size_t rowNum)
			{
				return {rowNum >> 6, 1ull << (rowNum & 0x3f)};
			}

	/*----------------*/
	/* public methods */
	/*----------------*/
	public:
		
		/* constructors */
		Bitmap(Relation & r, bool isFilled);	/* fill Bitmap */
		Bitmap(const Bitmap & b) = delete;
		Bitmap(Bitmap &&other) = default;

		/* getters and modifiers */
		
		/**
		 * Global information
		 *
		 * count():		@returns total "1"s in the bitmap 
		 * begin():		@returns a iterator points to the begin
		 * end():		@returns a iterator points to the end
		 * fill():		fills the whole map
		 * clear():		clear the whole map
		 */
		size_t count() const;
		iterator begin();
		iterator end();
		void fill();
		void clear();


		/**
		 * row operations
		 *
		 * getWidth():		@returns the width of the relation
		 * testFor(row):	@returns true if the specified row exists
		 * erase(row):		erase a specified row
		 *					@returns true if the row exists before erase
		 * setExist(row):	set a specified row as exist
		 *					@returns true if the row doesn't before erase
		 * operator[](row):	@returns a vector contains the tuple
		 */
		unsigned	getWidth() const;
		size_t		getSize() const;
		bool		testFor(size_t rowNum);
		bool		erase(size_t rowNum);
		bool		erase(iterator i);
		bool		setExist(size_t rowNum);
		__Row_t		operator[](size_t rowNum);

		/**
		 * column operations
		 */
		uint64_t	addSum(unsigned colNum);



		/* destructor */
		~Bitmap();

};

/*=======================*/
/* FUNCTION DECLARATIONS */
/*=======================*/

/* TODO: bits operation here */
namespace Inc
{
	int popcnt(uint64_t ll);
	int popcnt(uint32_t i);

	int getTrailingZeros(uint64_t ll);
	int getLeadingZeros(uint64_t ll);

	uint64_t lowbit(uint64_t ll);
	int lowbitPos(uint64_t ll);

};
