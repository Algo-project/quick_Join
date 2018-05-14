/**
 * Relation.cpp
 *
 * @see Relation.h
 */

/*=====================*/
/* INCLUDES AND MACROS */
/*=====================*/
#include <iostream>
#include <cassert>
#include <algorithm>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "Relation.h"

/*=================*/
/* IMPLEMENTATIONS */
/*=================*/

/*---------------------------*/
/* methods of class Relation */
/*---------------------------*/

void	/* load Relation from specified file */
Relation::loadFile(const char * fileName)
{
	using namespace std;
	int fd = open(fileName, O_RDONLY);
	if (fd==-1) {
		cerr << "cannot open " << fileName << endl;
		throw;
	}

	// Obtain file size
	struct stat sb;
	if (fstat(fd,&sb)==-1)
		cerr << "fstat\n";

	auto length=sb.st_size;

	char* addr=static_cast<char*>(mmap(nullptr,length,PROT_READ,MAP_PRIVATE,fd,0u));
	if (addr==MAP_FAILED) {
		cerr << "cannot mmap " << fileName << " of length " << length << endl;
		throw;
	}

	if (length<16) {
		cerr << "relation file " << fileName << " does not contain a valid header" << endl;
		throw;
	}

	this->size=*reinterpret_cast<uint64_t*>(addr);
	addr+=sizeof(size);
	auto numColumns=*reinterpret_cast<size_t*>(addr);
	addr+=sizeof(size_t);
	for (unsigned i=0;i<numColumns;++i) {
		this->cols.push_back(reinterpret_cast<uint64_t*>(addr));
		addr+=size*sizeof(uint64_t);
	}	
}

Relation::Relation(const std::string & fileName)
{
	this->size = 0;
	loadFile(fileName.c_str());
	sortedCols.resize(this->size);
	readyCols.resize(this->size);
	for(size_t i=0;i<size;i++) 
	{
		sortedCols[i] = NULL;
		readyCols[i] = false;
	}
}

bool 
Relation::isReady(unsigned col) const
{
	assert(col >= this->size);
	return readyCols[col];
}

void 
Relation::sortCol(unsigned col)
{
	sortedCols[col] = new SCE[this->size];
	
	/* copy memory */
	size_t i=0;
	auto q = cols[col];
	for(auto p = sortedCols[col];i<this->size;++p,++q)
	{
		p->key = *q;
		p->pos = i;
		++i;
	}

	auto p = sortedCols[col];
	std::sort(p,p+this->size);
	
	readyCols[col] = true;
}

SCE *
Relation::getSortedColumn(unsigned col)
{
	if(!readyCols[col])
		sortCol(col);
	return this->sortedCols[col];
}

UCE *
Relation::getColumn(unsigned col)
{
	return this->cols[col];
}

uint64_t 
Relation::getEle(unsigned col, size_t pos)
{
	return this->cols[col][pos];
}

size_t
Relation::getSize() const
{
	return this->size;
}

size_t
Relation::getWidth() const
{
	return this->cols.size();
}

Relation::~Relation()
{
	for(size_t i=0;i<readyCols.size();i++)
		if(readyCols[i])
			delete[] sortedCols[i];
}

