
#include "../Util/FUtil.hpp"
#include "Simulator.h"
#include "../NewBase/ModelMap.h"
#include "../NewBase/NewMap.h"
#include "../NewBase/Validation/ValidationData.h"
#include "../NewBase/MR.h"
#include "../ModelTypes/SimB.h"
#include "../Simulator/ModelInfo.h"
#include "../NewBase/Sim.h"

#ifndef COUPSTD
#include "../MainFrm.h"
#endif
//#include <algorithm> 


Simulator::Simulator(void)
{	
	m_pModelInfo = new ModelInfo();

	m_pModelStructure = &m_StructureModel.m_Structure;


	m_pModelInfo->AddModel("Soil Model", &m_SoilModel);
	m_pModelInfo->AddModel("Plant Model", &m_PlantModel);
	m_pModelInfo->AddModel("Atm Model", &m_AtmModel);
	m_pModelInfo->AddModel("Structure Model", &m_StructureModel);
	m_pModelInfo->AddModel("Plant Org Model", &m_PlantOrgModel);
	m_pModelInfo->AddModel("Soil Org Model", &m_SoilOrgModel);
   	m_pModelInfo->AddModel("HBV Model", &m_HBVModel);

	m_pValVar.resize(0);
	m_ShiftPointer=nullptr;
}
Simulator::~Simulator()
{

}
void Simulator::SetSimPointer(ModelBase *simptr)
{
	  m_pSim=simptr;
	  m_pModelMap =(ModelMap*)simptr;




	  m_AtmModel.m_Irrigation.setIrrigation(m_pModelInfo, m_pModelMap, m_pModelStructure);
	  m_SoilModel.m_SoilFrost.setSoil_Frost(m_pModelInfo, m_pModelMap, m_pModelStructure); 
	  m_SoilModel.m_HeatPump.setSoil_HeatPump(m_pModelInfo, m_pModelMap, m_pModelStructure);
	  m_HBVModel.m_HBV.setHBV(m_pModelInfo, m_pModelMap, m_pModelStructure);

	  m_PlantModel.m_WaterUptake.setWaterUptake(m_pModelInfo, m_pModelMap, m_pModelStructure);
	  m_StructureModel.m_Additional.setAdditional_BioticVariables(m_pModelInfo, m_pModelMap, m_pModelStructure);
	 // m_PlantOrgModel.m_NC_Drivings.setNC_Drivings(m_pModelInfo, m_pModelMap, m_pModelStructure);
	  m_SoilOrgModel.m_NC_SoilOrg.setNC_SoilManagement(m_pModelInfo, m_pModelMap, m_pModelStructure); 
	  m_PlantOrgModel.m_NC_Plant.setNC_Plant(m_pModelInfo, m_pModelMap, m_pModelStructure); 
	  m_StructureModel.m_Structure.setStructure(m_pModelInfo, m_pModelMap, m_pModelStructure);


	  m_SoilModel.m_SoilFrost.Def();
	  m_SoilModel.m_HeatPump.Def();
	  m_PlantModel.m_WaterUptake.Def();
	  m_AtmModel.m_Irrigation.Def();
	  //plantorgmodel.m_NC_Drivings.Def();
	  m_PlantOrgModel.m_NC_Plant.Def();
	  m_SoilOrgModel.m_NC_SoilOrg.Def();

	  m_StructureModel.m_Structure.Def();
	  m_StructureModel.m_Additional.Def();
	  m_HBVModel.m_HBV.Def();


	  ((NewMap*)m_pSim)->MakeMapComplete();

}
bool Simulator::Start(bool MultiFlag, bool First)
{

#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	CCoupModelApp *pCoupModel = (CCoupModelApp*)pApp;
	CMainFrame *p_mainframe;
	p_mainframe = (CMainFrame*)pCoupModel->m_pMainWnd;
#endif


	m_pModelInfo->Set_SimStart_Parameters();
	m_pModelInfo->m_OutInterval=(m_pSim->RunOpt.oiminutes+m_pSim->RunOpt.oidays*1440);


	m_pModelInfo->m_OutRecord=(m_pSim->RunOpt.longMinPostEnd-m_pSim->RunOpt.longMinStart)/ m_pModelInfo->m_OutInterval;
	m_pModelInfo->m_RecOutput=0;
	
	if(m_pModelInfo->m_OutRecord* m_pModelInfo->m_OutInterval!=m_pSim->RunOpt.longMinPostEnd-m_pSim->RunOpt.longMinStart) {
		auto newend = m_pSim->RunOpt.longMinStart + m_pModelInfo->m_OutRecord * m_pModelInfo->m_OutInterval;
		auto diff = m_pSim->RunOpt.longMinPostEnd - newend;
		m_pSim->RunOpt.longMinPostEnd = newend;
		m_pSim->RunOpt.longMinEnd -= diff;
	}
	m_pModelInfo->T_Current=double(m_pSim->RunOpt.longMinPreStart/1440);
	m_pModelInfo->T_Start=double(m_pSim->RunOpt.longMinPreStart/1440);
	m_pModelInfo->T_StartSimPeriod = double(m_pSim->RunOpt.longMinStart / 1440);
	m_pModelInfo->JDayNum=PGUtil::DayNumFunction(m_pSim->RunOpt.longMinPreStart);
	m_pModelInfo->T_End=double(m_pSim->RunOpt.longMinPostEnd/1440);
	
	m_pModelInfo->m_ActMin=1440*int(m_pModelInfo->T_Current);
	
	m_pModelInfo->T_Step=1./m_pSim->RunOpt.noofiter;
	m_DayError=1.;
	for(int i=0; i<m_pSim->RunOpt.noofiter;i++)
		m_DayError-= m_pModelInfo->T_Step;
	
	if(m_StructureModel.m_Structure.TimeResolution==1) {
		m_pModelInfo->T_RatioInput=m_pSim->RunOpt.noofiter;


	}

	m_pModelInfo->T_StepOrg= m_pModelInfo->T_Step;
	m_pModelInfo->T_CountIter=0;
	m_pModelInfo->T_CountIterOrg=0;
	m_pModelInfo->T_RatioOrgStep=1;
	m_pModelInfo->T_CountAdd=0;
	m_pModelInfo->T_TotNumIter=(m_pModelInfo->T_End- m_pModelInfo->T_Start)*m_pSim->RunOpt.noofiter;

	m_pModelInfo->SetAbioticTimeSteps(m_StructureModel.m_Structure.TimeResolution);
	  if(m_StructureModel.m_Structure.NitrogenCarbonStep==1) { // Nitrogen Carbon step is the same for Water and Heat processes
		  m_pModelInfo->T_Step_NC= m_pModelInfo->T_Step;
		  m_pModelInfo->T_NumIterNC_Day=m_pSim->RunOpt.noofiter;
	  }
	
	  else if(m_StructureModel.m_Structure.NitrogenCarbonStep==2) { //!Nitrogen Carbon step is independent of Water and Heat 
			unsigned int NC_MinStep=max(int(1440./m_StructureModel.m_Structure.NC_Iterations), int(1440.* m_pModelInfo->T_Step));
			m_pModelInfo->T_Step_NC=NC_MinStep/1440.;
			m_pModelInfo->T_NumIterNC_Day=int(1.00000001/ m_pModelInfo->T_Step_NC);
	  }

	if(!m_StructureModel.Ini()) return false;
	if(!m_HBVModel.Ini()) return false;
	if (!m_StructureModel.IniFunctors()) return false;
	//if (!m_HBVModel.IniFunctors()) return false;

	m_pSim->GetNumOutPuts();
	m_Outputs=m_pSim->GetAllSelectedOutputs();
	m_pModelInfo->m_NumOutputs=m_Outputs.size();
	m_ShiftSign=m_pSim->GetPressureHeadOutputIndex(m_Outputs);
	m_ShiftPointer=m_pSim->GetPtr(simtype::AUX,"PressureHead");




	m_OutFloat.resize(m_pModelInfo->m_NumOutputs);
	if(MultiFlag) {
		m_MultiOutputs=m_pSim->GetAllSelectedOutputs(true);
		m_ShiftSign=m_pSim->GetPressureHeadOutputIndex(m_MultiOutputs);
		((MR*)m_pSim)->SetMC_Step(m_StructureModel.m_Structure.MC_ScalingStepFunction(m_pModelInfo->ActualNoMulRun));


	}
//  Check and open File for WaterShed Option
	if(m_StructureModel.m_Structure.LateralInput==2) WShed_Ini();


	((CH*)m_pSim)->CH_Checker_build();
	((CH*)m_pSim)->GetFirstHit();

	Stat_Ini();

	if(!Valid_Ini(MultiFlag, First)) return false;
	m_pModelInfo->m_MinStore=m_pSim->RunOpt.longMinStart+ m_pModelInfo->m_OutInterval;
	m_pModelInfo->m_Repit=1;
	if(!MultiFlag) {
		m_pModelInfo->m_SimStart=clock();
		m_pSim->m_PG_OutPutFile.SetCompleteRead(true);
		if (!m_pSim->m_PG_OutPutFile.CreateNewFile(m_pModelInfo->m_NumOutputs, m_pModelInfo->m_OutRecord*m_pSim->m_DocFile.m_TimePeriodScaleFactor, 1)) {
			((Sim*)m_pSim)->AddToRunReport("Failed to create output file - Check if file may be too many records and variables");
#ifndef COUPSTD
			p_mainframe->UpdatePaneText(0, "Size too big for for output file");
#else
			string str = "Size too big for output file";
			cout << str+'\n';
#endif
			return false;

		};
	}
	else if(First) {
		m_pModelInfo->m_SimStart=clock();
		m_pSim->m_PG_MultiOutputFile.SetCompleteRepRead(true);
		if (m_pSim->m_PG_MultiOutputFile.IsOnlyForMemoryUse()) {
			m_pSim->m_PG_MultiOutputFile.Save(((ModelFiles*)m_pSim)->GetOutputFileName(0, false, true, 0, 0, true));
			m_pSim->m_PG_MultiOutputFile.ReOpen(true);
		}
		else {
			m_pSim->m_PG_MultiOutputFile.SetFileName(((ModelFiles*)m_pSim)->GetOutputFileName(0, false, true, 0, 0, true));

		}
		if (!m_pSim->m_PG_MultiOutputFile.CreateNewFile(m_MultiOutputs.size(), m_pModelInfo->m_OutRecord*m_pSim->m_DocFile.m_TimePeriodScaleFactor, m_pModelInfo->TotalNoMulRun)) {
			((Sim*)m_pSim)->AddToRunReport("Failed to create output file - Check if file may be too many records and variables");
#ifndef COUPSTD
			p_mainframe->UpdatePaneText(0, "Size too big for for output file");
#else
			string str = "Size too big for output file";
			cout << str + '\n';
#endif



			return false;
		};
#ifdef COUPSTD
		cout<< m_pSim->m_PG_MultiOutputFile.GetFileName();

#endif


		if (!m_pSim->m_PG_MultiOutputFile.WritePGFileStartHeader(m_pModelInfo->m_OutRecord*m_pSim->m_DocFile.m_TimePeriodScaleFactor, m_pModelInfo->TotalNoMulRun)) {
#ifdef COUPSTD
			cout << " problems when creating file:" +m_pSim->m_PG_MultiOutputFile.GetFileName();

#endif
			auto koll = m_pSim->m_PG_MultiOutputFile.GetFileName();
			int i = 12;


		}
		m_pSim->SetSimTime(-1);

	}
//	unsigned int test=m_ValPGFile[0].file->GetLongTime(1);
//m_pSim->m_PG_OutPutFile.AdjustSize(m_OutRecord, m_NumOutputs);

	return true;
}
bool Simulator::Run(bool MultiFlag)
{
#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	CCoupModelApp *pCoupModel=(CCoupModelApp*)pApp;
	CMainFrame *p_mainframe;
 	p_mainframe=(CMainFrame*)pCoupModel->m_pMainWnd;
#endif


	unsigned int itid1, NC_MinStep;
    bool START=true;
    bool LAST=false;
      bool CHANGE_T=false;
      bool StartDelay=false;
	  bool StartedSimPeriod = false;
	  m_pModelInfo->T_Elapsed=0.;
      int IKOLL=0;
      int IT=1;
      AccPeriod=0.;
	  int NC_COUNT=0;
	  bool StartReadData=true;
	  if(m_StructureModel.m_Structure.TimeResolution==1) 
		itid1=int(m_pModelInfo->T_Current+0.0001);
	  else 
	  	itid1=0;
	  //srand(0);
	 // rand_init(size_t(FRandomSeed));
	  
	  m_pModelInfo->SetAbioticTimeSteps(m_StructureModel.m_Structure.TimeResolution);

	  if(m_StructureModel.m_Structure.NitrogenCarbonStep==1) { //! Nitrogen Carbon step is the same as for Water and Heat
		  m_pModelInfo->T_Step_NC= m_pModelInfo->T_Step;
		  m_pModelInfo->T_NumIterNC_Day=size_t(1./ m_pModelInfo->T_Step_NC+0.001);
	  }
	  else if(m_StructureModel.m_Structure.NitrogenCarbonStep==2) { //! Nitrogen Carbon step is independent of Water and Heat
			NC_MinStep=max(int(1440./m_StructureModel.m_Structure.NC_Iterations), int(1440.* m_pModelInfo->T_Step));
			m_pModelInfo->T_Step_NC=NC_MinStep/1440.;
			m_pModelInfo->T_NumIterNC_Day=size_t(1./ m_pModelInfo->T_Step_NC+0.001);
	  }

	  m_pModelInfo->T_RatioInput=size_t(m_pModelInfo->T_StepSnow/ m_pModelInfo->T_Step);
	  m_pModelInfo->T_RatioNC=size_t(m_pModelInfo->T_Step_NC/ m_pModelInfo->T_Step);
	  if(m_pModelInfo->T_RatioInput<1) {
		  m_pModelInfo->T_StepSnow= m_pModelInfo->T_Step;
		  m_pModelInfo->T_RatioInput=1;
	  }
	  if(m_pModelInfo->T_RatioNC<1) {
		  m_pModelInfo->T_Step_NC= m_pModelInfo->T_Step;
		  m_pModelInfo->T_RatioNC=1;
	  }
	  m_pModelInfo->m_ActMinRead = m_pSim->RunOpt.longMinPreStart;
	  m_StructureModel.Dyn();
	//  T_Step_Irrig=T_StepSnow;   //	! Added Irrig Step 7 Oktober 2004

      //DST=T_Current
	  m_pModelInfo->T_SimPeriod= m_pModelInfo->T_End- m_pModelInfo->T_Start;
	  m_pModelInfo->RelRunTimeSingle=0.;
	  m_pModelInfo->RelRunTimeMulti=(m_pModelInfo->ActualNoMulRun-1.)/ m_pModelInfo->TotalNoMulRun;
	  size_t daycount=0;
	  size_t prevday=string::npos;


	 // m_OutRecord*=m_pSim->m_DocFile.m_TimePeriodScaleFactor;
      //CALL CPUOUT( false, true, false)
	for(size_t num_timescaling=0; num_timescaling<m_pSim->m_DocFile.m_TimePeriodScaleFactor; num_timescaling++) {
		if(num_timescaling>0) {
			Reset_DrivingFiles();
			m_pModelInfo->T_CountIter=0;
			m_pModelInfo->m_RecOutput=0;
			m_pModelInfo->T_CountIterOrg=0;
		}
		bool PostPeriod = false;
		size_t MoveBackMinutes = 365 * 1440;
		bool PostPeriodStarted = false;
		if (m_pSim->RunOpt.longMinEnd == m_pSim->RunOpt.longMinPostEnd) PostPeriodStarted = true;

		while(m_pModelInfo->m_ActMinRead<=m_pSim->RunOpt.longMinEnd||(m_pModelInfo->m_RecOutput< m_pModelInfo->m_OutRecord&&(m_StructureModel.m_Structure.TimeSerieOutputLevels==0||!m_pSim->IsMultiSimulation()))) {
			
			if(m_pModelInfo->T_CountIterOrg%m_pSim->RunOpt.noofiter==0&& m_pModelInfo->T_CountIterOrg>0) {
				daycount++;
				m_pModelInfo->T_CountIterOrg=0;
				m_pModelInfo->T_Elapsed=double(daycount);
	
			}
		
			m_pModelInfo->T_Current= m_pModelInfo->T_Start+ m_pModelInfo->T_Elapsed;

			m_pModelInfo->m_ActMin=int(1440.* m_pModelInfo->T_Current);
			if(num_timescaling>0)
				m_pModelInfo->m_ActMinRead= m_pModelInfo->m_ActMin-num_timescaling*(m_pSim->RunOpt.longMinEnd-m_pSim->RunOpt.longMinStart);
			else {
				if (m_pModelInfo->m_ActMin > m_pSim->RunOpt.longMinEnd&&!PostPeriodStarted) {
					if (!PostPeriodStarted) {
						Reset_DrivingFiles();
						PostPeriodStarted = true;
						m_pModelInfo->m_ActMinRead = m_pModelInfo->m_ActMin - MoveBackMinutes;
					}

				}
				else
					m_pModelInfo->m_ActMinRead = m_pModelInfo->m_ActMin;
			}




			m_pModelInfo->RelRunTimeSingle= m_pModelInfo->T_Elapsed/ m_pModelInfo->T_SimPeriod;

			if(m_pModelInfo->m_ActMin%1440==0) {
				m_pModelInfo->JDayNum=PGUtil::DayNumFunction(m_pModelInfo->m_ActMin);
				m_pModelInfo->JYearNum=PGUtil::YearNumFunction(m_pModelInfo->m_ActMin);
			}

		
			if (((CH*)m_pSim)->Make_ChaParCheck(m_pModelInfo->m_ActMinRead)) {
				m_StructureModel.m_Structure.p_Soil_WaterF->SetDrainageFunctors();
				m_StructureModel.IniFunctors();
			};

			m_StructureModel.Dyn();
			if(daycount!=prevday) {
				m_HBVModel.Dyn();
				prevday=daycount;
			}
			if (m_pModelInfo->m_ActMin >= m_pSim->RunOpt.longMinStart) {
				if (!StartedSimPeriod) {
					//if (!m_StructureModel.Ini()) return false;
					Stat_Ini();
					Set_StateVariables(true);

					StartedSimPeriod = true;
 				}
				Stat_Calc();
				Valid_Check();
			}
			if(m_pModelInfo->T_CountIter%5000==0) {

				if(m_pSim->IsMultiSimulation()) {
					string str;
	
					char buffer[50];
					size_t n=snprintf(buffer,50, "MultiSim %u (%u) - ", m_pModelInfo->ActualNoMulRun, m_pModelInfo->TotalNoMulRun);
					str.assign(&buffer[0],n);
					string numstr=FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo);
					numstr+=" - ";
					str+= m_pModelInfo->GetRemainingTimeString(m_pSim->IsMultiSimulation());
#ifndef COUPSTD
					p_mainframe->UpdatePaneText(0, numstr+str);
#else
					str += " RunNo :"+ FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo);
					cout << '\r'+str;
#endif
				}
				else {
					string str= m_pModelInfo->GetRemainingTimeString(m_pSim->IsMultiSimulation());
#ifndef COUPSTD
					string numstr=FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo);
					numstr+=" - ";
					p_mainframe->UpdatePaneText(0, numstr+str);
#else
					cout << '\r'+str;
#endif
				}

			}
		
			if(m_pModelInfo->m_ActMin>= m_pModelInfo->m_MinStore&&(m_StructureModel.m_Structure.TimeSerieOutputLevels==0||!m_pSim->IsMultiSimulation()||m_MultiOutputs.size()>0)) {
				m_pModelInfo->m_RecOutput++;
				if (m_pModelInfo->m_RecOutput <= m_pModelInfo->m_OutRecord) {
					Store_Outputs(MultiFlag, num_timescaling);
					m_pModelInfo->m_MinStore = m_pSim->RunOpt.longMinStart + m_pModelInfo->m_OutInterval * (m_pModelInfo->m_RecOutput + 1) + num_timescaling * (m_pSim->RunOpt.longMinEnd - m_pSim->RunOpt.longMinStart);
				}
				else {
					int i = 1;

				}
			}

			m_pModelInfo->m_UsedTime=clock()- m_pModelInfo->m_SimStart;
			m_pModelInfo->T_Elapsed+= m_pModelInfo->T_Step;
			m_pModelInfo->T_CountIter++;
			if(m_pModelInfo->T_CountAdd% m_pModelInfo->T_RatioOrgStep== m_pModelInfo->T_RatioOrgStep-1|| m_pModelInfo->T_RatioOrgStep==1)
				m_pModelInfo->T_CountIterOrg++;
			if(m_pModelInfo->T_RatioOrgStep>1) m_pModelInfo->T_CountAdd++;
		}
		while (m_pModelInfo->m_RecOutput< m_pModelInfo->m_OutRecord) {
				m_pModelInfo->m_RecOutput++;
				Store_Outputs(MultiFlag, num_timescaling);
				m_pModelInfo->m_MinStore=m_pSim->RunOpt.longMinStart+ m_pModelInfo->m_OutInterval*(m_pModelInfo->m_RecOutput+1)+num_timescaling*(m_pSim->RunOpt.longMinEnd-m_pSim->RunOpt.longMinStart);

		}
	}
	Set_StateVariables(false);
	return true;
}
bool Simulator::End(bool Multi, bool Final, bool First)
{
	m_pModelInfo->Set_Sim_EndParameters();

	m_StructureModel.End();
	m_pModelInfo->m_UsedTime=clock()- m_pModelInfo->m_SimStart;

	((CH*)m_pSim)->CH_Checker_Reset();

	if(m_pModelInfo->m_RecOutput<= m_pModelInfo->m_OutRecord) {





	}

	if(Multi) {
		char buffer[10000];
		string str;
		size_t n=snprintf(buffer,60, "MultiSimulation %d (%d) Completed - ", m_pModelInfo->ActualNoMulRun, m_pModelInfo->TotalNoMulRun);
		if (n>10&& n < 200&& m_pModelInfo->ActualNoMulRun== m_pModelInfo->TotalNoMulRun) {
			str.assign(&buffer[0], n);
			str += "RunNo " + FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo + 1 - m_pModelInfo->TotalNoMulRun) + " - ";
			str += m_pModelInfo->GetTimeUsedString();
			m_pSim->SetRunReportArray(str);
			m_pSim->SetSimTime(m_pModelInfo->GetTimeUsed());
		}
		else
			m_pSim->SetSimTime(m_pModelInfo->GetTimeUsed());
	}
	Stat_Reset();
	if(Multi) {
		Valid_End(true, Final, First);
		
		return Store_Write(true, Final);
	}
	else {
		Valid_End();

		m_pModelInfo->ActualNoMulRun;

		string Res = "Simulation completed Run No: ";
		Res += FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo)+ " - ";
		Res += m_pModelInfo->GetTimeUsedString();

		m_pSim->SetRunReportArray(Res);
