using namespace std;
#include "./Util/SimUtil.hpp"

#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Sim.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include "./FUtil.hpp"
#include <Util/ModelUtil.h>

#include <PG/PG.h>
#include "config.hpp"

#include <nlohmann/json.hpp>

unique_ptr<Doc> ApplyNewTimeSerieName(F* pF, unique_ptr<Doc> pDoc) {
    string ans;
    auto pPG = pF->GetPointer();
    if (pPG != nullptr) {
        cout << "Current name :" << pPG->GetFileName();
        cout << "    new name :";
        cin >> ans;
        if (ans.size() > 1) {

            pPG->SetFileName(ans);
            pF->SetValue(ans);
        }
    }
    return move(pDoc);
}

unique_ptr<Doc> ApplyNewVectorOutValue(OutVector* pOut, unique_ptr<Doc> pDoc) {
    auto pNE = pOut->GetNEPointer();
    auto numvalues = pNE->GetNEValue();
    auto nameindex = pNE->GetName();

    string ans;
    cout << "Current Number of elements : " << nameindex << " is : " << to_string(numvalues) << endl;
    cout << "Set common choice for all elements (y/n):";
    cin >> ans;
    if (ans.find("Y") == 0 || ans.find("y") == 0) {
            cout << "index " + to_string(1) + "current flag: " + to_string(pOut->GetStoreFlag(0)) + " change all to :" ;
            cin >> ans;
            auto ians = FUtil::AtoInt(ans);
            if (ians >= 0 && ians <= 2) for (size_t i = 0; i < numvalues; i++) pOut->SetStoreFlag(i, ians);
    }
    else {
        for (size_t i = 0; i < numvalues; i++) {
            cout << "index " + to_string(i + 1) + "current flag: " + to_string(pOut->GetStoreFlag(i)) + " change to :";
            cin >> ans;
            auto ians = FUtil::AtoInt(ans);
            if (ians >= 0 && ians <= 2) {
                pOut->SetStoreFlag(i, ians);
                pDoc->History_Add(static_cast<SimB*>(pOut), i, time(nullptr), to_string(pOut->GetStoreFlag(i)));
            }
        }
    }
    return move(pDoc);
}

unique_ptr<Doc> ApplyNewSingleOutValue(OutSingle* pOut, unique_ptr<Doc> pDoc) {
    string ans;
    cout << "Current value of : " << pOut->GetName() << " is : " << to_string(pOut->GetStoreFlag()) << endl;
    cout << "Your choice (0-No output, 1-SingleRun, 2- MultiRun)  :";
    cin >> ans;
    auto ians = FUtil::AtoInt(ans);
    if (ians >= 0 && ians <= 2) {
        pOut->SetStoreFlag(ians);
        pDoc->History_Add(static_cast<SimB*>(pOut), 0, time(nullptr), to_string(pOut->GetStoreFlag()));

    }
    return move(pDoc);
}

unique_ptr<Doc> ApplyNewTableParValue(P* pP, unique_ptr<Doc> pDoc) {
    auto pNE = pP->GetNEPointer();
    auto numvalues = pNE->GetNEValue();
    auto nameindex = pNE->GetName();

    string ans;
    cout << "Current Number of elements : " << nameindex << " is : " << to_string(numvalues) << endl;
    cout << "Change number of elements by new number or type y to accept:";
    cin >> ans;
    if (ans.find("y") != 0) {
        auto newnum = FUtil::AtoInt(ans);
        if (newnum > pNE->GetMinValue() && newnum <= pNE->GetMaxValue()) {
            pNE->SetNEValue(newnum);
        }
    }
    bool changewithinloop = true;
    while (changewithinloop) {
        cout << "Specify index to change or type y to continue without change :";
        cin >> ans;
        if (ans.find("y") != 0) {
            auto newindex = FUtil::AtoInt(ans);
            if (newindex > pNE->GetMinValue() && newindex <= pNE->GetMaxValue()) {
                cout << "Current value of : " << pP->GetName() << " index : " << to_string(newindex) + " has value: " << to_string(pP->GetValue(newindex - 1)) << endl;
                cout << "Your new choice :";
                cin >> ans;
                auto value = FUtil::AtoDouble(ans);
                if (value >= pP->GetMinValue(newindex - 1) && value <= pP->GetMaxValue(newindex - 1))
                   pDoc= ModelUtil::SetParTableValue(move(pDoc), pP, newindex-1, value);
                 else
                    cout << " Outside valid range !";
            }
        }
        else return move(pDoc);
    }
    return move(pDoc);
}

