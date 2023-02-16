

#include "Util/SimUtil.hpp"

#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Doc.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include "./FUtil.hpp"
#include "Util/FileSystemUtil.hpp"

#include <PG/PG.h>



int main(int argc, char *argv[]) {

    
    unique_ptr<NewMap> pDoc(new Doc());
    CommonModelInfo* pinfo{ nullptr };
    if (pDoc->m_pCommonModelInfo == nullptr) {
        auto pP = pDoc->GetPPointer("Upper Depth");
        if (pP != nullptr)	pinfo = pP->getCommonModelPointer();
    }
    else
        pinfo = pDoc->m_pCommonModelInfo;

    pDoc->ReDefinePostgresDataBase();
    string path = "C:\\Dev\\NewTest\\out\\build\\x64-debug\\bin";
    string simFilePath;

    path = FileSystemUtil::GetCurrentPath();
    auto kolla = FileSystemUtil::GetFileList(".xml");

    Doc* pDoca;
    for (size_t i = 0; i < kolla.size(); i++) {
        pDoca=SimUtil::CreateDoc(i, kolla[i]);
        pDoca->WriteDoc_To_Postgres();
    }




  //  pDoc->ReDefinePostgresDataBase();
/* Call to units which construct all necessary tables for Postgres
 auto init_tables = create_Init_Tables(pinfo);
 auto main_tables = create_Main_Tables(pinfo);
 auto add_tables = create_Additional_Tables(pinfo, move(pDoc));
 */

     

}
