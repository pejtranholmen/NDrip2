#include "./Util/SimUtil.hpp"

#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Sim.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include "./FUtil.hpp"

#include <PG/PG.h>



int main(int argc, char* argv[]) {
    string doc_file_name;
    string ans, path;
    int CurrentAction, CurrentSimKey, LocalHost;
    auto pDoc = make_unique<Doc>();
    {
        unique_ptr<Register> p_Register = make_unique<Register>();

        path = FUtil::GetCurrentPath();
        auto kolla = FUtil::GetFileList(".xml");
        p_Register = FUtil::WriteProfileString("LocalDirectory", path, move(p_Register));


        pair<int, unique_ptr<Register>> p = FUtil::GetProfileIntNo("Action", 1, move(p_Register));
        CurrentAction = p.first; p_Register = move(p.second);

        p = FUtil::GetProfileIntNo("PostgresOnLocalHost", 1, move(p_Register));
        LocalHost = p.first; p_Register = move(p.second);
        if (LocalHost == 0)
            pDoc->SetLocalHost(false);
        else
            pDoc->SetLocalHost(true);


        ans = "y";

        while (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
            vector<string> Action;
            if (MS_CODE) {
                Action.clear();
                Action.push_back(" - ReRun the Selected Simulation - Overwriting old results");
                Action.push_back(" - ReRun the Selected Simulation - creating new records");
                Action.push_back(" - Delete Selected Record ");
                Action.push_back(" - Download all files linked to Selected Record");
                Action.push_back(" - Download output time series as CSV files");
                Action.push_back(" - Upload a simulation document as xml and bin files:");
                Action.push_back(" - Only Viewing Simulations in database");
                if (LocalHost == 0)
                    Action.push_back(" - Change to from remote to local database");
                else
                    Action.push_back(" - Change to from local to remote database");
                Action.push_back(" - Reset Database (removes all items)");
                Action.push_back(" - Clean Database from unlinked units");

                cout << "List of possible actions:" << endl;
                size_t id = 1;
                for (auto line : Action) { cout << to_string(id) << line << endl; id++; }
                cout << "Specify your choice of action :";
                cin >> ans;
                int newaction = FUtil::AtoInt(ans);
                if (newaction > 0 && newaction < 8) {
                    p_Register = FUtil::WriteProfileInt("Action", newaction, move(p_Register));                  
                }
                CurrentAction = newaction;
            }
            vector<pair<int, string>> simulations = pDoc->GetDataBaseSimulations();
            auto numsim = simulations.size();
            p = FUtil::GetProfileIntNo("SimKey", 0, move(p_Register));
            CurrentSimKey = p.first; p_Register = move(p.second);
            if (CurrentAction != 8) {
                if (MS_CODE) {
                    bool keyexist = false; size_t MaxKeyValue{ 0 };
                    if (CurrentAction == 6) {
                        string doc_file_name;

                        auto path = FUtil::GetCurrentPath();
                        auto kolla = FUtil::GetFileList(".xml");


                        cout << kolla.size() << " xml files ready for upload" << endl;
                        for (size_t i = 0; i < kolla.size(); i++) {
                            cout << FUtil::STD_ItoAscii(i + 1) << " : " << kolla[i] << endl;
                        }


                        cout << "Select number to upload :";
                        cin >> ans;
                        size_t ifile = FUtil::AtoInt(ans);
                        if (ifile >= 0 && ifile <= kolla.size()) ifile += -1;
                        else ifile = 0;
                        doc_file_name = kolla[ifile];

                        cout << "Current settings for upload:" << endl;
                        pair<string, unique_ptr<Register>> p = FUtil::GetProfileStringStd("Creator", "NN", move(p_Register));
                        string currentcreator = p.first; p_Register = move(p.second);
                        cout << "Creator Id :" << currentcreator << endl;
                        p = FUtil::GetProfileStringStd("SiteNameId", "-", move(p_Register));
                        string current_site = p.first; p_Register = move(p.second);
                        cout << "SiteName Id :" << current_site << endl;
                        p = FUtil::GetProfileStringStd("Comment", "NN", move(p_Register));
                        string current_comment = p.first; p_Register = move(p.second);
                        cout << "Comment Id :" << current_comment << endl;
                        

                        cout << "Do want to change settings for your upload to database (Y/N);";
                        cin >> ans;
                        if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                            cout << "Current Creator Id :" << currentcreator << " Enter string to change: ";
                            cin >> ans; if (ans.size() > 2)  p_Register = FUtil::WriteProfileString("Creator", ans, move(p_Register));
                            cout << "Current SiteName Id :" << current_site << " Enter string to change: ";
                            cin >> ans;  if (ans.size() > 2 && ans.size() < 25)   p_Register = FUtil::WriteProfileString("SiteNameId", ans, move(p_Register));
                            cout << "Current Comment Id :" << current_comment << " Enter string to change: ";
                            cin >> ans;  if (ans.size() > 2 && ans.size() < 25)  p_Register = FUtil::WriteProfileString("Comment", ans, move(p_Register));
                        };

                        unique_ptr<Doc> pDoca = SimUtil::CreateDoc(0, doc_file_name);
                        if (pDoca->WriteDoc_To_Postgres())
                            cout << "File Content Uploaded :" << doc_file_name << endl;
                        else
                            cout << "Failed to upload" << endl;

                    }
                    else {
                       
                        string db = "local"; if (LocalHost == 0) db = "remote";
                        cout << to_string(numsim) << " simulations in " << db << " database" << endl;;
                        for (size_t i = 0; i < numsim; i++) {
                            cout << to_string(simulations[i].first) << " - " << simulations[i].second << endl;
                            if (MaxKeyValue < simulations[i].first) MaxKeyValue = simulations[i].first;
                            if (simulations[i].first == CurrentSimKey) keyexist = true;
                        }
                    }
                    ans = "Y";
                    if (CurrentAction < 6) {
                        if (keyexist) {
                            cout << " A simulation is identified key id : " << to_string(CurrentSimKey) << " Continue with action (Y) or Give new Key(#) :";
                            cin >> ans;
                            if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {



                            }
                            else
                            {
                                CurrentSimKey = FUtil::AtoInt(ans);
                                if (CurrentSimKey > 0 && CurrentSimKey <= MaxKeyValue) {
                                    p_Register = FUtil::WriteProfileInt("SimKey", CurrentSimKey, move(p_Register));
                                }
                                else return 0;
                            }
                        }
                        else {
                            cout << " No simulation is identified with no : " << to_string(CurrentSimKey) << " Continue by selection  number :";
                            cin >> ans;
                            CurrentSimKey = FUtil::AtoInt(ans);
                            if (CurrentSimKey > 0 && CurrentSimKey <= MaxKeyValue) {
                                p_Register = FUtil::WriteProfileInt("SimKey", CurrentSimKey, move(p_Register));
                            }
                        }
                    }
                }
            }


            bool action_done = false, result;
            pair<bool, unique_ptr<Register>> result_pair;
            switch (CurrentAction) {
            case 1:
            case 2:
                if (CurrentAction == 1)  pDoc->SetDB_Action(0);
                else pDoc->SetDB_Action(1);
                
                result_pair = pDoc->RunModel_Using_Postgres(CurrentSimKey, move(p_Register));
                if(result_pair.first) action_done = true;
                p_Register = move(result_pair.second);
                break;
            case 3:
                if (pDoc->DeleteDoc_From_Postgres(CurrentSimKey)) action_done = true;
                break;
            case 4:
                if (pDoc->SelectDoc_From_Postgres(CurrentSimKey, true, path)) action_done = true;
                break;
            case 5:  //Download time series as csv
                if (pDoc->SelectDoc_From_Postgres(CurrentSimKey, true, path, true)) action_done = true;
                break;
            case 6:  
                action_done = true;
                break;// Download specific outputs
            case 7:
                break;
            
            case 8:
                if (LocalHost == 1) {
                    pDoc->SetLocalHost(false);
                    LocalHost = 0;
                }
                else {
                    pDoc->SetLocalHost(true);
                    LocalHost = 1;
                }
                p_Register=FUtil::WriteProfileInt("PostgresOnLocalHost", LocalHost, move(p_Register));

                action_done = true;
                break;
            case 9: 
                pDoc->ReDefinePostgresDataBase();
                action_done = true;
                break;
            case 10:

                break;
            }

            if (action_done)
                cout << " Action - " << Action[CurrentAction-1] << "  made" << endl;
            else 
                if(CurrentAction!=7) cout << " No Successful Action" << endl;

            cout << "Do you want to continue (Y/N) ?:";
            cin >> ans;

        }
        return 0;
    }
    return 0;
}