#ifndef NO_FILES
        return Store_Write();
#else
        return true;
#endif
      
	}
	if(Final) {



	}


}

Structure *Simulator::GetSimStructurePointer()
{
	return &m_StructureModel.m_Structure;
}

void Simulator::Stat_Ini()
{
	Stat.resize(m_Outputs.size()+m_WshedOutputs.size());
	OutVector* pVector;
	OutSingle* pSingle;
	size_t multipgindex, singlepgindex;
	multipgindex = singlepgindex = 1;
	for(size_t i=0;i<m_Outputs.size(); i++) {
		Stat[i].Acc=0;
		Stat[i].AccTotal=0;
		Stat[i].Max=float(MISSING);
		Stat[i].Min=-MISSING;
		if (!m_Outputs[i].pBase->Is_Vector()) {
			pSingle = dynamic_cast<OutSingle*>(m_Outputs[i].pBase);
			Stat[i].Initial = pSingle->GetValue();
			pSingle->SetPgFileIndex(singlepgindex);
			singlepgindex++;
			if (pSingle->GetStoreFlag() == 2) {
				Stat[i].YBin = true;
				pSingle->SetMultiPGFileIndex(multipgindex);
				multipgindex++;
			}
			else
				Stat[i].YBin=false;
		}
		else {
			pVector = dynamic_cast<OutVector*>(m_Outputs[i].pBase);
			Stat[i].Initial = pVector->GetValue(m_Outputs[i].TabIndex);
			pVector->SetPgFileIndex(m_Outputs[i].TabIndex, singlepgindex);
			singlepgindex++;
			if (pVector->GetStoreFlag(m_Outputs[i].TabIndex) == 2) {
				Stat[i].YBin = true;
				pVector->SetMultiPGFileIndex(m_Outputs[i].TabIndex,multipgindex);
				multipgindex++;

			}
			else
				Stat[i].YBin=false;

		}
	}
	size_t start=m_Outputs.size();
	for(size_t i=0;i<m_WshedOutputs.size(); i++) {
		Stat[i+start].Acc=0;
		Stat[i+start].AccTotal=0;
		Stat[i+start].Max=float(MISSING);
		Stat[i+start].Min=-MISSING;
		if(!m_WshedOutputs[i].pBase->Is_Vector()) {
			pSingle=dynamic_cast<OutSingle*>(m_WshedOutputs[i].pBase);
			Stat[i+start].Initial=pSingle->GetValue();
			if(pSingle->GetStoreFlag()==2)
				Stat[i+start].YBin=true;
			else
				Stat[i+start].YBin=false;
		}
		else {
			pVector=dynamic_cast<OutVector*>(m_WshedOutputs[i].pBase);			
			Stat[i+start].Initial=pVector->GetValue(m_WshedOutputs[i].TabIndex);	
			if(pVector->GetStoreFlag(m_WshedOutputs[i].TabIndex)==2)
				Stat[i+start].YBin=true;
			else
				Stat[i+start].YBin=false;

		}
	}



	
	AccPeriod=0;


}
void Simulator::Stat_Calc()
{
	OutVector* pVector;
	OutSingle* pSingle;
	double value;
	for(size_t i=0;i<m_Outputs.size(); i++) {
		if(!m_Outputs[i].pBase->Is_Vector()) {
			pSingle=dynamic_cast<OutSingle*>(m_Outputs[i].pBase);
			value=pSingle->GetValue();		
		}
		else {
			pVector=dynamic_cast<OutVector*>(m_Outputs[i].pBase);			
			value=pVector->GetValue(m_Outputs[i].TabIndex);	

		}

		if(value<Stat[i].Min) Stat[i].Min=value;
		if(value>Stat[i].Max) Stat[i].Max=value;
		Stat[i].Acc+=value*m_pModelInfo->T_Step;
	}
	size_t start=m_Outputs.size();
	for(size_t i=0;i<m_WshedOutputs.size(); i++) {
		if(!m_WshedOutputs[i].pBase->Is_Vector()) {
			pSingle=dynamic_cast<OutSingle*>(m_WshedOutputs[i].pBase);
			value=pSingle->GetValue();		
		}
		else {
			pVector=dynamic_cast<OutVector*>(m_WshedOutputs[i].pBase);			
			value=pVector->GetValue(m_WshedOutputs[i].TabIndex);	

		}

		if(value<Stat[i+start].Min) Stat[i+start].Min=value;
		if(value>Stat[i+start].Max) Stat[i+start].Max=value;
		Stat[i+start].Acc+=value*m_pModelInfo->T_Step;
	}
	AccPeriod+=m_pModelInfo->T_Step;
}
void Simulator::Stat_Reset()
{
	OutSingle *pSingle;
	OutVector *pVector;
	SimB *pHead=m_pSim->GetPtr(simtype::AUX,"PressureHead");
	double final;
	double min;

	for(size_t i=0;i<m_Outputs.size(); i++) {
		if(m_Outputs[i].pBase->Is_Vector()){
			pVector=dynamic_cast<OutVector*>(m_Outputs[i].pBase);

			if(m_StructureModel.m_Structure.PressureHeadSign==0&&m_Outputs[i].pBase==pHead) {
				Stat[i].Initial*=-1;
				final=-pVector->GetValue(m_Outputs[i].TabIndex);
				min=-Stat[i].Max;
				Stat[i].Max=-Stat[i].Min;
				Stat[i].Min=min;
				Stat[i].AccTotal*=-1;
			}
			else
				final=pVector->GetValue(m_Outputs[i].TabIndex);
			pVector->AddSumVar(m_Outputs[i].TabIndex, Stat[i].Initial,final , Stat[i].Min, Stat[i].Max,
				Stat[i].AccTotal/(m_pModelInfo->T_End- m_pModelInfo->T_StartSimPeriod), Stat[i].AccTotal, int(i+1), 0, pVector->GetStoreFlag(m_Outputs[i].TabIndex));
				
		}
		else {
			pSingle=dynamic_cast<OutSingle*>(m_Outputs[i].pBase);

			pSingle->AddSumVar(float(Stat[i].Initial), float(pSingle->GetValue()), float(Stat[i].Min), float(Stat[i].Max),
				float(Stat[i].AccTotal/(m_pModelInfo->T_End- m_pModelInfo->T_StartSimPeriod)), float(Stat[i].AccTotal), int(i+1), 0, int(pSingle->GetStoreFlag()));

		}


	}


}
bool Simulator::Store_Write(bool MultiFlag, bool Final)
{
	size_t count=0;
	if(MultiFlag) {
		for(size_t i=0;i<Stat.size(); i++) {
			if(Stat[i].YBin) { // YBin true corresponds to a multiRun output of Variables to MultiBin File format
				m_pSim->m_PG_MultiOutputFile.SetVarName(count+1,m_MultiOutputs[count].pBase->GetName(),m_pModelInfo->ActualNoMulRun);
				m_pSim->m_PG_MultiOutputFile.SetVarUnit(count+1,m_MultiOutputs[count].pBase->GetUnit(),m_pModelInfo->ActualNoMulRun);
				if(m_StructureModel.m_Additional.Multi_PGFile_ID_Selector==0)
					m_pSim->m_PG_MultiOutputFile.SetVarId(count+1,FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo),m_pModelInfo->ActualNoMulRun );
				else if(m_StructureModel.m_Additional.Multi_PGFile_ID_Selector==1)
					m_pSim->m_PG_MultiOutputFile.SetVarId(count+1,FUtil::STD_ItoAscii(int(m_StructureModel.m_Additional.SeqNo_MetFile)),m_pModelInfo->ActualNoMulRun );
				else if(m_StructureModel.m_Additional.Multi_PGFile_ID_Selector==2)
					m_pSim->m_PG_MultiOutputFile.SetVarId(count+1,FUtil::STD_ItoAscii(int(m_StructureModel.m_Additional.IDNo_MetFile)),m_pModelInfo->ActualNoMulRun );
				m_pSim->m_PG_MultiOutputFile.SetVarLat(count+1, m_StructureModel.m_Additional.Y_coordinate,m_pModelInfo->ActualNoMulRun);
				m_pSim->m_PG_MultiOutputFile.SetVarLong(count+1, m_StructureModel.m_Additional.X_coordinate,m_pModelInfo->ActualNoMulRun);
				m_pSim->m_PG_MultiOutputFile.SetVarAlt(count+1, m_StructureModel.m_Additional.Z_coordinate,m_pModelInfo->ActualNoMulRun);
				m_pSim->m_PG_MultiOutputFile.SetVarStation(count+1,FUtil::STD_DtoAscii(m_AtmModel.m_Irrigation.PGFileName_Index),m_pModelInfo->ActualNoMulRun);
				count++;
				if (m_Outputs[i].pBase->Is_Vector()) {
					((OutVector*)m_Outputs[i].pBase)->SetMultiPGFileIndex(m_Outputs[i].TabIndex,count);

				}
				else {
					((OutSingle*)m_Outputs[i].pBase)->SetMultiPGFileIndex( count);


				}
			}
		}
		m_pSim->m_PG_MultiOutputFile.WriteNewPGFileRecord();
		m_pSim->m_PG_MultiOutputFile.ReCalculateMinMax(m_pModelInfo->ActualNoMulRun);
		if(Final) {
			bool goodfile=m_pSim->m_PG_MultiOutputFile.WritePGFileEnd(m_pModelInfo->m_OutRecord, m_pModelInfo->TotalNoMulRun);
			
			m_pSim->m_PG_OutPutFile.ReCalculateMinMax();
			m_pSim->m_PG_OutPutFile.CheckStatus();
			m_pSim->m_PG_OutPutFile.SetNormalTimeInterval(m_pModelInfo->m_OutInterval);
			string filename = ((ModelFiles*)m_pSim)->GetNewOutputFileName();
			auto filen = m_pSim->m_PG_OutPutFile.GetFileName();
			m_pSim->m_PG_OutPutFile.SetFileName(filename);

			
			if(!m_pSim->m_PG_MultiOutputFile.IsOnlyForMemoryUse())
				m_pSim->m_PG_MultiOutputFile.CloseFile();
#ifdef COUPSTD
			if(goodfile)
				cout << "Completed writing to Multi PG OutFile: " + m_pSim->m_PG_MultiOutputFile.GetFileName() +'\n';
			else
				cout << "Error when writing to Multi PG OutFile: " + m_pSim->m_PG_MultiOutputFile.GetFileName() + '\n';
#endif
		}
	}
	else {
		for(size_t i=0;i<m_Outputs.size(); i++) {
			m_pSim->m_PG_OutPutFile.SetVarName(i+1,m_Outputs[i].pBase->GetName());
			m_pSim->m_PG_OutPutFile.SetVarUnit(i+1,m_Outputs[i].pBase->GetUnit());
			m_pSim->m_PG_OutPutFile.SetVarUnitType(i + 1, m_Outputs[i].pBase->GetUnitType());
			m_pSim->m_PG_OutPutFile.SetVarId(i+1,FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo) );

			if(m_Outputs[i].pBase->GetDataType()==SIMPROFILE&&m_Outputs[i].pBase->Is_Vector()) {
				size_t index=m_Outputs[i].TabIndex;
				if(index<m_SoilModel.m_SoilFrost.DepthMidpLayer.size()) {
					string str=FUtil::STD_ItoAscii(int(100*m_SoilModel.m_SoilFrost.DepthMidpLayer[index]+1.E-4));
					if(str.size()==1) str="   "+str;
					if(str.size()==2) str="  "+str;
					if(str.size()==3) str=" "+str;
					if(str.size()<5) m_pSim->m_PG_OutPutFile.SetVarPos(i+1,str);
					string str1=FUtil::STD_ItoAscii(int(100*m_SoilModel.m_SoilFrost.ModelUpperDepth[index] + 1.E-4));
					if(str1.size()==1) str1="  "+str1;
					if(str1.size()==2) str1=" "+str1;
					string str2=FUtil::STD_ItoAscii(int(100*m_SoilModel.m_SoilFrost.ModelLowerDepth[index] + 1.E-4));
					if(str2.size()==1) str2="  "+str2;
					if(str2.size()==2) str2=" "+str2;
					string id=str1+"-"+str2+ " cm "+FUtil::STD_ItoAscii(int(m_pSim->m_DocFile.m_SimulationRunNo));
					m_pSim->m_PG_OutPutFile.SetVarId(i+1,id);
				}


			}
			else if(m_Outputs[i].pBase->GetDataType()==PLANT&&m_Outputs[i].pBase->Is_Vector()) {
				size_t index=m_Outputs[i].TabIndex;
				string str=FUtil::STD_ItoAscii(index+1);
				str="Plant "+str+" "+FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo);
				m_pSim->m_PG_OutPutFile.SetVarId(i+1,str);
			}

			m_pSim->m_PG_OutPutFile.SetVarLat(i+1, m_StructureModel.m_Additional.Y_coordinate);
			m_pSim->m_PG_OutPutFile.SetVarLong(i+1, m_StructureModel.m_Additional.X_coordinate);
			m_pSim->m_PG_OutPutFile.SetVarAlt(i+1, m_StructureModel.m_Additional.Z_coordinate);

		}
		m_pSim->m_PG_OutPutFile.ReCalculateMinMax();
		m_pSim->m_PG_OutPutFile.CheckStatus();
		m_pSim->m_PG_OutPutFile.SetNormalTimeInterval(m_pModelInfo->m_OutInterval);

		string filename = ((ModelFiles*)m_pSim)->GetNewOutputFileName();
		if (!m_pSim->m_PG_OutPutFile.IsOnlyForMemoryUse()) {
			if (!m_pSim->m_PG_OutPutFile.SaveAs(((ModelFiles*)m_pSim)->GetNewOutputFileName(), true)) {
#ifdef LINUX2
				string koll = ((ModelFiles*)m_pSim)->GetNewOutputFileName();
				cerr << koll << endl;
				return false;
#endif

			};
		}
		else
			m_pSim->m_PG_OutPutFile.SetFileName(filename);



	}

	//WaterShed Outputs below:

	if (m_WshedOutputs.size() == 0) return true;
	for(size_t i=0;i<m_WshedOutputs.size(); i++) {
			m_pSim->m_PG_WShed_Out.SetVarName(i+1,m_WshedOutputs[i].pBase->GetName());
			m_pSim->m_PG_WShed_Out.SetVarUnit(i+1,m_WshedOutputs[i].pBase->GetUnit());
			m_pSim->m_PG_WShed_Out.SetVarId(i+1,FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo) );
			if(m_WshedOutputs[i].pBase->GetDataType()==SIMPROFILE&&m_WshedOutputs[i].pBase->Is_Vector()) {
				size_t index=m_Outputs[i].TabIndex;
				if(index<m_SoilModel.m_SoilFrost.DepthMidpLayer.size()) {
					string str=FUtil::STD_ItoAscii(int(100*m_SoilModel.m_SoilFrost.DepthMidpLayer[index] + 1.E-4));
					if(str.size()==1) str="   "+str;
					if(str.size()==2) str="  "+str;
					if(str.size()==3) str=" "+str;
					if(str.size()<5) m_pSim->m_PG_OutPutFile.SetVarPos(i+1,str);
					string str1=FUtil::STD_ItoAscii(int(100*m_SoilModel.m_SoilFrost.ModelUpperDepth[index] + 1.E-4));
					if(str1.size()==1) str1="  "+str1;
					if(str1.size()==2) str1=" "+str1;
					string str2=FUtil::STD_ItoAscii(int(100*m_SoilModel.m_SoilFrost.ModelLowerDepth[index] + 1.E-4));
					if(str2.size()==1) str2="  "+str2;
					if(str2.size()==2) str2=" "+str2;
					string id=str1+"-"+str2+ " cm "+FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo);
					m_pSim->m_PG_WShed_Out.SetVarId(i+1,id);
				}


			}

			m_pSim->m_PG_WShed_Out.SetVarLat(count+1, m_StructureModel.m_Additional.Y_coordinate);
			m_pSim->m_PG_WShed_Out.SetVarLong(count+1, m_StructureModel.m_Additional.X_coordinate);
			m_pSim->m_PG_WShed_Out.SetVarAlt(count+1, m_StructureModel.m_Additional.Z_coordinate);

		}
	m_pSim->m_PG_WShed_Out.ReCalculateMinMax();
	if (!m_pSim->m_PG_WShed_Out.SaveAs(m_pSim->GetWShedFileName_Out(), true)) {
#ifndef COUPSTD	
			MFC_Util::MessageBoxReturn("Error when write WaterShed File:" + m_pSim->GetWShedFileName_Out(), "File write Error", MB_OK);
#endif
			return false;
	}
			
	return true;
}
void Simulator::Store_Outputs(bool MultiFlag, size_t num_timescaling)
{
	size_t ist;
	if(!MultiFlag) 
		ist=(m_pModelInfo->m_RecOutput-1+num_timescaling* m_pModelInfo->m_OutRecord)*(m_pModelInfo->m_NumOutputs+1);
	else
		ist=(m_pModelInfo->m_RecOutput-1+num_timescaling* m_pModelInfo->m_OutRecord)*(m_MultiOutputs.size()+1);

	size_t ist_Wshed;

	ist_Wshed=(m_pModelInfo->m_RecOutput-1+num_timescaling* m_pModelInfo->m_OutRecord)*(m_WshedOutputs.size()+1);
	if(AccPeriod==0){
		if(!MultiFlag) {
			m_OutFloat.assign(m_pModelInfo->m_NumOutputs,MISSING);
		}
		else {
			m_OutFloat.assign(m_MultiOutputs.size(),MISSING);

		}
		m_OutFloatWShed.assign(m_WshedOutputs.size(), MISSING);
	}
	else {
		if(MultiFlag) m_OutFloat.clear();
		for(size_t i=0;i<m_Outputs.size(); i++) {
			if(!MultiFlag)
				m_OutFloat[i]=float(Stat[i].Acc/AccPeriod);	
			else if(Stat[i].YBin) {
				m_OutFloat.push_back(float(Stat[i].Acc/AccPeriod));
			}
			Stat[i].AccTotal+=Stat[i].Acc;
			Stat[i].Acc=0;
		}
		if(m_WshedOutputs.size()!=m_OutFloatWShed.size()) m_OutFloatWShed.resize(m_WshedOutputs.size());
		for(size_t i=0;i<m_WshedOutputs.size(); i++) {
			m_OutFloatWShed[i]=float(Stat[i+m_Outputs.size()].Acc/AccPeriod);	

			Stat[i+m_Outputs.size()].AccTotal+=Stat[i+m_Outputs.size()].Acc;
			Stat[i+m_Outputs.size()].Acc=0;
		}

		if(m_StructureModel.m_Structure.PressureHeadSign==0) 
			for(size_t i=0; i<m_ShiftSign.size(); i++)
				m_OutFloat[m_ShiftSign[i]]*=-1;


	
	}
	unsigned int minute;
	minute = m_pModelInfo->m_MinStore - m_pModelInfo->m_OutInterval / 2;
	if(!MultiFlag&& m_pModelInfo->m_NumOutputs>0) {
		//m_pSim->m_PG_OutPutFile.SetLongTime(m_pModelInfo->m_RecOutput+num_timescaling*m_OutRecord,m_MinStore-m_OutInterval/2);

		m_pSim->m_PG_OutPutFile.SetDatePointer(&minute, ist);
		m_pSim->m_PG_OutPutFile.SetVarPointer(&m_OutFloat[0], ist, m_pModelInfo->m_NumOutputs);
	}
	else if(m_MultiOutputs.size()>0) {
		m_pSim->m_PG_MultiOutputFile.SetDatePointer(&minute,ist);	
		m_pSim->m_PG_MultiOutputFile.SetVarPointer(&m_OutFloat[0], ist, m_MultiOutputs.size());
	}
	if(m_WshedOutputs.size()>0) {
		m_pSim->m_PG_WShed_Out.SetDatePointer(&minute, ist_Wshed);
		m_pSim->m_PG_WShed_Out.SetVarPointer(&m_OutFloatWShed[0], ist_Wshed, m_WshedOutputs.size());
	}
	AccPeriod=0;
}
bool Simulator::Valid_Check_Precise(size_t i) {

	CPG *pPG=m_ValPGFile[i].simvalfile;
	bool changepos=pPG->Check_Update_ActHit(0, m_pModelInfo->m_ActMin);

	if(changepos) {
		for (size_t j = 0; j < m_ValPGFile[i].varindex.size(); j++) {
			size_t LL = m_ValPGFile[i].val_listindex[j];
			m_ValPGFile[i].simvalues[j] = float(GetSimValValue(LL));
			if (m_StructureModel.m_Structure.PressureHeadSign == 0 && m_ShiftPointer != nullptr)
				if (m_pValVar[LL].pBase == m_ShiftPointer) m_ValPGFile[i].simvalues[j] *= -1;

			size_t ist = (pPG->GetPrevHitPos(j) - 1)*(m_ValPGFile[i].varindex.size() + 1);
			if (ist >= m_ValPGFile[i].FirstValidRecord)
				pPG->SetVarPointer(&m_ValPGFile[i].simvalues[0], ist, m_ValPGFile[i].varindex.size());
			m_ValPGFile[i].ActPos = pPG->GetPrevHitPos(j) + 1;
		}
	}
	return true;
}
bool Simulator::Valid_Check_Interval(size_t i) {
	CPG *pPG=m_ValPGFile[i].simvalfile;
			//if Mean value is to be calculated then check for hit within a window for the specific file
		// Return ActHit as the ActPos of that file when valid hit is find
	// bool changepos=pPG->Check_Update_ActHit(m_ActMin);
	 auto pos=pPG->GetPos(m_pModelInfo->m_ActMin);
	 size_t ActHit=pPG->GetHitPos(0, m_pModelInfo->m_ActMin-m_ValPGFile[i].HalfDuration, m_pModelInfo->m_ActMin+m_ValPGFile[i].HalfDuration);

		if(ActHit>0) {
			//Hit for mean value find
			if(ActHit==m_ValPGFile[i].PrevPos) {
				// Only accumulated when same record
				auto delta = (m_pModelInfo->T_Current - m_ValPGFile[i].prevSimTime);
				for(size_t j=0; j<m_ValPGFile[i].varindex.size(); j++){				
					size_t LL=m_ValPGFile[i].val_listindex[j];
					m_ValPGFile[i].accsimvalues[j]+=float(GetSimValValue(LL))*delta;
					
				}
				m_ValPGFile[i].count++;
				m_ValPGFile[i].accduration+= delta;
				m_ValPGFile[i].prevSimTime = m_pModelInfo->T_Current;
			}
			else if(ActHit>=m_ValPGFile[i].PrevPos+1) {
				// Estimate Mean value and store data from previous period and set new accumulator for latest hit
				// The hit is within next valid window for making mean value
				if(m_ValPGFile[i].count>0) {
					for(size_t j=0; j<m_ValPGFile[i].varindex.size(); j++){				
						size_t LL=m_ValPGFile[i].val_listindex[j];
						m_ValPGFile[i].simvalues[j]=m_ValPGFile[i].accsimvalues[j]/m_ValPGFile[i].accduration;
						if(m_StructureModel.m_Structure.PressureHeadSign==0&&m_ShiftPointer!=nullptr) 
								if(m_pValVar[LL].pBase==m_ShiftPointer) m_ValPGFile[i].simvalues[j]*=-1;
						m_ValPGFile[i].accsimvalues[j]=float(GetSimValValue(LL)* (m_pModelInfo->T_Current - m_ValPGFile[i].prevSimTime));
					}
					size_t ist=(m_ValPGFile[i].PrevPos-1)*(m_ValPGFile[i].varindex.size()+1);
					if(ist>=m_ValPGFile[i].FirstValidRecord) 
						pPG->SetVarPointer(&m_ValPGFile[i].simvalues[0], ist,m_ValPGFile[i].varindex.size()); 
					m_ValPGFile[i].count=1;
					m_ValPGFile[i].accduration = m_pModelInfo->T_Current - m_ValPGFile[i].prevSimTime;
				}

				if(m_ValPGFile[i].ActPos>=0) m_ValPGFile[i].PrevPos++;
			}

		}
		else {
			// No valid hit but previous mean values to be stored in file
			// The previous values are stored and the accumulator for mean value is not set
			if(m_ValPGFile[i].count>0&&m_ValPGFile[i].PrevPos>0) {
					for(size_t j=0; j<m_ValPGFile[i].varindex.size(); j++){				
						size_t LL=m_ValPGFile[i].val_listindex[j];
						m_ValPGFile[i].simvalues[j]=m_ValPGFile[i].accsimvalues[j]/m_ValPGFile[i].accduration;
						if(m_StructureModel.m_Structure.PressureHeadSign==0&&m_ShiftPointer!=nullptr) 
							if(m_pValVar[LL].pBase==m_ShiftPointer) m_ValPGFile[i].simvalues[j]*=-1;
						m_ValPGFile[i].accsimvalues[j]=0;
					}
					size_t ist=(m_ValPGFile[i].PrevPos-1)*(m_ValPGFile[i].varindex.size()+1);
					if(ist>=m_ValPGFile[i].FirstValidRecord)
						pPG->SetVarPointer(&m_ValPGFile[i].simvalues[0], ist,m_ValPGFile[i].varindex.size()); 
					m_ValPGFile[i].count=0;
					m_ValPGFile[i].accduration = 0;
					m_ValPGFile[i].PrevPos++;
					m_ValPGFile[i].prevSimTime = m_pModelInfo->T_Current;
			}

		}
	return true;
}
bool Simulator::Valid_Check() {
	for(size_t i=0; i<m_ValPGFile.size(); i++) {
		if(m_ValPGFile[i].file==nullptr||m_ValPGFile[i].simvalfile==nullptr) continue;

		if(!m_ValPGFile[i].MeanValueOut) Valid_Check_Precise(i);
		else Valid_Check_Interval(i);
	}
	return true;
}
bool Simulator::Valid_Ini(bool MultiFlag, bool First) {

	m_pModelInfo->m_pValData=((NewMap*)m_pSim)->GetValidDataPtr();
	if(First||!MultiFlag) {
		m_ValTest.resize(m_pSim->GetNumAll_TSV_ValVariables());
		m_pValVar.resize(0);

		m_pValVar.resize(m_pSim->GetNumAll_TSV_ValVariables());
	}

	SimB *pSimB;
	string type;
	SIMB a;

	map<CPG*,size_t>::iterator it;
	map<CPG*,size_t> File_And_Index;
	size_t count=0;
	VALID_PGFILE b;
	if(First||!MultiFlag) {
		// Find number of files and create a vector for those
		size_t numfile=0;
		for(size_t i=0; i<m_ValTest.size(); i++) {
			size_t ifile=m_pSim->m_Val_Array[i].ValidationFileNumber;
			numfile=max(ifile,numfile); 
		}

		m_ValPGFile.resize(numfile);
		for(size_t i=0; i<m_ValPGFile.size();i++) {
			m_ValPGFile[i].file=nullptr;
			m_ValPGFile[i].simvalfile=nullptr;
		}

		while(m_StructureModel.m_Structure.Abs_Error.size()<m_ValTest.size()) {
			m_StructureModel.m_Structure.Abs_Error.push_back(m_StructureModel.m_Structure.Abs_Error.back());
			m_StructureModel.m_Structure.Rel_Error.push_back(m_StructureModel.m_Structure.Rel_Error.back());

		}
		// Check Validation data from document info create ValPGFile vector to store info for validation
		for(size_t i=0; i<m_ValTest.size(); i++) {
			switch (m_pSim->m_Val_Array[i].OutputType) {
						case 1:	type="State Variables";
							break;
						case 2:	type="Flow Variables";
		 					break;
						case 3:	type="Auxiliary Variables";
	 						break;
						 case 4:type="Driving Variables";
	 						break;
			}
			pSimB=m_pSim->GetPtr(type,m_pSim->m_Val_Array[i].Group,m_pSim->m_Val_Array[i].Name);
			if (pSimB == nullptr) {
				m_pSim->m_Val_Array[i].Name = FUtil::NameRevision2018(m_pSim->m_Val_Array[i].Name);
				pSimB = m_pSim->GetPtr(type, m_pSim->m_Val_Array[i].Group, m_pSim->m_Val_Array[i].Name);
				if (pSimB == nullptr) {
					pSimB = m_pSim->GetPtrByName(m_pSim->m_Val_Array[i].Name);
				}

			}
			a.pBase=pSimB; a.TabIndex=m_pSim->m_Val_Array[i].LocalIndex;
			m_pValVar[i]=a;
			m_StructureModel.m_Structure.Abs_Error[i]=m_pSim->m_Val_Array[i].A_Error;
			m_StructureModel.m_Structure.Rel_Error[i]=m_pSim->m_Val_Array[i].P_Error;
			auto ifile=m_pSim->m_Val_Array[i].ValidationFileNumber;

			F *pF=((NewMap*)m_pSim)->ValidationFilePointer(int(ifile));
			CPG *pPGNew=pF->GetPointer();
			it=File_And_Index.find(pPGNew);
			if(it==File_And_Index.end()) {
				if(!pPGNew->AreAllValuesAssigned())
				if(!pF->CheckFileNameAndAssignNameToPGClass()) {
#ifndef COUPSTD	
	auto choice=MFC_Util::MessageBoxReturn("Simulation completed with errors \nValidation File not find ","Write Result Write Error" ,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);

#endif
					return false;		
					
					
				};
				pF->CallDataDescription();
				File_And_Index.insert(pair<CPG*,size_t>(pPGNew, ifile-1));
				b.varindex.clear();
				b.val_listindex.clear();
				b.file=pPGNew;
				b.simvalfile=nullptr;
				m_ValPGFile[ifile-1]=b;
				count++;
			}

			it=File_And_Index.find(pPGNew);
			if(it!=File_And_Index.end()) {
				m_ValPGFile[it->second].varindex.push_back(m_pSim->m_Val_Array[i].ValidationFileIndex);
				m_ValPGFile[it->second].val_listindex.push_back(i);
			}
		}
	}

	if (m_ValPGFile.size() > 0&&m_ValPGFile[0].varindex.size() > 0&& m_StructureModel.m_Structure.IndexForValid==1) {
		m_ValPGFile[0].varindex[0] = size_t(m_StructureModel.m_Structure.Val_Index-1);




	}




	for(size_t i=0; i<m_ValPGFile.size(); i++) {
		m_ValPGFile[i].pRes=(CResiduals*)((NewMap*)m_pSim)->GetResidualPointer(i+1);
		if(MultiFlag&&First) {
			if(m_ValPGFile[i].file==nullptr) continue;
			auto recl=m_ValPGFile[i].file->GetNumRecords();
			auto numvar=m_ValPGFile[i].varindex.size();
			auto totrec=numvar*m_pModelInfo->TotalNoMulRun ;
			if(m_ValPGFile[i].pRes!=nullptr) // One residual Xbin file for entire Multirun
				if(!m_ValPGFile[i].pRes->CreateFile(((NewMap*)m_pSim)->GetXBinFileName(int(i+1)),totrec, recl, numvar )) m_ValPGFile[i].pRes=nullptr;
		}
		m_ValPGFile[i].simvalues.resize(m_ValPGFile[i].varindex.size());
		m_ValPGFile[i].prevSimTime = m_pModelInfo->T_Current;
		m_ValPGFile[i].accduration = 0;
		if(m_ValPGFile[i].file==nullptr) continue;
		// Open Validation File


		if (!m_pSim->m_pCommonModelInfo->IsRunningWithoutFiles()) {
			m_ValPGFile[i].file->SetCompleteRead(false);
			m_ValPGFile[i].file->CloseFile();
			bool testfile = m_ValPGFile[i].file->ReOpen();
		}
		if (m_StructureModel.m_Structure.DynamicValidFileIndex == 1) {
			m_ValPGFile[i].file->SetRepititionMode(true);
			m_ValPGFile[i].file->SetRepititionSequence(size_t(m_StructureModel.m_Structure.ValFile_Index + 0.1), true);
		}
		else {
			m_ValPGFile[i].file->SetActBuffer(0);
		}

		auto test=m_ValPGFile[i].file->GetLongTime(1);

		// Create New file with Simulated outputs at validation time points (one for each simulation to be deleted if not used as archive)
		if(m_ValPGFile[i].simvalfile==nullptr) {
			m_ValPGFile[i].simvalfile = ((NewMap*)m_pSim)->ValidationResultPG_Pointer(i);
			if (m_ValPGFile[i].simvalfile == nullptr) {
				cout << "Error to get pointer to Validation Output structure " << endl;
				return false;
			}
	//	m_ValPGFile[i].simvalfile= new CPG();
			m_ValPGFile[i].simvalfile->SetCompleteRead(true);
			auto rec=m_ValPGFile[i].file->GetNumRecords();
			m_ValPGFile[i].simvalfile->CreateNewFile(m_ValPGFile[i].varindex.size(),rec,1);
			unsigned int min_min;
			for (size_t j = 0; j < rec; j++) {
				size_t ist = j*(m_ValPGFile[i].varindex.size() + 1);
				auto min = m_ValPGFile[i].file->GetLongTime(j + 1);
				if (j == 0) min_min = min;
				m_ValPGFile[i].simvalfile->SetDatePointer(&min, ist);
			}
			m_ValPGFile[i].simvalfile->pg.status.MinStart = min_min;
		}

		m_ValPGFile[i].simvalfile->ResetPos();
		PGPOS pos = m_ValPGFile[i].simvalfile->GetPos(m_pModelInfo->m_ActMin);
		m_ValPGFile[i].file->ResetPos();
		m_ValPGFile[i].count = 0;
		pos=m_ValPGFile[i].file->GetPos(m_pModelInfo->m_ActMin);
		m_ValPGFile[i].ActPos=pos.first-1;

		if(pos.first>1) m_ValPGFile[i].FirstValidRecord=pos.first*(m_ValPGFile[i].varindex.size()+1);
		else m_ValPGFile[i].FirstValidRecord=0;
		//if(!First) {
			if(pos.first>1) {
				size_t ist; /// Set Missing values in validation file if validation data is outside simulated period
				for(size_t j=0; j<m_ValPGFile[i].simvalfile->GetNumRecords(); j++) {
					ist=j*(m_ValPGFile[i].varindex.size()+1);
					m_ValPGFile[i].simvalfile->SetRecordPointer(&m_ValPGFile[i].simvalues[0], ist,m_ValPGFile[i].varindex.size()); 
					m_ValPGFile[i].simvalfile->SetMissingValueRecordPointer(ist,m_ValPGFile[i].varindex.size()); 
	
				}
				m_ValPGFile[i].FirstValidRecord=pos.first*(m_ValPGFile[i].varindex.size()+1);

			}
		//}
		m_ValPGFile[i].MeanValueOut=false;
		for (size_t j=0; j<m_ValPGFile[i].val_listindex.size(); j++) {// Check if Mean values should be calculated for Validation Outputs
			size_t LL;
			LL=m_ValPGFile[i].val_listindex[j];
			if(m_pSim->m_Val_Array[LL].Duration>1.) {
				 auto pos= m_ValPGFile[i].file->GetPos(m_pModelInfo->m_ActMin);
				 //size_t ActHit= m_ValPGFile[i].file->GetHitPos(m_ActMin-m_ValPGFile[i].HalfDuration,m_ActMin+m_ValPGFile[i].HalfDuration);

				m_ValPGFile[i].MeanValueOut=true;
				m_ValPGFile[i].HalfDuration=m_pSim->m_Val_Array[LL].Duration/2;
				m_ValPGFile[i].PrevPos=pos.first;
				m_ValPGFile[i].accsimvalues.resize(m_ValPGFile[i].val_listindex.size());
			}
			else {
				m_ValPGFile[i].MeanValueOut=false;
				m_ValPGFile[i].PrevPos=1;
				m_ValPGFile[i].accsimvalues.resize(m_ValPGFile[i].val_listindex.size());
				m_ValPGFile[i].count=0;
				m_ValPGFile[i].simvalfile->GetPos(m_pModelInfo->m_ActMin);

			}

		}
		if (m_StructureModel.m_Structure.DynamicValidFileIndex == 1) {
			m_ValPGFile[i].file->SetRepititionMode(true);
			m_ValPGFile[i].file->SetRepititionSequence(size_t(m_StructureModel.m_Structure.ValFile_Index + 0.1), true);
		}
		else {
			m_ValPGFile[i].file->SetActBuffer(0);
			m_ValPGFile[i].file->ReadFirst_Buffer();
		}
		

	}

	for(size_t i=0; i<m_ValTest.size(); i++) 
	double v=GetSimValValue(i);


	return true;
}
bool Simulator::Valid_End(bool Multi, bool Final, bool First) {
	
	if(Multi) {
		for(size_t i=0; i<m_ValPGFile.size(); i++) {
			if(m_ValPGFile[i].file==nullptr) continue;
			CPG *pPG=m_ValPGFile[i].simvalfile;
			pPG->SetNormalTimeInterval();
			pPG->CheckStatus();
			for(size_t j=0; j<m_ValPGFile[i].val_listindex.size(); j++) {
				size_t LL=m_ValPGFile[i].val_listindex[j];
				string str;
				str=m_pSim->m_Val_Array[LL].Name;
				if(m_pValVar[LL].TabIndex>=0) str+='('+FUtil::STD_ItoAscii(m_pValVar[LL].TabIndex+1)+')';
				pPG->SetVarName(j+1,str);
				pPG->SetVarUnitType(j+1,m_pValVar[LL].pBase->GetUnitType());
				pPG->SetVarId(j+1,FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo) );
			}
			PGPOS pos=pPG->GetPos(m_pModelInfo->m_ActMin);
			if(pos.first<pPG->GetNumRecords()) {
				size_t ist; /// Set Missing values in validation file if validation data is outside simulated period
				for(size_t j=pos.first; j<pPG->GetNumRecords(); j++) {
					ist=j*(m_ValPGFile[i].varindex.size()+1);
					pPG->SetMissingValueRecordPointer(ist,m_ValPGFile[i].varindex.size()); 
				}
			}
			//pPG->WriteNewPGFileRecord();
			if(First) {
				pPG->ReCalculateMinMax();
				pPG->CheckStatus();
				string newfilename = ((ModelFiles*)m_pSim)->GetNewOutputFileName(int(i + 1), true);
				if (!pPG->IsOnlyForMemoryUse())
					pPG->SaveAs(newfilename);
				else
					pPG->SetFileName(newfilename);
			}
		}


	}
	else {
		// Creating the output Validation file with all simulated values corresponding to the 
		// Validation input file
		for(size_t i=0; i<m_ValPGFile.size(); i++) {
			if(m_ValPGFile[i].file==nullptr) continue;
			CPG *pPG=m_ValPGFile[i].simvalfile;
			pPG->SetNormalTimeInterval();
			for(size_t j=0; j<m_ValPGFile[i].val_listindex.size(); j++) {
				size_t LL=m_ValPGFile[i].val_listindex[j];
				string str;
				str=m_pSim->m_Val_Array[LL].Name;
				if(m_pValVar[LL].pBase->Is_Vector()) 
					 str+='('+FUtil::STD_ItoAscii(m_pValVar[LL].TabIndex+1)+')';
				pPG->SetVarName(j+1,str);
				pPG->SetVarUnitType(j+1,m_pValVar[LL].pBase->GetUnitType());
				pPG->SetVarId(j+1,FUtil::STD_ItoAscii(m_pSim->m_DocFile.m_SimulationRunNo) );
			}
			PGPOS pos = pPG->GetPos(m_pModelInfo->m_ActMin);

			if(pos.first<pPG->GetNumRecords()) {
				size_t ist; /// Set Missing values in validation file if validation data is outside simulated period end
				for(size_t j=pos.first; j<pPG->GetNumRecords(); j++) {
					ist=j*(m_ValPGFile[i].varindex.size()+1);
					pPG->SetMissingValueRecordPointer(ist,m_ValPGFile[i].varindex.size()); 
				}
			}
			pPG->ResetPos();
			pos = pPG->GetPos(m_pSim->RunOpt.longMinStart);
			if (pos.first>1) {
				size_t ist; /// Set Missing values in validation file if validation data is outside simulated period - start
				for (size_t j = 0; j<pos.first; j++) {
					ist = j*(m_ValPGFile[i].varindex.size() + 1);
					pPG->SetMissingValueRecordPointer(ist, m_ValPGFile[i].varindex.size());
				}
			}
			/*else if(pos.first==pPG->GetNumRecords()) {
				size_t ist; /// Set Missing values in validation file if validation data is outside simulated period
				size_t LL;
				for(size_t j=0; j<m_ValPGFile[i].varindex.size(); j++){				
					LL=m_ValPGFile[i].val_listindex[j];
					m_ValPGFile[i].simvalues[j]=GetSimValValue(LL);
					if(m_StructureModel.m_Structure.PressureHeadSign==0&&m_ShiftPointer!=nullptr) 
						if(m_pValVar[LL].pBase==m_ShiftPointer) m_ValPGFile[i].simvalues[j]*=-1;
				}
				ist=(pos.first-1)*(m_ValPGFile[i].varindex.size()+1);
				pPG->SetVarPointer(&m_ValPGFile[i].simvalues[0], ist,m_ValPGFile[i].varindex.size()); 

			}*/
			pPG->ReCalculateMinMax();
			pPG->CheckStatus();
			string newfilename = ((ModelFiles*)m_pSim)->GetNewOutputFileName(int(i + 1));
			if (!pPG->IsOnlyForMemoryUse())
				pPG->SaveAs(newfilename, true);
			else
				pPG->SetFileName(newfilename);

		}
	}
	Valid_Evaluation(Multi, Final);
	if(Final||!Multi) {
		if (m_ValPGFile.size() == 0) return true;
		m_ValPGFile.clear();
		m_ValTest.clear();
	}
	return true;
}
bool Simulator::Valid_Evaluation(bool MultiFlag, bool Final) {
	// Estimate the performance and set the values into the Validation Data Structure.
	vector <float> Sim, Obs, Residual;
	vector <double> sim, obs;
	STAT_PERF a;


	if ((m_pModelInfo->ActualNoMulRun-1)%size_t(m_StructureModel.m_Structure.AggregateCalibNumber + 0.01) == 0) {
		m_pSim->m_DocFile.m_TsVLogLi = 0;
		m_pSim->m_DocFile.m_TsVLogLiSum = 0;
		m_pSim->m_InformCriteria.n_parameters =static_cast<MR*>(m_pSim)->MR_Get_NumBayesPar();
		m_pSim->m_InformCriteria.n_points = 0;
		m_pSim->m_InformCriteria.SSE = 0;
		m_pSim->m_InformCriteria.AIC = 0;
		m_pSim->m_InformCriteria.BIC = 0;
		auto nagg = size_t(m_StructureModel.m_Structure.AggregateCalibNumber + 0.01);
		m_pSim->m_InformCriteria.n_aggregateNumber = nagg;
	}

	
	for (size_t i = 0; i < m_ValPGFile.size(); i++) {
		if (m_ValPGFile[i].file == nullptr) continue;
		CPG* pPG_Sim = m_ValPGFile[i].simvalfile;
		CPG* pPG_Obs = m_ValPGFile[i].file;
		//if(!pPG_Sim->IsOpen()) pPG_Sim->ReOpen();

		if (!m_pSim->m_pCommonModelInfo->IsRunningWithoutFiles()) {

			if (!pPG_Obs->IsOpen())	pPG_Obs->ReOpen();
			if (!pPG_Sim->IsGood() || !pPG_Obs->IsGood()) continue;
		}

			if (pPG_Obs->GetNumRepititions() > 1&&(MultiFlag|| m_StructureModel.m_Structure.DynamicValidFileIndex == 1)) {
				pPG_Obs->SetRepititionMode(true);
				if (m_StructureModel.m_Structure.DynamicValidFileIndex == 1) {
					m_ValPGFile[i].file->SetRepititionSequence(size_t(m_StructureModel.m_Structure.ValFile_Index + 0.1), false);
				}
				else {
					size_t SeqnoDim1 = m_pSim->MR_GetDimCounter(0);
					pPG_Obs->SetRepititionSequence(SeqnoDim1, false);
				}


			}
			for(size_t j=0; j<m_ValPGFile[i].val_listindex.size(); j++) {
				size_t LL=m_ValPGFile[i].val_listindex[j];
				
				Sim=pPG_Sim->GetVarVector(j+1);
				Obs=pPG_Obs->GetVarVector(m_ValPGFile[i].varindex[j]+1);
				Residual.resize(Obs.size());

				sim.clear(); obs.clear();
				for(size_t ii=0; ii<Sim.size(); ii++) {
					if(Sim[ii]>-1.E37f&&Obs[ii]>-1.E37f) {
						sim.push_back(double(Sim[ii]));
						obs.push_back(double(Obs[ii]));
						Residual[ii]=Obs[ii]-Sim[ii];
					}
					else {
						Residual[ii]=MISSING;
					}
				}
				if(m_ValPGFile[i].pRes!=nullptr) {
					if (m_ValPGFile[i].pRes->AddResiduals(&Residual[0]));
							
				}
				m_pSim->m_Val_Array[LL].LogLi=m_StructureModel.m_Structure.SumLikelihood(LL, &sim, &obs);

				if (sim.size() > 0) {
					a = Get_StatComp(&sim, &obs);
					m_pSim->m_DocFile.m_TsVLogLi += float(m_pSim->m_Val_Array[LL].LogLi);
					m_pSim->m_InformCriteria.n_points+= a.n;
					m_pSim->m_InformCriteria.SSE += a.sse;
				}
				else{
					a.n=0;
					a.intercept=0;
					a.slope=1;
					a.r2=0;
					a.meansim=0;
					a.meanobs=0;
					a.rmse=0.;
					a.me=0.;
					a.nse=0.;
					a.sse = 0. ;
					m_pSim->m_Val_Array[LL].LogLi = MISSING;
				}
				m_pSim->m_Val_Array[LL].A0=a.intercept;
				m_pSim->m_Val_Array[LL].A1=a.slope;
				m_pSim->m_Val_Array[LL].nNumber=int(a.n);
				m_pSim->m_Val_Array[LL].R2=a.r2;
				m_pSim->m_Val_Array[LL].MeanSim=a.meansim;
				m_pSim->m_Val_Array[LL].MeanVal=a.meanobs;
				m_pSim->m_Val_Array[LL].NSE=a.nse;
				m_pSim->m_Val_Array[LL].RMSE=a.rmse;
				m_pSim->m_Val_Array[LL].ME=a.me;
			}
			m_ValPGFile[i].file->ResetPos();
			m_ValPGFile[i].simvalfile->ResetPos();

			if(!MultiFlag||Final) {

			//	vector<float> vv=m_ValPGFile[i].pRes->GetResidualVector(0,0);
				m_ValPGFile[i].file->CloseFile();
				m_ValPGFile[i].simvalfile->CloseFile();
				if(m_ValPGFile[i].pRes!=nullptr) m_ValPGFile[i].pRes->CloseFile();
			}
	}

	if ((m_pModelInfo->ActualNoMulRun - 1) % size_t(m_StructureModel.m_Structure.AggregateCalibNumber + 0.01) == m_StructureModel.m_Structure.AggregateCalibNumber-1) {
		m_pSim->m_InformCriteria.AIC = AIC_Func(m_pSim->m_InformCriteria.SSE, m_pSim->m_InformCriteria.n_points, m_pSim->m_InformCriteria.n_parameters);
		m_pSim->m_InformCriteria.BIC = BIC_Func(m_pSim->m_InformCriteria.SSE, m_pSim->m_InformCriteria.n_points, m_pSim->m_InformCriteria.n_parameters);
	}

	if(m_pSim->m_ValSum_Array.size()>0) {
		MR* pMR;
		pMR=dynamic_cast<MR*>(m_pSim);
		pMR->UpdateValidationPerformance_SmartFlexVariables();/*
		for(size_t i=0; i<m_pSim->m_ValSum_Array.size();i++) {
			auto valsum=pMR->m_ValidationData.GetValSumStruct(i);
			if (valsum.SimValueSource == SIMSUMSOURCE::MULTIFILESTAT) {
				bool vector = false;
				SIMB sumv;
				sumv.pBase = m_pSim->GetPtrFromIntType(valsum.OutputType, valsum.Group, valsum.Name);
				sumv.TabIndex = valsum.LocalIndex;
				vector = sumv.pBase->Is_Vector();
				for (size_t ii = 0; ii < Stat.size(); ii++) {
					if ((m_Outputs[ii].pBase == sumv.pBase && !vector) || (m_Outputs[ii].pBase == sumv.pBase&&m_Outputs[ii].TabIndex == sumv.TabIndex)) {
						double value = 0;
						switch (valsum.OutputType) {
						case 0:
							value = Stat[ii].Initial;
							break;
						case 1:
							if (vector)
								value = ((OutVector*)sumv.pBase)->GetValue(sumv.TabIndex);
							else
								value = ((OutSingle*)sumv.pBase)->GetValue();
							break;
						case 2:
							value = Stat[ii].Min;
							break;
						case 3:
							value = Stat[ii].Max;
							break;
						case 4:
							value = Stat[ii].AccTotal / (T_End - T_StartSimPeriod);
							break;
						case 5:
							value = Stat[ii].AccTotal;
							break;
						}
						m_pSim->m_ValSum_Array[i].ME = float(value - valsum.ObsValue);
						m_pSim->m_ValSum_Array[i].RMSE = m_pSim->m_ValSum_Array[i].ME*m_pSim->m_ValSum_Array[i].ME;
						m_pSim->m_ValSum_Array[i].LogLi = m_StructureModel.m_Structure.SumLikelihoodSingleValue(i, value);
						m_pSim->m_DocFile.m_TsVLogLiSum += float(m_pSim->m_ValSum_Array[i].LogLi);
						break;
					}
				}
			}
			else if (valsum.SimValueSource == SIMSUMSOURCE::CSVFLEXFILE) {




			}
			else if (valsum.SimValueSource == MULTI_OUTPGFILE) {




			}
		}*/

	}

	return true;
}

