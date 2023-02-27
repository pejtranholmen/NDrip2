

#include "./Util/SimUtil.hpp"

#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Doc.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include "./FUtil.hpp"

#include <PG/PG.h>



int main(int argc, char* argv[]) {

    string doc_file_name;
    auto path = FUtil::GetCurrentPath();
    auto kolla = FUtil::GetFileList(".xml");
    string answer;

   if (argc == 1 && kolla.size() > 0) {
       doc_file_name = kolla[0];
       unique_ptr<Doc> pDoc = SimUtil::CreateDoc(0, doc_file_name);
       if (pDoc != nullptr) {
           pDoc->WriteDoc_To_Postgres();
           cout << doc_file_name << endl;
           cin >> answer;
       }
       return 0;
   }


   if (argc == 2) {
       doc_file_name = argv[1];
       unique_ptr<Doc> pDoc = SimUtil::CreateDoc(0, doc_file_name);
       pDoc->WriteDoc_To_Postgres();
       cout << "xml-sim file: "<<doc_file_name <<"transfered to database"<< endl;
       cin >> answer;

       return 0;
   }
   
    

   for (size_t i = 0; i < kolla.size(); i++) {
            unique_ptr<Doc> pDoc = SimUtil::CreateDoc(i, kolla[i]);
      //      if(i==0) pDoc->ReDefinePostgresDataBase();

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
