



#include <NewBase/CommonModelInfo.h>
#include <NewBase/ModelMap.h>
#include <NewBase/Doc.h>
#include <Simulator/DefineModel.h>
#include <NewBase/ModelCompNames.hpp>
#include <PG/PG.h>
#include <pqxx/pqxx>
#include <Util/CoupModel_DB_Link.hpp>
using namespace pqxx;

int main(int argc, char *argv[]) {

    unique_ptr<NewMap> pDoc(new Doc());
    CommonModelInfo* pinfo{ nullptr };
    if (pDoc->m_pCommonModelInfo == nullptr) {
        auto pP = pDoc->GetPPointer("Upper Depth");
        if (pP != nullptr)	pinfo = pP->getCommonModelPointer();
    }
    else
        pinfo = pDoc->m_pCommonModelInfo;

    
   int ikoll = create_Init_Tables(pinfo);
   ikoll = query_createtable(SWITCH, "Switches");
   vector<SimB*> v_Allsimb = pDoc->GetAllPtr(SWITCH);

   pinfo->DefineSwitchMap(v_Allsimb);
   ikoll = query_inserttable(SWITCH, "Switches", v_Allsimb, pDoc->m_pCommonModelInfo);

   ikoll = query_createtable(PAR_SINGLE, "singleparameters");
   v_Allsimb = pDoc->GetAllPtr(PAR_SINGLE);
   pinfo->DefineSingleParMap(v_Allsimb);
   ikoll = query_inserttable(PAR_SINGLE, "singleparameters", v_Allsimb, pDoc->m_pCommonModelInfo);

   ikoll = query_createtable(NUM_ELEMENTS_VECTOR, "NE_Vectors");
   v_Allsimb = pDoc->GetAll_NEPointers();
   pinfo->DefineNEVectorMap(v_Allsimb);

   ikoll = query_inserttable(NUM_ELEMENTS_VECTOR, "NE_Vectors", v_Allsimb, pDoc->m_pCommonModelInfo);

   ikoll = query_createtable(PAR_TABLE, "vectorparameters");
   v_Allsimb = pDoc->GetAllPtr(PAR_TABLE);
   pinfo->DefineVectorParMap(v_Allsimb);
   ikoll = query_inserttable(PAR_TABLE, "vectorparameters", v_Allsimb, pDoc->m_pCommonModelInfo);

   ikoll = query_createtable(STATE, "VectorOutputs");
   v_Allsimb = pDoc->GetAllPtr(STATE, FLOW, AUX, DRIVE);
   pinfo->DefineVectorOutputMap(v_Allsimb);
   ikoll = query_inserttable(STATE, "VectorOutputs", v_Allsimb, pDoc->m_pCommonModelInfo);



   ikoll = query_createtable(STATE_SINGLE, "SingleOutputs");
   v_Allsimb = pDoc->GetAllPtr(STATE_SINGLE, FLOW_SINGLE, AUX_SINGLE, DRIVE_SINGLE);
   pinfo->DefineSingleOutputMap(v_Allsimb);
   ikoll = query_inserttable(STATE_SINGLE, "SingleOutputs", v_Allsimb, pDoc->m_pCommonModelInfo);

   ikoll = query_createtable(PGFILE, "TimeSeriesInputs");
   v_Allsimb = pDoc->GetAllPtr(PGFILE);
   pinfo->DefineTimeSeriesMap(v_Allsimb);
   ikoll = query_inserttable(PGFILE, "TimeSeriesInputs", v_Allsimb, pDoc->m_pCommonModelInfo);


   ikoll = create_Main_Tables(pinfo);

   ikoll = create_Additional_Tables(pinfo, move(pDoc));


     
}
