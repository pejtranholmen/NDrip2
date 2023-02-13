



#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Doc.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include <PG/PG.h>
#include <pqxx/pqxx>
#include <Util/CoupModel_DB_Link.hpp>
using namespace pqxx;

int main(int argc, char *argv[]) {

    unique_ptr<NewMap> pDoc(new Doc());
    CommonModelInfo* pinfo{ nullptr };
    if (pDoc->m_pCommonModelInfo == nullptr) {
        auto pP = pDoc->GetPPointer("Upper Depth");
        if (pP != nullptr)	pinfo = pP->getCommonModelPointer();
    }
    else
        pinfo = pDoc->m_pCommonModelInfo;

// Call to units which construct all necessary tables for Postgres
   auto init_tables = create_Init_Tables(pinfo);
   auto main_tables = create_Main_Tables(pinfo);
   auto add_tables = create_Additional_Tables(pinfo, move(pDoc));


     
}
