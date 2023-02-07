#include "DB_Header.h"


DB_Header::DB_Header(void)
{
	MySingle::instance().SetDB_HeaderPointer(this);
}


DB_Header::~DB_Header(void)
{
}
size_t DB_Header::DeleteRecord(size_t db_index, size_t keyindex) {
		if(db_index<9&&db_index>=0) {
			if(keyindex<m_OldHeader[db_index].size()&& keyindex<m_NewHeader[db_index].size()) {
				m_OldHeader[db_index].erase(m_OldHeader[db_index].begin()+keyindex);
				m_NewHeader[db_index].erase(m_NewHeader[db_index].begin()+keyindex);
				if (keyindex <= m_NumOriginalRec[db_index]) m_NumOriginalRec[db_index]--;
				return m_OldHeader[db_index].size();
			}
			else
				return m_OldHeader[db_index].size();
		}
		else
			return m_OldHeader[db_index].size();
}