unique_ptr<Doc> ApplyNewSingleParValue(Ps* pP, unique_ptr<Doc> pDoc) {
    string ans;
    cout << "Current value of : " << pP->GetName() << " is : " << to_string(pP->GetValue()) << endl;
    cout << "Your choice :";
    cin >> ans;
    auto value = FUtil::AtoDouble(ans);
    if (value >= pP->GetMinValue() && value <= pP->GetMaxValue()) {
        pDoc = ModelUtil::SetParValue(move(pDoc), pP, value);
    }
       
    else
        cout << " Outside valid range !";
    return move(pDoc);
}


unique_ptr<Doc> ApplyNewSwValue(Sw* pSw, unique_ptr<Doc> pDoc) {
   auto n=pSw->GetNumOptions();
   string ans;
   cout << "Current value of : " <<pSw->GetName()<<" is : "<< pSw->GetOption(pSw->GetIntValue()) << endl;
   
   for (size_t i = 0; i < n; i++) {
       cout << to_string(i + 1) + " - " + pSw->GetOption(i) << endl;
   }
   cout << "Your choice :";
   cin >> ans;
   auto ichoice = FUtil::AtoInt(ans);
   if (ichoice > 0 && ichoice <= n) {
       size_t recalc =pSw->SetIntValue(ichoice - 1);
       if (recalc > 0)
           pDoc->LinkedChangeToSwitches(pSw, recalc);
        pDoc->History_Add(pSw, int(HIST_INFO::SWITCH_INFO), time(nullptr), pSw->GetOption(ichoice - 1));
   }
   else
       cout << "No valid choice !";
   return move(pDoc);
}

unique_ptr<Doc> ChangeValues(simtype changetype, unique_ptr<Doc> pDoc) {
  auto ptr = pDoc->GetPtrVector(changetype, false);
   vector<string> items;
   string item, ans, typetochange;
    for (auto pSim : ptr) {
        item = pSim->GetName();
        if (changetype == SWITCH) {
            Sw* pSw = static_cast<Sw*>(pSim);
            item+= "- "+pSw->GetOption(pSw->GetIntValue());           
            typetochange = "Switch";
        }
        else if (changetype == PAR_SINGLE) {
            Ps* pP = static_cast<Ps*>(pSim);
            item += " - " + to_string(pP->GetValue());
            typetochange = "Parameter";
        }
        else if (changetype == PAR_TABLE) {
            P* pP = static_cast<P*>(pSim);
            auto nep = pP->GetNEPointer();
            auto numvalues=nep->GetNEValue();
            auto nameindex = nep->GetName();
            item += " - " + to_string(numvalues)+" of "+ nameindex;
            typetochange = "Parameter Vector";
        }
        else if (changetype >= STATE && changetype <= DRIVE) {
            OutVector* pOut = static_cast<OutVector*>(pSim);
            auto pNE = pOut->GetNEPointer();
            auto numvalues = pNE->GetNEValue();
            auto nameindex = pNE->GetName();
            item += " - " + to_string(numvalues) + " of " + nameindex;
            if (changetype == STATE) typetochange = "State Vector";
            else if (changetype == FLOW) typetochange = "Flow Vector";
            else if (changetype == AUX) typetochange = "Auxiliary Vector";
            else if (changetype == DRIVE) typetochange = "Driving Vector";
        }
        else if (changetype >= STATE_SINGLE && changetype <= DRIVE_SINGLE) {
            OutSingle* pOut = static_cast<OutSingle*>(pSim);
            auto storeflag=pOut->GetStoreFlag();
            item += " - " + to_string(storeflag);
            if (changetype == STATE_SINGLE) typetochange = "State";
            else if (changetype == FLOW_SINGLE) typetochange = "Flow";
            else if (changetype == AUX_SINGLE) typetochange = "Auxiliary";
            else if (changetype == DRIVE_SINGLE) typetochange = "Driving";
        }
        else if (changetype == PGFILE) {
            F* pF = static_cast<F*>(pSim);
            item += " - " + pF->GetStrValue();
            typetochange = "Time Serie";
        }
        else if (changetype == HEADER_INPUTS) {
 
        }
        items.push_back(item);
    }
    auto listoptions = [&items]() {
        size_t count = 1;
        for (auto opt : items) {
            cout << to_string(count) + " - " + opt << endl;
            count++;
        }
    };

    bool keepon = true;
    while (keepon) {
        listoptions();
        cout << "Select one "+typetochange + " by number(#) or (Y) to continue: ";
        cin >> ans;
        if (ans.find("Y")==0 || ans.find("y")==0) return move(pDoc);
        int iselect = FUtil::AtoInt(ans);
        if (iselect > 0 && iselect <= ptr.size()) {
            switch (changetype) {
                case SWITCH:
                    pDoc = ApplyNewSwValue(static_cast<Sw*>(ptr[iselect - 1]), move(pDoc));
                break;
                case PAR_SINGLE:
                    pDoc = ApplyNewSingleParValue(static_cast<Ps*>(ptr[iselect - 1]), move(pDoc));
                    break;
                case PAR_TABLE:
                    pDoc = ApplyNewTableParValue(static_cast<P*>(ptr[iselect - 1]), move(pDoc));
                    break;
                case STATE:
                case FLOW:
                case AUX:
                case DRIVE:
                    pDoc = ApplyNewVectorOutValue(static_cast<OutVector*>(ptr[iselect - 1]), move(pDoc));
                    break;
                case STATE_SINGLE:
                case FLOW_SINGLE:
                case AUX_SINGLE:
                case DRIVE_SINGLE:
                    pDoc = ApplyNewSingleOutValue(static_cast<OutSingle*>(ptr[iselect - 1]), move(pDoc));
                    break;
                case PGFILE:
                    pDoc = ApplyNewTimeSerieName(static_cast<F*>(ptr[iselect - 1]), move(pDoc));
                    break;
            }
        }
        else {
            listoptions();

        }
  

    }


    return move(pDoc);
}

