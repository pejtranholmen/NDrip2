#include "../std.h"
#include "./PGFileArchive.h"

PGFileArchive::PGFileArchive()
{

	//MySingle::instance().SetPGFileArchivePointer(this);
	m_PGFileArray.resize(0);

}
PGFileArchive::~PGFileArchive()
{


}

CPG *PGFileArchive::GetPGFile(size_t index)
{
	if(index<m_PGFileArray.size())
		return m_PGFileArray[index].pFile;
	else
		return nullptr;

}

CCoupModelDoc *PGFileArchive::GetDocPointer(size_t index)
{
	if(index<m_PGFileArray.size())
		return m_PGFileArray[index].pDoc;
	else
		return nullptr;

}
