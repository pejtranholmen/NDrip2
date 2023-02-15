
using namespace std;
#include "NewBase/Doc.h"
#include "Simulator/ModelInfo.h"
#include "Util/FUtil.hpp"
#include "Util/FileSystemUtil.hpp"
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
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>



void SimProc(size_t i, Doc* pDoc, bool Multi)
{
    if (Multi) {
        pDoc->CheckAndUpdateFileName(true);
        pDoc->MakeMultiRun();
    }
    else {
        pDoc->CheckAndUpdateFileName(false);
        pDoc->MakeSingleRun();
    }
}

Doc* CreateDoc(size_t i, string str) {
    Doc* pDoc;
    pDoc = new Doc();
    pDoc->SetCurrentFileName(str);

    bool makesim;
    makesim = pDoc->ReadDocFile(pDoc->GetCurrentSimFile());

    if (!makesim) {
        cout << "\n";
        cout << "Simulation file not correctly read - Check File !";
        cin >> str;
        return NULL;
    }

    return pDoc;
}

bool MakeMulti(size_t i, Doc* pDoc) {
    string str = "N";
    bool out;
    out = false;
    if (i == 0) {
        if (pDoc->m_MultiRun_Array.size() > 0) {
            cout << "\n";
            if ((str.find("Y") != string::npos) || (str.find('y') != string::npos) ) out = true;
            pDoc->SetMultiSimulation();
        }
    }
    else if (pDoc->m_MultiRun_Array.size() > 0) {
        Ps* pPs;
        pPs = pDoc->GetPsPointer("Random Seed");
        double v = pPs->GetValue() + i + 0.00001;
        pPs->SetValue(v);
        out = true;
        pDoc->SetMultiSimulation();
    }
    return out;
}

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

int main(int argc, char *argv[])
{
   cxxopts::Options options("test", "A brief description");
   options.add_options()
        ("b,bar", "Param bar", cxxopts::value<std::string>())
        ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))
        ("f,foo", "Param foo", cxxopts::value<int>()->default_value("10"))
        ("h,help", "Print usage")
        ;

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    bool debug = result["debug"].as<bool>();
    std::string bar;
    if (result.count("bar"))
        bar = result["bar"].as<std::string>();
    int foo = result["foo"].as<int>();

    using namespace std;

    // Adding path var
    string path = "C:\\Dev\\NewTest\\out\\build\\x64-debug\\bin";
    string simFilePath;
    string SimInputsFileNameJson;

    path = FileSystemUtil::GetCurrentPath();
    auto kolla = FileSystemUtil::GetFileList(".Sim");

    simFilePath = path + "Main_000000.Sim";

    // Setting global OPTIONS
    FUtil::WriteProfileStringStd("WorkingDirectory", path);
    FUtil::WriteProfileStringStd("UserDirectory", path);
    FUtil::WriteProfileStringStd("UserSubDirectory", path);
    FUtil::WriteProfileStringStd("DataBaseDirectory", path);
    FUtil::WriteProfileInt("SimulationRunNo", 1);

    int i = 0;
    Doc* pDoc = CreateDoc(i, simFilePath);

    string jsonfile = path + "par.json";
    readJson(jsonfile, pDoc);

    auto koll=PGUtil::createInputBinFile(path + "CLIMATE.TXT");
    koll=PGUtil::createInputBinFile(path + "FERTILIZERS.TXT");
    koll=PGUtil::createInputBinFile(path + "SCIROOT.TXT");

    SimProc(0, pDoc, false);

    pDoc->WriteDocFile();
    pDoc->WriteDoc_To_Postgres();

    //*
    return 0;
}