unique_ptr<Doc> ChangeHeaderValues(unique_ptr<Doc> pDoc) {
    Sw* pSw;
    int iv, idays, preminutes;
    cout << "Header Items" << endl;
    vector<string> items;
    string item, ans;
    for (size_t i = 0; i < 11; ++i) {
        if (i == 1) continue;
        item = to_string(i + 1) + " - " + pDoc->m_RunInfo[i].Name+" : ";
        switch (i) {
            case 0:
                item += to_string(pDoc->m_DocFile.m_SimulationRunNo);
                break;
            case 2:
                item += PGUtil::StringDatum(pDoc->RunOpt.longMinStart);
                break;
            case 3:
                item += PGUtil::StringDatum(pDoc->RunOpt.longMinEnd);
                break;
            case 4:
                preminutes = pDoc->m_DocFile.ipredays * 1440;
                item += PGUtil::StringDatum(pDoc->RunOpt.longMinStart-preminutes);
            case 5:
               
                item += to_string(pDoc->m_DocFile.m_TimePeriodScaleFactor);
                break;
            case 6:
                pSw = dynamic_cast<Sw*>(pDoc->GetPtr(simtype::SWITCH, "TimeResolution"));
                iv = pSw->GetIntValue();
                item += to_string(iv);
                break;
            case 7:
                item += to_string(pDoc->RunOpt.oiminutes + pDoc->RunOpt.oidays * 1440);
                break;
            case 8:
                item += to_string(pDoc->RunOpt.noofiter);
                break;
            case 9:
                item += pDoc->m_DocFile2.m_RunId;
                break;
            case 10:
                item += pDoc->m_DocFile2.m_Comments;
                break;
        }       
        cout << item << endl;
    }
    bool keepon = true;

    while (keepon) {
        cout << "Specify item (#) to change or (Y) to continue without changes";
        cin >> ans;
        if (ans.find("Y") == 0 || ans.find("y") == 0) return move(pDoc);
        int ichoice = FUtil::AtoInt(ans)-1;

        switch (ichoice) {
            case 0:
                cout << " New RunNo: ";
                cin >> ans;
                pDoc->m_DocFile.m_SimulationRunNo = FUtil::AtoInt(ans);
                break;
            case 2:
                cout << " Start Date: ";
                cin >> ans;
                pDoc->RunOpt.longMinStart=PGUtil::MinutConv(PGUtil::Ato_IPGTID(ans));
                break;
            case 3:
                cout << " End Date: ";
                cin >> ans;
                pDoc->RunOpt.longMinEnd = PGUtil::MinutConv(PGUtil::Ato_IPGTID(ans));
                break;
            case 4:
                cout << " Pre period (days): ";
                cin >> ans;
                pDoc->m_DocFile.ipredays = FUtil::AtoInt(ans);
                break;
            case 5:
                cout << " Scaling of sim period: ";
                cin >> ans;
                pDoc->m_DocFile.m_TimePeriodScaleFactor = FUtil::AtoInt(ans);
                break;
            case 6:
                cout << " Time Resolution index: ";
                cin >> ans;
                pSw = dynamic_cast<Sw*>(pDoc->GetPtr(simtype::SWITCH, "TimeResolution"));
                iv = pSw->SetIntValue(FUtil::AtoInt(ans));
            case 7:
                cout << " Output Time Interval : ";
                cin >> ans;
                iv = FUtil::AtoInt(ans);
                idays = iv / 1440;
                pDoc->RunOpt.oidays = idays;
                if (idays > 0)
                    pDoc->RunOpt.oiminutes = iv - idays * 1440;
                else
                    pDoc->RunOpt.oiminutes = iv;               
                break;
            case 8:
                cout << " Number of iterations : ";
                cin >> ans;
                iv = FUtil::AtoInt(ans);
                pDoc->RunOpt.noofiter = iv;
                break;
            case 9:
                cout << " Short RunId : ";
                cin >> ans;              
                pDoc->m_DocFile2.m_RunId = ans;
                break;
            case 10:
                cout << " Comments : ";
                cin >> ans;
                pDoc->m_DocFile2.m_Comments = ans;
        }




    }
    return move(pDoc);
}


