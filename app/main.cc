
using namespace std;
#include <iostream>;
#include "Util/SimUtil.hpp"
#include "NewBase/NewMap.h"
#include "Util/StatUtil.h"

#include "NewBase/SimArchive.h"
#include "SoilData/PlotPF/PFCurve.h"
#include "Model/FunctionProperties.h"

#include "SoilData/GetSoilStorage.h"

#include "PG/FileImport/PGFileImport.h"
#include "PG/PGUtil.h"
#include "./NewBase/MultiRun/MultiStorageStatData.h"
#include <chrono>
#ifdef MS_CODE
#include <direct.h>

#else
#include <filesystem>
#include <unistd.h>

#endif

#include "Util/Constants.h"

#include "config.hpp"
#include "Simulator/DefineModel.h"
#include "Model/Structure/StructureModel.h"

#include <nlohmann/json.hpp>


void readJson(string jsonFileName, Doc* pDoc) {

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
}
vector<string> merge_filter_datafiles(vector<string> files) {
    string climatefile, validationfile;
    vector<string> out;
    auto basenamepos = files[0].find("_LF");
    climatefile = files[0].substr(0, basenamepos)+"_forcing.txt";
    validationfile= files[0].substr(0, basenamepos) + "_validation.txt";
    std::fstream  in_file;
    //std::ofstream clim_stream, val_stream;
    
    //climatefile = FUtil::check_open_newcsvfile(&clim_stream, climatefile);
    //validationfile = FUtil::check_open_newcsvfile(&val_stream, validationfile);
    std::ofstream clim_stream(climatefile);
    ofstream val_stream(validationfile);
    out.push_back(climatefile); out.push_back(validationfile);

    

    string line_in, line_out;
   
    vector<string> str_v, units, names;
    vector<float> float_v;
    vector<size_t> commapos;
    vector<size_t> valpos{3,6,9,12,13,14,20,52,65,69,72,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,100,101,102};
    vector<size_t> climpos{23,25,29,30,36,39,48,54,56,64,66,68,79,80};
    size_t count = 0, super_count = 0;
    for (size_t ifil = 0; ifil < files.size(); ifil++) {
        in_file.open(files[ifil], ios::in);
        count = 0;
        getline(in_file, line_in);
        count++;

        while (getline(in_file, line_in)) {
            commapos = FUtil::getcommapositions(line_in);
            FUtil::trim(line_in);
            FUtil::trim_column(line_in);
            if (count < 4) {
                str_v = FUtil::GetStringVectorFromStringLine(line_in, commapos.size()+1);
                if (count == 1) names = str_v;
                else if (count == 2) units = str_v;
                if (count == 3 && super_count == 0) {
                    if (clim_stream.is_open()) {
                        string provline;
                        clim_stream << "TimeStamp,";
                        val_stream << "TimeStamp, ";
                        for (size_t i = 0; i < valpos.size(); i++) {
                            val_stream << names[valpos[i]];
                            if (i < valpos.size() - 1) val_stream << ",";
                        }
                        for (size_t i = 0; i < climpos.size(); i++) {
                            clim_stream << names[climpos[i]];
                            if (i < climpos.size() - 1) clim_stream << ",";
                        }
                        val_stream << "\n"; clim_stream << "\n";
                        clim_stream << "-,";
                        val_stream << "-,";
                        for (size_t i = 0; i < valpos.size(); i++) {
                            val_stream << units[valpos[i]];
                            if (i < valpos.size() - 1) val_stream << ",";
                        }
                        for (size_t i = 0; i < climpos.size(); i++) {
                            clim_stream << units[climpos[i]];
                            if (i < valpos.size() - 1) clim_stream << ",";
                        }
                        val_stream << "\n"; clim_stream << "\n";
                    }

                }
            }
            else {
                str_v = FUtil::GetStringVectorFromStringLine(line_in, commapos.size()+1);
                float_v = FUtil::GetFloatVectorFromString(line_in, commapos.size()+1);
                if (clim_stream.is_open()) {
                    clim_stream << str_v[0].substr(0,16) + ",";
                    val_stream << str_v[0].substr(0,16) + ",";
                    for (size_t i = 0; i < valpos.size(); i++) {
                        val_stream << str_v[valpos[i]];
                        if (i < valpos.size() - 1) val_stream << ",";
                    }
                    for (size_t i = 0; i < climpos.size(); i++) {
                        clim_stream << str_v[climpos[i]];
                        if (i < climpos.size() - 1) clim_stream << ",";
                    }
                    val_stream << "\n";
                    clim_stream << "\n";
                }

            }
            count++;
        }
        
        super_count += count;
        in_file.close();
    }
    clim_stream.close();
    val_stream.close();


    return out;
}



int main(int argc, char *argv[])
{
    using namespace std;

    // Adding path var
    string path = "C:\\Dev\\NewTest\\out\\build\\x64-debug\\bin";
    string simFilePath;
    string SimInputsFileNameJson;


    path = FUtil::GetCurrentPath();
    auto kolla = FUtil::GetFileList(".Sim");

    for (size_t i = 0; i < 2; i++) {
       
        path = "C:\\Dev\\Perennial data\\Perennial "+FUtil::STD_ItoAscii(i+1);
        FUtil::SetDocumentPath(path);
        kolla = FUtil::GetFileList(".dat", path);
        vector<string> outfile = merge_filter_datafiles(kolla);
        PGUtil::createInputBinFile(outfile[0], 17);
        PGUtil::createInputBinFile(outfile[1], 17);
    }
    
    
    
    /*
#ifndef COUP_POSTGRES
#ifdef LINUX2
    string pfile = string(argv[0]);
    auto pos=pfile.rfind("/");
    path = pfile.substr(0, pos)+"/";

#endif
    simFilePath = path + "Main_000000.Sim";

    // Setting global OPTIONS
    FUtil::WriteProfileStringStd("WorkingDirectory", path);
    FUtil::WriteProfileStringStd("UserDirectory", path);
    FUtil::WriteProfileStringStd("UserSubDirectory", path);
    FUtil::WriteProfileStringStd("DataBaseDirectory", path);
    FUtil::WriteProfileInt("SimulationRunNo", 1);

    int i = 0;
    auto pDoc = SimUtil::CreateDoc(i, simFilePath);

    string jsonfile = path + "par.json";
    readJson(jsonfile, pDoc);


    auto koll=PGUtil::createInputBinFile(path + "CLIMATE.TXT");
    koll=PGUtil::createInputBinFile(path + "FERTILIZERS.TXT");
    koll=PGUtil::createInputBinFile(path + "SCIROOT.TXT");

    SimUtil::SimProc(0, pDoc, false);

    pDoc->WriteDocFile();
#else

    
    auto koll=FUtil::GetFileList(".");
    
     Doc* pDoc = new Doc();
        vector<pair<int,string>> a=pDoc->GetDataBaseSimulations();
        pDoc->SelectDoc_From_Postgres(4);
        pDoc->MakeSingleRun(true);
     
#endif       */

  
    return 0;
}
