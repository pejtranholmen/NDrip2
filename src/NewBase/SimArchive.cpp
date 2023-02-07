
#include "SimArchive.h"
#include "../std.h"
#ifndef COUPSTD
#include "../CoupModelDoc.h"
#endif

SimArchive::SimArchive()
{
#ifndef COUPSTD
	MySingle::instance().SetSimArchivePointer(this);
#endif
}
SimArchive::~SimArchive()
{


}

#ifndef COUPSTD
Doc *SimArchive::GetSimDocPointer(size_t index)
{
	if(index<m_SimDocArray.size())
		return (Doc*)m_SimDocArray[index].pSimDoc;//GetDocPointer(index)->GetSimDocPointer();
	else
		return nullptr;

}

CCoupModelDoc *SimArchive::GetDocPointer(size_t index)
{
	if(index<m_SimDocArray.size())
		return m_SimDocArray[index].pDoc;
	else
		return nullptr;

}
#endif