unique_ptr<Doc> readJson(string jsonFileName, unique_ptr<Doc> pDoc) {

    std::ifstream i(jsonFileName);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        cout << "setting simfile parameters:" << endl;

        for (int x = 0; x < j["params"].size(); x++)
        {
            cout << j["params"][x]["description"] << endl;

            if (j["params"][x]["type"] == "TABLE") {

                Tab* pTabi = dynamic_cast<Tab*>(pDoc->GetPtr(TABLE, j["params"][x]["name"]));
                if (pTabi != NULL) {
                    cout << pTabi->GetTitle(j["params"][x]["var_index"]) << endl;
                    P* pP = (P*)pTabi->GetP_Pointer(j["params"][x]["var_index"]);
                    cout << pP->GetValue(j["params"][x]["value_index"]) << endl;
                    pP->SetValue(j["params"][x]["value_index"], j["params"][x]["value"]);
                    cout << pP->GetValue(j["params"][x]["value_index"]) << endl;
                }
            }
            else if (j["params"][x]["type"] == "PAR_SINGLE") {

                Ps* pP = (Ps*)pDoc->GetPsPointer(j["params"][x]["name"]);
                if (pP != NULL) {
                    cout << pP->GetValue() << endl;
                    pP->SetValue(j["params"][x]["value"]);
                    cout << pP->GetValue() << endl;
                }

            }
            else if (j["params"][x]["type"] == "SWITCH") {

                Sw* pSw = dynamic_cast<Sw*>(pDoc->GetPtr(SWITCH, j["params"][x]["name"]));
                cout << pSw->GetIntValue() << endl;
                pSw->SetIntValue(j["params"][x]["value"]);
                cout << pSw->GetIntValue() << endl;
            }
            else if (j["params"][x]["type"] == "RUN_INFO") {

                if (j["params"][x]["name"] == "longMinStart") {
                    cout << pDoc->RunOpt.longMinStart << endl;
                    pDoc->RunOpt.longMinStart = PGUtil::MinutConv(j["params"][x]["value"]);
                    cout << pDoc->RunOpt.longMinStart << endl;
                }
                else if (j["params"][x]["name"] == "longMinEnd") {
                    cout << pDoc->RunOpt.longMinEnd << endl;
                    pDoc->RunOpt.longMinEnd = PGUtil::MinutConv(j["params"][x]["value"]);
                    cout << pDoc->RunOpt.longMinEnd << endl;
                }
                else if (j["params"][x]["name"] == "longMinPreStart") {
                    cout << pDoc->RunOpt.longMinPreStart << endl;
                    pDoc->RunOpt.longMinPreStart = PGUtil::MinutConv(j["params"][x]["value"]);
                    cout << pDoc->RunOpt.longMinPreStart << endl;
                }
                else if (j["params"][x]["name"] == "longMinPostEnd") {
                    cout << pDoc->RunOpt.longMinPostEnd << endl;
                    pDoc->RunOpt.longMinPostEnd = PGUtil::MinutConv(j["params"][x]["value"]);
                    cout << pDoc->RunOpt.longMinPostEnd << endl;
                }
            }
        }
    }
    else {
        cout << "No json file used, file is missing (par.json)";

    }
    return move(pDoc);
}

