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

    vector<pair<int, string>> simulations=pDoc->GetDataBaseSimulations();

    auto numsim = simulations.size();

    cout << to_string(numsim) << " simulations in database" << endl;;
    
    for (size_t i = 0; i < numsim; i++) {
        cout << to_string(simulations[i].first) <<" - "<< simulations[i].second << endl;
    }
    cout << "Select simulation by Id_number :";
    cin >> ans;
    int selectedkey = FUtil::AtoInt(ans);
    pDoc->SelectDoc_From_Postgres(selectedkey, true, path);
    

    return 0;

}
