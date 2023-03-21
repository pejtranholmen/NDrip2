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
    int CurrentAction, CurrentSimKey;
    auto pDoc = make_unique<Doc>();
    {
        unique_ptr<Register> p_Register = make_unique<Register>();

        path = FUtil::GetCurrentPath();
        auto kolla = FUtil::GetFileList(".xml");
        p_Register = FUtil::WriteProfileString("LocalDirectory", path, move(p_Register));
  

        pair<int, unique_ptr<Register>> p = FUtil::GetProfileIntNo("Action", 1, move(p_Register));
        CurrentAction = p.first; p_Register = move(p.second);


        if (MS_CODE) {
            vector<string> Action;
            Action.push_back(" - ReRun the Selected Simulation - Overwriting old results");
            Action.push_back(" - ReRun the Selected Simulation - creating new records");
            Action.push_back(" - Delete Selected Record ");
            Action.push_back(" - Download all files linked to Selected Record");
            Action.push_back(" - Download output time series as CSV files");
            Action.push_back(" - Download Specific outputs ");
            Action.push_back(" - Only Viewing Simulations in database");

            cout << "Action to be made: " << Action[CurrentAction - 1] + " (Y/N): ";
            cin >> ans;

            if (ans.find('y') != string::npos || ans.find('Y') != string::npos);
            else
            {
                cout << "List of possible actions:" << endl;
                size_t id = 1;
                for (auto line : Action) { cout << to_string(id) << line << endl; id++; }
                cout << "Specify your choice of action :";
                cin >> ans;
                int newaction = FUtil::AtoInt(ans);
                if (newaction > 0 && newaction < 8) {
                    p_Register = FUtil::WriteProfileInt("Action", newaction, move(p_Register));
                    CurrentAction = newaction;
                }
                else
                    return 0;
            }
        }


        

        p = FUtil::GetProfileIntNo("PostgresOnLocalHost", 1, move(p_Register));
        int LocalHost = p.first; p_Register = move(p.second);
        if (LocalHost == 0)
            pDoc->SetLocalHost(false);
        else
            pDoc->SetLocalHost(true);



        vector<pair<int, string>> simulations = pDoc->GetDataBaseSimulations();
        auto numsim = simulations.size();

        p = FUtil::GetProfileIntNo("SimKey", 0, move(p_Register));
        CurrentSimKey = p.first; p_Register = move(p.second);
    
    if (MS_CODE) {
        bool keyexist = false; size_t MaxKeyValue{ 0 };
        cout << to_string(numsim) << " simulations in database" << endl;;
        for (size_t i = 0; i < numsim; i++) {
            cout << to_string(simulations[i].first) << " - " << simulations[i].second << endl;
            if (MaxKeyValue < simulations[i].first) MaxKeyValue = simulations[i].first;
            if (simulations[i].first == CurrentSimKey) keyexist = true;
        }

        ans = "Y";
        if (keyexist) {
            cout << " A simulation is identified key id : " << to_string(CurrentSimKey) << " Continue with action (Y/N) :";
            cin >> ans;
            if (ans.find('y') != string::npos || ans.find('Y') != string::npos);
            else
            {
                cout << " Select new number or (0 = quit) :";
                cin >> ans;
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
            else return 0;
        }




        ans = "1";
    }
    }
    bool action_done = false;
        switch (CurrentAction) {
        case 1:
        case 2:
            if (CurrentAction == 1)  pDoc->SetDB_Action(0);
            else pDoc->SetDB_Action(1);
            if(pDoc->RunModel_Using_Postgres(CurrentSimKey)) action_done=true;
            break;
        case 3:
            if (pDoc->DeleteDoc_From_Postgres(CurrentSimKey)) action_done = true;
            break;
        case 4:
            if (pDoc->SelectDoc_From_Postgres(CurrentSimKey, true, path)) action_done = true;
            break;
        case 5:  //Download time series as csv
            if (pDoc->SelectDoc_From_Postgres(CurrentSimKey, true, path,true)) action_done = true;
            break;

        case 6:  // Download specific outputs
        case 7:
            break;
        }

        if (action_done)
            cout << " Action Made "<<endl;
        else
            cout << " No Successful Action"<<endl;


        cout << "Do you want to make new Settings (Y/N) ?:";
        cin >> ans;
        if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
            cout << "Do you want to Connect with Remote Database (Y/N) ?:";
            cin >> ans;
            unique_ptr<Register> p_Register = make_unique<Register>();
            if (ans.find('y') != string::npos || ans.find('Y') != string::npos)
                p_Register= FUtil::WriteProfileInt("PostgresOnLocalHost", 0, move(p_Register));
            else
                p_Register= FUtil::WriteProfileInt("PostgresOnLocalHost", 1, move(p_Register));
        }
        return 0;
}