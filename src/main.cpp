

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

       cout << kolla.size()<<" xml files ready for upload"<<endl;

       for (size_t i = 0; i < kolla.size(); i++) {
           cout << FUtil::STD_ItoAscii(i + 1) << " : " << kolla[i] << endl;
       }

       cout << "Select your number:";
       cin >> answer;
       size_t ifile = FUtil::AtoInt(answer);
       if (ifile >= 0 && ifile <= kolla.size()) ifile+= -1;
       else ifile = 0;

       doc_file_name = kolla[ifile];
       cout << "Upload to Remote Database (Y/N) ?:";
       cin >> answer;

       unique_ptr<Doc> pDoc = SimUtil::CreateDoc(0, doc_file_name);

       if (answer.find('y') != string::npos || answer.find('Y') != string::npos) 
           pDoc->SetLocalHost(false);
       else
           pDoc->SetLocalHost(true);


       if (pDoc != nullptr) {
           if (pDoc->WriteDoc_To_Postgres()) {
               cout <<"File Content Uploaded :"<< doc_file_name << endl;
               cin >> answer;
           }
       }
       return 0;
   }



   else if (argc == 2) {
       doc_file_name = argv[1];
       string str;
       auto pos = doc_file_name.rfind('\\');
       str = doc_file_name.substr(0, pos + 1);

       FUtil::SetDocumentPath(str);
       cout << str << endl;
       cout << "Upload to Remote Database (Y/N) ?:";
       cin >> answer;

       unique_ptr<Doc> pDoc = SimUtil::CreateDoc(0, doc_file_name);
       if (answer.find('y') != string::npos || answer.find('Y') != string::npos) {        
           pDoc->SetLocalHost(false);
       }
       else
           pDoc->SetLocalHost(true);

       pDoc->WriteDoc_To_Postgres();
       cout << "xml-sim file: "<<doc_file_name <<"transfered to database "<< endl;
       cin >> answer;

       return 0;
   }
   else if (argc ==1) {
       string ans;
       cout << "Do you want to reset Remote Database (Y/N) ?:";
       cin >> ans;
       if (ans.find('y')!=string::npos || ans.find('Y')!=string::npos) {

           auto pDoc = make_unique<Doc>();

           pDoc->SetLocalHost(false);
           pDoc->ReDefinePostgresDataBase();
       }
       else {
           cout << "Do you want to reset Local Database (Y/N) ?:";
           cin >> ans;
           if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
               auto pDoc = make_unique<Doc>();
               pDoc->ReDefinePostgresDataBase();
           }
       }
    }
  
    return 0;

}
