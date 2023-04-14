

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
    unique_ptr<Register> p_Register = make_unique<Register>();

    string answer;

    pair<string, unique_ptr<Register>> p = FUtil::GetProfileStringStd("Creator", "NN", move(p_Register));

    string currentcreator = p.first; p_Register = move(p.second);

    cout << "Current Creator Id :" << currentcreator<<" Enter string to change: ";
    cin >> answer;

    if (answer.size() > 2) {
        p_Register=FUtil::WriteProfileString("Creator", answer, move(p_Register));

    }
    p = FUtil::GetProfileStringStd("SiteNameId", "-", move(p_Register));
    
    string current_site = p.first; p_Register = move(p.second);
    
    cout << "Current SiteName Id :" << current_site << " Enter string to change: ";
    cin >> answer;

    if (answer.size() > 2&&answer.size()<25)   p_Register=FUtil::WriteProfileString("SiteNameId", answer, move(p_Register));

    
    p = FUtil::GetProfileStringStd("Comment", "NN", move(p_Register));
    string current_comment = p.first; p_Register = move(p.second);

    cout << "Current Comment Id :" << current_comment << " Enter string to change: ";
    cin >> answer;
    if (answer.size() > 2 && answer.size() < 25)  p_Register= FUtil::WriteProfileString("Comment", answer, move(p_Register));
    
    if (argc == 1 && kolla.size() > 0) {

        cout << kolla.size() << " xml files ready for upload" << endl;

        cout << "Do you want to Upload ? (Y/N) :";
        cin >> answer;

        if (answer.find('y') != string::npos || answer.find('Y') != string::npos);
        else
            kolla.clear();
    }
   if (argc == 1 && kolla.size() > 0) {

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
               pDoc->SetLocalHost(true);
               pDoc->ReDefinePostgresDataBase();
           }
       }
    }
  
    return 0;

}
