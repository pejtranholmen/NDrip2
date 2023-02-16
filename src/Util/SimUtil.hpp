#pragma once

#include "NewBase/NewMap.h"
#include "NewBase/Doc.h"

#include "Simulator/ModelInfo.h"
#include "Util/FUtil.hpp"

namespace SimUtil {
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
                if ((str.find("Y") != string::npos) || (str.find('y') != string::npos)) out = true;
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
}
