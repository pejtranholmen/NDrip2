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
    string ans;
    cout << "Do you want to Connect with Remote Database (Y/N) ?:";
    cin >> ans;
    auto pDoc = make_unique<Doc>();

    if (ans.find('y') != string::npos || ans.find('Y') != string::npos)
        pDoc->SetLocalHost(false);
    else
        pDoc->SetLocalHost(true);

    vector<pair<int, string>> simulations = pDoc->GetDataBaseSimulations();

    auto numsim = simulations.size();

    cout << to_string(numsim) << " simulations in database" << endl;;

    for (size_t i = 0; i < numsim; i++) {
        cout << to_string(simulations[i].first) << " - " << simulations[i].second << endl;
    }

    ans = "1";

    while (ans.size() > 0) {
        cout << "Select simulation by Id_number :";
        cin >> ans;
        int selectedkey;
        if (ans.size() > 0) {
            selectedkey = FUtil::AtoInt(ans);
            cout << "Delete selected Document (Y/N) :";
            cin >> ans;
            if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                if (pDoc->DeleteDoc_From_Postgres(selectedkey))
                    cout << "Deleted document with key :" << selectedkey << endl;
                else
                    cout << "Failed to Delete document :" << selectedkey << endl;
            }
            else {
                cout << "Download selected Document (Y/N) :";
                cin >> ans;
                if (ans.find('y') != string::npos || ans.find('Y') != string::npos) {
                    if (pDoc->SelectDoc_From_Postgres(selectedkey, true, path))
                        cout << "Selected document with key :" << selectedkey << endl;
                    else
                        cout << "Failed to Select document :" << selectedkey << endl;
                }
                else
                    ans.clear();
            }
        }
    }
    return 0;
}
