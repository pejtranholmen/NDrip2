#pragma once
#include "GetDB.h"

class DB_Header: public GetDB
{
public:
	DB_Header(void);
	~DB_Header(void);
	size_t DeleteRecord(size_t db_index, size_t rec_index);
};

