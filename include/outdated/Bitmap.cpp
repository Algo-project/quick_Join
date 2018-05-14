/**
 * Bitmap.cpp
 *
 * @see Bitmap.h
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#include <string.h>
#include "Bitmap.h"

/*==========================*/
/* FUNCTION IMPLEMENTATIONS */
/*==========================*/
using namespace Inc;

int Inc::popcnt(uint64_t ll)
{
	return __builtin_popcountll(ll);
}

int Inc::popcnt(uint32_t i)
{
	return __builtin_popcountll(i);
}

int Inc::getTrailingZeros(uint64_t ll)
{
	return __builtin_ctzll(ll);
}

int Inc::getLeadingZeros(uint64_t ll)
{
	return __builtin_clzll(ll);
}

uint64_t Inc::lowbit(uint64_t ll)
{
	return ll&(-ll);
}

int Inc::lowbitPos(uint64_t ll)
{
	return __builtin_ffsll(ll);
}

/*=======================*/
/* CLASS IMPLEMENTATIONS */
/*=======================*/

/*----------------------*/
/* for Bitmap::iterator */
/*----------------------*/

using BIterator = Bitmap::iterator;

BIterator::iterator(Bitmap * _pb, size_t _pos)
	:pb(_pb),pos(_pos){}

std::vector<uint64_t> 
BIterator::operator*()
{
	return (*pb)[pos];
}

BIterator &
BIterator::operator++()
{
	size_t ind = pos >> 6, /* pos/64 */
		   off = 1ull << (pos & 0x3f);
	__Ele_t ele = pb->bits[ind],
			mask = ~((off << 1) - 1);
	auto tmp = ele & mask;
	auto tind = lowbitPos(tmp);
	if(tind) this->pos = (ind << 6) + tind;
	else 
	{
		while(ind < pb->__slots && pb->bits[++ind] == 0);
		if(ind == pb->__slots) 
			this->pos = this->npos;
		else 
			this->pos = (ind << 6) + lowbitPos(pb->bits[ind]);
	}
	
	return *this;
}

BIterator 
BIterator::operator++(int)
{
	BIterator ret = *this;
	++ret;
	return ret;
}

bool
BIterator::operator==(const BIterator & other)
{
	return this->pb == other.pb && this->pos == other.pos;
}

bool
BIterator::operator!=(const BIterator & other)
{
	return !(*this == other);
}

/*------------*/
/* for Bitmap */
/*------------*/

/* constructors */
	
Bitmap::Bitmap(Relation & r, bool isFilled)
	:relation(r),__slots((r.getSize()>>6)+1)
{
	/* all filled */
	__count = relation.getSize();
	
	startPos = 0;
//	endPos = __count - 1;

	bits = new __Ele_t[__slots];
	if(isFilled)
		fill();
	else clear();
}


/* global operations */

size_t
Bitmap::count() const
{
	return this->__count;
}

BIterator
Bitmap::begin() 
{
	return this->__begin;
}

BIterator 
Bitmap::end()
{
	return this->__end;
}

void
Bitmap::fill()
{
	memset(bits, -1, __slots * sizeof(__Ele_t));
	auto last = (1ull << (relation.getSize() & 0x3f)) - 1;
	bits[__slots-1] = last;

	__count = relation.getSize();
	startPos = 0; //endPos = __count - 1;

	__begin = iterator(this, 0);
	__end = iterator(this, BIterator::npos);
}

void
Bitmap::clear()
{
	memset(bits, 0, __slots * sizeof(__Ele_t));
	__count = 0;
	startPos = -1; //endPos = 0;
	__begin = iterator(this, BIterator::npos);
	__end = iterator(this, BIterator::npos);
}


/*
 * row operations
 */

unsigned
Bitmap::getWidth() const
{
	return relation.getWidth();
}

Bitmap::__Row_t
Bitmap::operator[](size_t rowNum)
{
	__Row_t v;
	for(size_t i=0;i<this->getWidth();i++)
		v.push_back(relation.getEle(i, rowNum));
	return v;
}

bool
Bitmap::testFor(size_t rowNum)
{
	auto pair = row2pair(rowNum);
	return bits[pair.first] & pair.second;
}

bool 
Bitmap::erase(size_t rowNum)
{
	/* if doesn't exist */
	if(!testFor(rowNum)) return false;

	/* update begin & end */
	if (rowNum == startPos)
	{
		++__begin;
		startPos = __begin.pos;
	}

	/*
	if (rowNum == endPos)
	{
	}
	*/

	--__count;
	auto pair = row2pair(rowNum);
	bits[pair.first] &= ~pair.second;
	return true;
}

bool
Bitmap::erase(iterator i)
{
	if(i.pb != this) return false;
	return this->erase(i.pos);
}

bool 
Bitmap::setExist(size_t rowNum)
{
	/* if already exist */
	if(testFor(rowNum)) return false;

	/* update begin and end */
	if (rowNum < startPos)
	{
		startPos = rowNum;
		__begin = iterator(this,startPos);
	}

	/*
	if (rowNum > end)
	{
	}
	*/

	++__count;
	auto pair = row2pair(rowNum);
	bits[pair.first] |= pair.second;
	return true;
}

size_t
Bitmap::getSize() const
{
	return relation.getSize();
}

/**
 * column operations
 */

uint64_t 
Bitmap::addSum(unsigned colNum)
{
	auto col = relation.getColumn(colNum);
	uint64_t ans = 0;
	for(size_t i=0;i<relation.getSize();i++)
	{
		auto pair = row2pair(i);
		if(bits[pair.first] & pair.second)
			ans += col[i];
	}
	return ans;
}

/* destructor */
Bitmap::~Bitmap()
{
	delete[] bits;
}