void Simulator::Reset_DrivingFiles() 
{
	vector<SimB*> fpv;
	F* pF;
	fpv=m_pSim->GetPtrVector(simtype::PGFILE,true);
	for(size_t i=0; i<fpv.size(); i++) {
		pF=dynamic_cast<F*>(fpv[i]);
		if(pF!=nullptr) {
			if(pF->GetType()== simtype::PGFILE) { // Find pointer to File of DRIVE type
				CPG *pPG=pF->GetPointer(); // Get pointer to PG File
				pPG->ResetPos();			
			}
		}
	}
}
bool Simulator::WShed_Ini() {
	bool check=true;
	m_WshedOutputs=m_pSim->GetWShedOutputs();

	m_pSim->m_PG_WShed_In.SetCompleteRead(true);
	m_pSim->m_PG_WShed_Out.SetCompleteRead(true);
	string filenameout=m_pSim->GetWShedFileName_Out();
	if(filenameout.size()>0)
		check=m_pSim->m_PG_WShed_Out.CreateNewFile(m_WshedOutputs.size(), m_pModelInfo->m_OutRecord*m_pSim->m_DocFile.m_TimePeriodScaleFactor, 1);

	if(!check) return false;
	
	string filenamein=m_pSim->GetWShedFileName_In();
	if(filenamein.size()>0)
		check=m_pSim->m_PG_WShed_In.Open(filenamein);


	return check;
}
bool Simulator::WShed_Store() {
	bool check=true;

	return check;
}
bool Simulator::WShed_StoreWrite() {
	bool check=true;

	return check;
}
void Simulator::Set_StateVariables(bool Initial) {
	vector<SimB*> vp_final, vp_start, vp_single;

	SimB* pPtr;

	vp_start = m_pSim->GetPtrVector("State Variables", "");
	for (size_t i = 0; i < vp_start.size(); i++) {
		pPtr = vp_start[i];
		auto name = pPtr->GetName();
		if ((name == "WaterStorage" || name == "SoilHeat" || name.find("Humus") != string::npos || name.find("Litter") != string::npos || name.find("Litter2") != string::npos) && pPtr->Is_Vector()) {
			vp_final.push_back(pPtr);
		}
		else if (!pPtr->Is_Vector())
			vp_single.push_back(pPtr);

	}
	for (auto pPtr : vp_final) {
		X* pX = static_cast<X*>(pPtr);
		for (size_t i = 0; i < pX->GetNumberOfFlags(); i++) {
			if (Initial) pX->SetInitial(i, pX->GetValue(i));
			else pX->SetFinal(i, pX->GetValue(i));
		}
	}
	for (auto pPtr : vp_single) {
		Xs* pXs = static_cast<Xs*>(pPtr);
		if (Initial)	pXs->SetInitial(pXs->GetValue());
		else pXs->SetFinal(pXs->GetValue());
	}

}