int main(int argc, char* argv[]) {
    string doc_file_name;
    string ans, path;
    int CurrentAction, CurrentSimKey, LocalHost, MakeMultiRunIfPossible;
    auto pDoc = make_unique<Doc>();
    {
        unique_ptr<Register> p_Register = make_unique<Register>();

        path = FUtil::GetCurrentPath();
        auto kolla = FUtil::GetFileList(".xml");
        p_Register = FUtil::WriteProfileString("LocalDirectory", path, move(p_Register));

        string test = "c:/Users/Admin/documents/Coupmodel/SoilProperties/";
        auto testtrue=FUtil::IsFileExisting(test + "pfprof.db");

        pair<string, unique_ptr<Register>> pst = FUtil::GetProfileStringStd("DataBaseDirectory", test, move(p_Register));
        p_Register = move(pst.second); 

        pair<int, unique_ptr<Register>> p = FUtil::GetProfileIntNo("Action", 1, move(p_Register));
        CurrentAction = p.first; p_Register = move(p.second);
        if (CurrentAction < 1 || CurrentAction>12) CurrentAction = 2;

        p = FUtil::GetProfileIntNo("PostgresOnLocalHost", 1, move(p_Register));
        LocalHost = p.first; p_Register = move(p.second);
        if (LocalHost == 0)
            pDoc->SetLocalHost(false);
        else
            pDoc->SetLocalHost(true);

        p = FUtil::GetProfileIntNo("MakeMultiRun_IfPossible", 1, move(p_Register));
        MakeMultiRunIfPossible = p.first; p_Register = move(p.second);
        if (MakeMultiRunIfPossible == 0)
            pDoc->SetMakeMultiRunIfPossible(false);
        else
            pDoc->SetMakeMultiRunIfPossible(true);

        string user = "PEJ";
        string Computer = "PEJ_Tranholmen";
        pst = FUtil::GetProfileStringStd("Signature",user , move(p_Register));
        
        p_Register = move(pst.second); user = pst.first;
        
        pst = FUtil::GetProfileStringStd("ComputerName", Computer, move(p_Register));
        p_Register = move(pst.second); Computer = pst.first;


        pst = FUtil::GetProfileStringStd("DataBaseDirectory", "", move(p_Register));
        string databasedir;
        p_Register = move(pst.second); databasedir = pst.first;
         pDoc->m_DataBaseDirectory = databasedir;

      




        ans = "y";
        while (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
            vector<string> Action;
            if (MS_CODE) {
                Action.clear();
                Action.push_back(" - ReRun the Selected Simulation - Overwriting old results");
                Action.push_back(" - Make o complete Copy (new parent Documents)");
                Action.push_back(" - Make a child copy (inheritance to parent Document)");
                Action.push_back(" - Download all files linked to Selected Record");
                Action.push_back(" - Download output time series as CSV files");
                Action.push_back(" - Upload a simulation document as xml and bin files:");
                Action.push_back(" - View of Simulations in database");
                if (LocalHost == 0)
                    Action.push_back(" - Change from remote to local database");
                else
                    Action.push_back(" - Change from local to remote database");
                if (MakeMultiRunIfPossible == 0)
                    Action.push_back(" - Change from SingleRun to MultiRuns (if possible)");
                else
                    Action.push_back(" - from Making MultiRuns (if possible) to SingleRun ");



                Action.push_back(" - Delete Selected Record ");
                Action.push_back(" - Reset Database (removes all items)");
                Action.push_back(" - Clean Database from unlinked units");
                Action.push_back(" - Soil Databas management");

                cout <<"Current Action is : "<<Action[CurrentAction-1]<<" See List of possible actions : (Y / N) : ";
                cin >> ans;
                if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                    size_t id = 1;
                    for (auto line : Action) { cout << to_string(id) << line << endl; id++; }
                }
                //else
                  //  CurrentAction-;
                
                cout <<"Current choice: "<<to_string(CurrentAction)<< " Specify your choice (#) to change action (Y to accept):";
                cin >> ans;
                if (ans.find('y') != string::npos || ans.find('Y') != string::npos);
                else {
                    int newaction = FUtil::AtoInt(ans);
                    if (newaction > 0 && newaction < 8) {
                        p_Register = FUtil::WriteProfileInt("Action", newaction, move(p_Register));
                    }
                    CurrentAction = newaction;

                }
                if (CurrentAction < 1 ) CurrentAction = 2;
            }
            vector<pair<int, string>> simulations = pDoc->GetDataBaseSimulations();
            auto numsim = simulations.size();
            p = FUtil::GetProfileIntNo("SimKey", 0, move(p_Register));
            CurrentSimKey = p.first; p_Register = move(p.second);

            bool setoptionsforsimulations = true;
            if (CurrentAction >= 8 && CurrentAction <= 13) setoptionsforsimulations = false;

            if (setoptionsforsimulations) {
                if (MS_CODE) {
                    bool keyexist = false; size_t MaxKeyValue{ 0 };
                    if (CurrentAction == 6) {
                        string doc_file_name;

                        auto path = FUtil::GetCurrentPath();
                        auto kolla = FUtil::GetFileList(".xml");


                        cout << kolla.size() << " xml files ready for upload - See List (Y/N):";
                        cin >> ans;
                        if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                            for (size_t i = 0; i < kolla.size(); i++) {
                                cout << FUtil::STD_ItoAscii(i + 1) << " : " << kolla[i] << endl;
                            }
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
                        

                        cout << "Do want to change settings for your upload to database (Y/N):";
                        cin >> ans;
                        if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                            cout << "Current Creator Id :" << currentcreator << " Enter string (min of 2 char) to change: ";
                            cin >> ans; if (ans.size() > 2)  p_Register = FUtil::WriteProfileString("Creator", ans, move(p_Register));
                            cout << "Current SiteName Id :" << current_site << " Enter string (min of 2 char) to change: ";
                            cin >> ans;  if (ans.size() > 2 && ans.size() < 25)   p_Register = FUtil::WriteProfileString("SiteNameId", ans, move(p_Register));
                            cout << "Current Comment Id :" << current_comment << " Enter string (min of 2 char) to change: ";
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
                        bool makelist = true;
                        if (CurrentAction != 7) {
                            cout << to_string(numsim) << " simulations in " << db << " database - View list  (Y/N) :";
                            cin >> ans; if (ans.find('y') != string::npos || ans.find('Y') != string::npos) makelist = true;
                        }
                        if (makelist) {
                            for (size_t i = 0; i < numsim; i++) {
                                cout << to_string(simulations[i].first) << " - " << simulations[i].second << endl;
                                if (MaxKeyValue < simulations[i].first) MaxKeyValue = simulations[i].first;
                                if (simulations[i].first == CurrentSimKey) keyexist = true;
                            }
                        }

                    }
                    if (CurrentAction < 6) {
                        if (keyexist) {
                            cout << " A simulation is identified with key id : " << to_string(CurrentSimKey) << " Continue with action (Y) or Give new Key(#) :";
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
            bool makechild_document = false;

            if (CurrentAction > 0 && CurrentAction < 4) {
                if (CurrentAction == 1)  pDoc->SetDB_Action(0);
                else pDoc->SetDB_Action(1);
                
                vector<int> keys = pDoc->PreSelectDoc_From_Postgres(CurrentSimKey);
                bool valid = true;
                bool first = true;
                for (auto currentkey : keys) {
                    if(first) if (!pDoc->SelectDoc_From_Postgres(currentkey, true, false)) valid = false;
                    else if (!pDoc->SelectDoc_From_Postgres(currentkey, false, false)) valid = false;
                    first = false;
                }
                if (valid) {
                    bool child = false;
                    if (CurrentAction == 3) child = true;

                    p_Register = pDoc->SetNewRunNo(true, CurrentSimKey,child, move(p_Register));
                    ans = "y";
                    size_t ChangeAction = 1, CurrentChangeAction;
                    pair<int, unique_ptr<Register>> p = FUtil::GetProfileIntNo("Change_Action", 1, move(p_Register));
                    CurrentChangeAction = p.first; p_Register = move(p.second);
                    while (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                        vector<string> Action;
                        if (MS_CODE) {
                            Action.clear();
                            Action.push_back(" - Change General inputs");
                            Action.push_back(" - Change Switches");
                            Action.push_back(" - Change Parameters");
                            Action.push_back(" - Change Outputs");
                            Action.push_back(" - Change Input Time Series");
                            Action.push_back(" - Changes from Json file");
                            Action.push_back(" - No Changes");
                        }
                        cout << "Current Change Action is : " << Action[CurrentChangeAction - 1] << " See List of options : (Y / N) : ";
                        cin >> ans;
                        if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                            size_t id = 1;
                            for (auto line : Action) { cout << to_string(id) << line << endl; id++; }
                        }
                        cout << "Current choice: " << to_string(CurrentChangeAction) << " Specify your choice (#) to change action (Y to accept):";
                        cin >> ans;
                        if (ans.find('y') != string::npos || ans.find('Y') != string::npos);
                        else {
                            int newaction = FUtil::AtoInt(ans);
                            if (newaction > 0 && newaction < 8) {
                                p_Register = FUtil::WriteProfileInt("Change_Action", newaction, move(p_Register));
                            }
                            CurrentChangeAction = newaction;
                        }
                        switch (CurrentChangeAction) {
                        case (1):
                            pDoc = ChangeHeaderValues(move(pDoc));
                        case (2):
                            pDoc = ChangeValues(SWITCH, move(pDoc));
                            break;
                        case (3):
                            pDoc = ChangeValues(PAR_SINGLE, move(pDoc));
                            pDoc = ChangeValues(PAR_TABLE, move(pDoc));
                            break;
                        case (4):
                            pDoc = ChangeValues(STATE_SINGLE, move(pDoc));
                            pDoc = ChangeValues(FLOW_SINGLE, move(pDoc));
                            pDoc = ChangeValues(AUX_SINGLE, move(pDoc));
                            pDoc = ChangeValues(DRIVE_SINGLE, move(pDoc));
                            pDoc = ChangeValues(STATE, move(pDoc));
                            pDoc = ChangeValues(FLOW, move(pDoc));
                            pDoc = ChangeValues(AUX, move(pDoc));
                            pDoc = ChangeValues(DRIVE, move(pDoc));
                            break;
                        case (5):
                            pDoc = ChangeValues(PGFILE, move(pDoc));
                            break;
                        case (6):
                            auto path = FUtil::GetCurrentPath() + "par.json";
                            pDoc = readJson(path, move(pDoc));
                            break;
                        }
                        cout << "Continue with more changes (y/n) :";
                        cin >> ans;                       
                    }
                }
            }

            switch (CurrentAction) {
            case 1:
            case 2:
            case 3:
               
                if (CurrentAction == 3) makechild_document = true;
                
                result_pair = pDoc->RunModel_Using_Postgres(CurrentSimKey, makechild_document, move(p_Register));
                if(result_pair.first) action_done = true;
                p_Register = move(result_pair.second);
                break;
          
        
                break;
            case 4:
                if (pDoc->SelectDoc_From_Postgres(CurrentSimKey, true, true, path)) action_done = true;
                break;
            case 5:  //Download time series as csv
                if (pDoc->SelectDoc_From_Postgres(CurrentSimKey, true, true, path, true)) action_done = true;
                break;
            case 6:  
                action_done = true;
                break;// Download specific outputs
            case 7:
                action_done = true;
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
                if (MakeMultiRunIfPossible == 1) {
                    pDoc->SetMakeMultiRunIfPossible(false);
                    MakeMultiRunIfPossible = 0;
                }
                else {
                    pDoc->SetMakeMultiRunIfPossible(true);
                    MakeMultiRunIfPossible = 1;
                }
                action_done = true;
                p_Register = FUtil::WriteProfileInt("MakeMultiRun_IfPossible", MakeMultiRunIfPossible, move(p_Register));
                break;

            case 10: 
                 if (pDoc->DeleteDoc_From_Postgres(CurrentSimKey)) action_done = true;
                break;
            case 11:
                pDoc->ReDefinePostgresDataBase();
                action_done = true;
                break;
            case 13:
                pDoc->Export_OLDSOILDB_toPostgres();
                action_done = true;
                break;

            }

            if (action_done)
                cout << " Action - " << Action[CurrentAction-1] << "  made" << endl;
            else 
                if(CurrentAction!=7) cout << " No Successful Action no: " <<to_string(CurrentAction)<< endl;

            cout << "Do you want to continue (Y/N) ?:";
            cin >> ans;


        }
        return 0;
    }
    return 0;
}