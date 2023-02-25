

#include "./Util/SimUtil.hpp"

#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Doc.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include "./FUtil.hpp"

#include <PG/PG.h>



int main(int argc, char *argv[]) {

   auto path = FUtil::GetCurrentPath();
   auto kolla = FUtil::GetFileList(".xml");
    

   for (size_t i = 0; i < kolla.size(); i++) {
            unique_ptr<Doc> pDoc = SimUtil::CreateDoc(i, kolla[i]);
            if(i==0) pDoc->ReDefinePostgresDataBase();

            pDoc->WriteDoc_To_Postgres();
    }
   
    

  //  pDoc->ReDefinePostgresDataBase();
/* Call to units which construct all necessary tables for Postgres
 auto init_tables = create_Init_Tables(pinfo);
 auto main_tables = create_Main_Tables(pinfo);
 auto add_tables = create_Additional_Tables(pinfo, move(pDoc));
 */

    return 0;

}
