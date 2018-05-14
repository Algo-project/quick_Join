SET(HEADERS Query.h Relation.h Database.h
	LinkedList.h HashTable.h
	)

SET(SOURCES Query.cpp Relation.cpp Database.cpp
	LinkedList.cpp 
	)

ADD_LIBRARY(libalp STATIC ${HEADERS} ${SOURCES})
