#include "NC_ExternalInputs.h"
#include "./Functors/NC_ExternalInputs_Functors.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../PLANT_NC/NC_Plant.h"
#include "../Atmosphere/AtmModel.h"
#include "../PLANT/PlantModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/WaterUptake.h"
#include "../Structure/StructureModel.h"
#ifndef COUPSTD	
#include "../../Util/MFC_Util.h"
#else
#include "../../Util/FUtil.hpp"
#endif
						   
NC_ExternalInputs::NC_ExternalInputs(void)
{
	ModuleName="Meteorological Data";
	ModuleNo=7;
	ModuleConditions="";
	m_FileNameParTable = "";
	m_NoUnderScore = false;

}

bool NC_ExternalInputs::Ini()	{

//		Use Parameters
//	Use Profile, only	: NL
  //  Character*256 FNew
    //Integer L3

	//Allocate (ManDepthDist.back())
	
	N_Fert=0.;
	NumMan=p_Soil_WaterF->NL;
	ManDepthDist.assign(NumMan,0.0);

    N_Irrig_NO3=0.;
    N_Irrig_NH4=0.;

	if(ManureInput>0||N_Fertilization==2) {		// Initiate PG-file read;
		string filename=m_PG_Manure.GetFileName();
		if(FertilizationIndexFileName==1) {
			size_t LL=filename.rfind("_");
			AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
			Irrigation *pMet=&pAtm->m_Irrigation;
         //LL=Index(Filedriv1,'_')
			 if(LL>0) {
				string newname, num;
				auto n=int(pMet->PGFileName_Index);
				num=FUtil::STD_ItoAscii(int(pMet->PGFileName_Index));
				newname= filename.substr(0,LL+1)+num;
				newname+=".bin";
				m_PG_Manure.SetFileName(newname);
				m_PG_Manure.CloseFile();
				
			 } 

		}
		if(FertilizationIndexFileName==2) {
			Additional_Variables *p_AdditionalVar;
			StructureModel *pStructureModel=(StructureModel*)m_pModelInfo->GetModelPtr("Structure Model");
			p_AdditionalVar=&pStructureModel->m_Additional;

			if(p_AdditionalVar->Multi_PGFile_ID_Selector==0)
				m_PG_Manure.SetRepititionSequence(m_pModelInfo->ActualNoMulRun);
			else if(p_AdditionalVar->Multi_PGFile_ID_Selector==1) {
				size_t repno=size_t(p_AdditionalVar->SeqNo_MetFile+0.2);
				m_PG_Manure.SetRepititionSequence(repno);
			}
			else if(p_AdditionalVar->Multi_PGFile_ID_Selector==2) {
				long repid=long(p_AdditionalVar->IDNo_MetFile+0.2);
				m_PG_Manure.SetRepititionIdNo(repid);
			}

		}
		if(!m_PG_Manure.AreAllValuesAssigned())
		if(!m_PG_Manure.Open(filename)) {
		        Message("ERROR - Manure/Fertilization Driving variable file not found");
			return false;		        
		}
		m_PG_Manure.ResetPos();
		if(!CheckEcoDriv4()) {
			Message("ERROR - No valid driving variable found");
			m_pModelInfo->Run_Res_Id=false;		
			return false;
		}
	}

	if (N_Fertilization == 5 && ReadYearBasedManagementTable == 1) {

		size_t LL;
		if (m_pModelInfo->ActualNoMulRun <= 1) {
			LL = string::npos;		
			while (LL == string::npos) {
				string str;
				str = "You have to identify a File with the correct base of Filename \n";
				str += "The file should correspond to the Year Tables for Fertilization and Crop Management";
				//MFC_Util::MessageBox(str);
				//m_FileNameParTable = MFC_Util::GetCSVFileName();
				m_FileNameParTable=m_pFertTableStream->GetStrValue();
				//m_pFertTableStream->CheckFileName();
				LL = m_FileNameParTable.rfind("_");
				size_t DIRCHAR = m_FileNameParTable.rfind("\\");
				if(DIRCHAR!=string::npos)
					if (DIRCHAR > LL) m_NoUnderScore = true;
				else
					if(LL==string::npos)  m_NoUnderScore = true;
			}
			if (!m_NoUnderScore) {
				m_FileNameParTable = m_FileNameParTable.substr(0, LL + 1);
			}
		}
		string newname;


		if (CsvFileManagementIndex > 0 && !m_NoUnderScore) {
			string num = FUtil::STD_ItoAscii(int(CsvFileManagementIndex));
			newname = m_FileNameParTable;
			newname += num;
			newname += ".csv";
		}
		else
			newname = m_FileNameParTable;


		if (!ReadAndSetParameterValues(newname)) {
			string str = "Problem occurred when reading File:\n";
			str += newname;
#ifndef COUPSTD
			MFC_Util::MessageBox(str);
#else
			cout << str;

#endif
		}


	}

	if(N_Deposition==2) {		// Initiate PG-file read;
		string filename=m_PG_Deposition.GetFileName();
		if(DepositionIndexFileName==1) {
			size_t LL=filename.rfind("_");
			AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
			Irrigation *pMet=&pAtm->m_Irrigation;
         //LL=Index(Filedriv1,'_')
			 if(LL>0) {
				string newname, num;
				num=FUtil::STD_ItoAscii(int(pMet->PGFileName_Index));
				newname= filename.substr(0,LL+1)+num;
				newname+=".bin";
				m_PG_Deposition.SetFileName(newname);
				m_PG_Deposition.CloseFile();
				m_PG_Deposition.ResetPos();
			 } 

		}
		if(DepositionIndexFileName==2) {
			m_PG_Deposition.ResetPos();
			Additional_Variables *p_AdditionalVar;
			StructureModel *pStructureModel=(StructureModel*)m_pModelInfo->GetModelPtr("Structure Model");
			p_AdditionalVar=&pStructureModel->m_Additional;

			if(p_AdditionalVar->Multi_PGFile_ID_Selector==0)
				m_PG_Deposition.SetRepititionSequence(m_pModelInfo->ActualNoMulRun);
			else if(p_AdditionalVar->Multi_PGFile_ID_Selector==1) {
				size_t repno=size_t(p_AdditionalVar->SeqNo_MetFile+0.2);
				m_PG_Deposition.SetRepititionSequence(repno);
			}
			else if(p_AdditionalVar->Multi_PGFile_ID_Selector==2) {
				long repid=long(p_AdditionalVar->IDNo_MetFile+0.2);
				m_PG_Deposition.SetRepititionIdNo(repid);
			}

		}
		if(!m_PG_Deposition.Open(filename)) {
		        Message("ERROR - Manure/Fertilization Driving variable file not found");
			return false;		        
		}
		if(!CheckEcoDeposition()) {
			Message("ERROR - No valid driving variable found");
			m_pModelInfo->Run_Res_Id=false;		
			return false;
		}
	}

	m_YearFertTest = true;
	m_CountStartFertYear = 0;

	return true;
}
bool NC_ExternalInputs::IniFunctors() {
	if (N_Fertilization_Sw(N_Fertilization) == N_Fertilization_Sw::Parameters) f_NFert_WithinYear = Get_D1_Functor(MyFunc::N_FERTILIZATION_DAY);
	if (N_Fertilization_Sw(N_Fertilization) == N_Fertilization_Sw::Plant_Stage_Table) f_N_AgeStageFertilization = Get_D2_Functor(MyFunc::N_FERTILIZATION_STAGE_YEAR);
	if (N_Fertilization_Sw(N_Fertilization) == N_Fertilization_Sw::Year_Based_Table_1_app) f_N_YearStageFertilization = Get_D2_Functor(MyFunc::N_FERTILIZATION_TABLE_YEAR_1APP);
	if (N_Fertilization_Sw(N_Fertilization) == N_Fertilization_Sw::Year_Based_Table_3_app) f_N_YearStageFertilization = Get_D2_Functor(MyFunc::N_FERTILIZATION_TABLE_YEAR_3APP);
	return true;
}
bool NC_ExternalInputs::Dyn()
{

	Flux();
	Integ();
	return true;
}

void NC_ExternalInputs::Flux()
{

	PlantOrgModel* pPlantOrg=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	NC_Plant *pNC_Plant=&pPlantOrg->m_NC_Plant;
	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	Irrigation *pMet=&pAtm->m_Irrigation;
	PlantModel* p_Plant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake *pPlant=&p_Plant->m_WaterUptake;

//	Fertilization
	if(N_Fertilization_Sw(N_Fertilization)> N_Fertilization_Sw::off) {;
		  N_Fert_App=0.;
		  if(N_Fertilization_Sw(N_Fertilization) == N_Fertilization_Sw::Parameters) {
			  N_Fert_App = f_NFert_WithinYear(m_pModelInfo->JDayNum);
		  }
		  else if(N_Fertilization_Sw(N_Fertilization)== N_Fertilization_Sw::PG_file) {;
			N_Fert_App=GetEcoDat(&m_PGPos_Manure, FertNIpos)/T_Step;
		  }	
		  else if(N_Fertilization_Sw(N_Fertilization) == N_Fertilization_Sw::Plant_Stage_Table) {
			 N_Fert_App= f_N_AgeStageFertilization(pNC_Plant->SimPlantYearAge.front(), m_pModelInfo->JDayNum);
			}
		  else if (N_Fertilization_Sw(N_Fertilization) >= N_Fertilization_Sw::Year_Based_Table_1_app) 
			  N_Fert_App = f_N_YearStageFertilization(double(m_pModelInfo->JYearNum), m_pModelInfo->JDayNum);	
	
		N_Fert_NO3=N_Fert*N_FertDiss_k*(1-N_Fert_NH4_Frac);
		N_Fert_NH4=N_Fert*N_FertDiss_k*N_Fert_NH4_Frac;
	};

    if(N_Irrigation==1) {
		  if(pMet->Dripper==1) { 
				for(size_t jj=0; jj< SoilNitrateInflow.size(); jj++) SoilNitrateInflow[jj]=pMet->Wsource[jj]*N_Irrig_NO3_Conc/1000.;
				for(size_t jj=0; jj<SoilAmmoniumInflow.size(); jj++) SoilAmmoniumInflow[jj]=pMet->Wsource[jj]*N_Irrig_NH4_Conc/1000.;
				N_Irrig_NO3=sum(SoilNitrateInflow);
				N_Irrig_NH4=sum(SoilAmmoniumInflow)  ;
		  }
		  else {
				N_Irrig_NO3=pMet->IrrigationRate*N_Irrig_NO3_Conc/1000.;
				N_Irrig_NH4=pMet->IrrigationRate*N_Irrig_NH4_Conc/1000.;
		  };
      N_Irrig_App=N_Irrig_NO3+N_Irrig_NH4;
    }

//	Deposition;
	if(N_Deposition>0) {
		if(N_Deposition==1) {
		   N_Deposition_NO3=Dep_N_DryRate*(1.-Dep_NH4_FracD)
							+pNC_Plant->NDrivInfiltration*Dep_N_WetConc*(1.-Dep_NH4_FracW)/1.E3;
		   N_Deposition_NH4=Dep_N_DryRate*Dep_NH4_FracD
							+pNC_Plant->NDrivInfiltration*Dep_N_WetConc*Dep_NH4_FracW/1.E3;
		}
		else if(N_Deposition==2) {
			N_Deposition_NO3=GetEcoDat(&m_PGPos_Deposition, NO3NIpos)/365.25; // Converting Yearly to daily rate
			N_Deposition_NH4=GetEcoDat(&m_PGPos_Deposition, NH4NIpos)/365.25; // Converting Yearly to daily rate

		}

		for(size_t i=0; i<N_Deposition_Leaf.size(); i++) N_Deposition_Leaf[i]=Dep_N_toLeaf[i]*pPlant->LAI[i];
		   Total_N_DepositionLeaf=sum(N_Deposition_Leaf);
	}
//	Manure application;
	if(FaecesPool>0) {

		  ManureDay			=false;
		  N_Manure_NH4		= N_Manure_Litter	=  N_Manure_Faeces	=C_Manure_Litter	= C_Manure_Faeces	=0.;
		  if(ManureInput==0) {
			for(size_t i=0; i<Manure_NH4.size(); i++) {
				if(m_pModelInfo->JDayNum==int(Manure_DayNo[i]+0.1)&&m_pModelInfo->JYearNum%int(Manure_YearFreq[i]+0.1) ==int(ManureYearStart) ) {
					ManureDay=true;
					N_Manure_NH4	=Manure_NH4[i];
					N_Manure_Litter	=Manure_Litter[i];
					N_Manure_Faeces	=Manure_Faeces[i];
					C_Manure_Litter	=Manure_Litter[i]*Manure_CN_Litter[i];
					C_Manure_Faeces	=Manure_Faeces[i]*Manure_CN_Faeces[i];
					ManDepth 		=Manure_Depth[i];
					break;
				};
			}  //EndDo loopdays2;
		 }
		 else	{		// To read from Pg-file;
			//CALL READECO(T_Current, IManure, NumValidInput4, NDrivECODAT);
			ManureDay=false;
			if(ManNHIpos.ivar!=0) 
			    N_Manure_NH4	=GetEcoDat(&m_PGPos_Manure,ManNHIpos)/T_Step;
			else
			    N_Manure_NH4=0;
			
			if(ManNLNIpos.ivar!=0) 
			    N_Manure_Litter	=GetEcoDat(&m_PGPos_Manure, ManNLNIpos) / T_Step;
			else
			    N_Manure_Litter=0;
			
			if(ManFNIpos.ivar!=0) 
			    N_Manure_Faeces	=GetEcoDat(&m_PGPos_Manure,ManFNIpos) / T_Step;
			else
			    N_Manure_Faeces=0;
			
			if(CNBEDIpos.ivar!=0) 
			    C_Manure_Litter	=GetEcoDat(&m_PGPos_Manure,CNBEDIpos)*N_Manure_Litter / T_Step ;
			else
			    C_Manure_Litter	=Manure_CN_Litter.front()*N_Manure_Litter;
			
			if(CNFECIpos.ivar!=0)
			    C_Manure_Faeces	=GetEcoDat(&m_PGPos_Manure,CNFECIpos)*N_Manure_Faeces / T_Step;
			else
			    C_Manure_Faeces	=Manure_CN_Faeces.front()*N_Manure_Faeces;
			
			if(ManDepthIpos.ivar!=0) 
			    ManDepth 		=GetEcoDat(&m_PGPos_Manure,ManDepthIpos) / T_Step;

			if(N_Manure_NH4>0.||N_Manure_Litter>0.||N_Manure_Faeces>0.) 
				ManureDay=true;

		 }

	}
}
void NC_ExternalInputs::Integ() {

/*		Use Timeinfo;
	Use Profile, Only	: NL, DepthDist;
	Use General, Only	: InflowScalar, OutFlowScalar, OutFlow,InFlow, Intflow;
	Use NC_Organic, only	: O_State[_L1][_N_], O_State[_L1][_C_], O_State[_F][_N_], CFaeces;
;*/
	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");	Irrigation *pMet=&pAtm->m_Irrigation;
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	if(N_Fertilization>0) {
		InFlow(N_Fert, N_Fert_App);
		OutFlow(N_Fert, N_Fert_NO3);
		OutFlow(N_Fert, N_Fert_NH4);

		InFlow(SoilNitrate.front(), N_Fert_NO3);
		InFlow(SoilAmmonium.front(), N_Fert_NH4);
	}
  
    if(N_Irrigation>0&&pMet->Dripper==0) {
    	InFlow(SoilNitrate.front(), N_Irrig_NO3);
		InFlow(SoilAmmonium.front(), N_Irrig_NH4);
    }

	if(N_Deposition>0) {
		InFlow(SoilNitrate.front(), N_Deposition_NO3);
		InFlow(SoilAmmonium.front(), N_Deposition_NH4);
//		Add integration into plant later or make in Plant growth model;
	}
	DEPTH_type out;
	if(FaecesPool>0&&ManureDay) {
		out=pSoil->DepthDist(2, N_Manure_NH4, ManDepth, 0.1) ;
		ManDepthDist=out.Dist;
		InFlow(SoilAmmonium, ManDepthDist);

		out=pSoil->DepthDist(2, N_Manure_Litter, ManDepth, 0.1) ;
		ManDepthDist=out.Dist;
		InFlow(O_State[_L1][_N_], ManDepthDist);

		out=pSoil->DepthDist(2, N_Manure_Faeces, ManDepth, 0.1) ;
		ManDepthDist=out.Dist;
		InFlow(O_State[_F][_N_], ManDepthDist);

		out=pSoil->DepthDist(2, C_Manure_Litter, ManDepth, 0.1) ;
		ManDepthDist=out.Dist;
		InFlow(O_State[_L1][_C_], ManDepthDist);
		
		out=pSoil->DepthDist(2, C_Manure_Faeces, ManDepth, 0.1) ;
		ManDepthDist=out.Dist;
		InFlow(O_State[_F][_C_], ManDepthDist);
	}

}
bool NC_ExternalInputs::CheckEcoDriv4() {
	string str;
	int Imiss;	// Local Variable;
;
//     CLOSE(NUnitDriv10);
  //   Run_res_id=Iopenbin(FILEDriv10, NUnitDriv10, IVAR, IREC, DAT1, DAT2);
	// if(Run_res_id<0) return;
;
	// Allocate(NDrivEcoDat(IVAR));
;
   Imiss=0;
   FertNIpos.ivar=ManNHIpos.ivar=ManNLNIpos.ivar=CNBEDIpos.ivar=ManFNIpos.ivar=CNFECIpos.ivar=ManDepthIpos.ivar=0;
   FertNIpos.pgfile=ManNHIpos.pgfile=ManNLNIpos.pgfile=CNBEDIpos.pgfile=ManFNIpos.pgfile=CNFECIpos.pgfile=ManDepthIpos.pgfile=&m_PG_Manure;
   FertNIpos.discrete=ManNHIpos.discrete=ManNLNIpos.discrete=CNBEDIpos.discrete=ManFNIpos.discrete=CNFECIpos.discrete=ManDepthIpos.discrete=false;
   FertNIpos.shift=ManNHIpos.shift=ManNLNIpos.shift=CNBEDIpos.shift=ManFNIpos.shift=CNFECIpos.shift=ManDepthIpos.shift=0;
   FertNIpos.discrete = ManNHIpos.discrete = ManNLNIpos.discrete = CNBEDIpos.discrete = ManFNIpos.discrete = CNFECIpos.discrete = ManDepthIpos.discrete = true;
   bool ResetReadWrite=true;

   if (!m_PG_Manure.AreAllValuesAssigned())
	if(!m_PG_Manure.ReOpen()) return false;
   m_PG_Manure.SetEndPoints_ValidValue();
   FertNIpos.shift = ManNHIpos.shift = ManNLNIpos.shift = CNBEDIpos.shift = ManFNIpos.shift = CNFECIpos.shift = ManDepthIpos.shift = 720;


   for(size_t i=0; i<m_PG_Manure.GetNumVariables(); i++) {
     string name=m_PG_Manure.GetVarName(i+1);
	 string Id=m_PG_Manure.GetVarId(i+1);

	 name += Id;

	 for(size_t ii=0; ii<name.size();ii++) {
		if(int(name[ii])>=97) {
			name[ii]=char(int(name[ii])-32);
		}
	 }

         
                ;
     if(name.find("MANNH")!=string::npos&&ManureInput>0) {;
		if(ManNHIpos.ivar==0) 
			ManNHIpos.ivar=i+1;
		else
			Message("Warning - too many MANNH variables, extra not used is :");
		  
	 }	
	 else if (name.find("MANNLN") != string::npos&& ManureInput > 0) {
		 ;
		if(ManNLNIpos.ivar==0) 
			ManNLNIpos.ivar=i+1;
		else
          Message("Warning - too many ManNLN variables, extra not used is :");
	}
	 else if (name.find("CNBED") != string::npos && ManureInput > 0) {
		if(CNBEDIpos.ivar==0) 
			CNBEDIpos.ivar=i+1;
		else
          Message("Warning - too many CNBED variables, extra not used is :");
		  
	 }
     else if(name.find("MANFN")!=string::npos&&ManureInput>0) {
		if(ManFNIpos.ivar==0) 
			ManFNIpos.ivar=i+1;
		else
			Message("Warning - too many MANFN variables, extra not used is :");

	}
     else if(name.find("CNFEC")!=string::npos&&ManureInput>0) {
		if(CNFECIpos.ivar==0) 
			CNFECIpos.ivar=i+1;
		else
          Message("Warning - too many CNFEC variables, extra not used is :");
		  
	 }
	 else if(name.find("MANDEPTH")!=string::npos&&ManureInput>0) {
		if(ManDepthIpos.ivar==0) 
			ManDepthIpos.ivar=i+1;
		else
          Message("Warning - too many MANDEPTH variables, extra not used is :");
		  
	 }
	 else if(name.find("FERTILIZ")!=string::npos&&N_Fertilization==2) {
		if(FertNIpos.ivar==0)
			FertNIpos.ivar=i+1;
		else
          Message("Warning - too many Fertilizer variables, extra not used is :");
		FertNIpos.shift = 720;
		  
	 }		
     else {
          Imiss++;
		  str="Note - Not identified :";
		  str+=name.data();
          Message(str);
     }
   }


	NumValidInput4=m_PG_Manure.GetNumVariables();

	return true;

};

bool NC_ExternalInputs::CheckEcoDeposition() {
	string str;
	int Imiss;	// Local Variable;

   Imiss=0;
   NH4NIpos.ivar=NO3NIpos.ivar=0;
   NH4NIpos.discrete=NO3NIpos.discrete=false;
   NH4NIpos.shift=NO3NIpos.shift=0;
   NH4NIpos.pgfile=NO3NIpos.pgfile=&m_PG_Deposition;
 
   bool ResetReadWrite=true;
   if(!m_PG_Deposition.ReOpen()) return false;
   m_PG_Deposition.SetEndPoints_ValidValue();


   for(size_t i=0; i<m_PG_Deposition.GetNumVariables(); i++) {
     string name=m_PG_Deposition.GetVarName(i+1);
	 string Id=m_PG_Deposition.GetVarId(i+1);

	 for(size_t ii=0; ii<20;ii++) {
		if(int(name[ii])>=97) {
			name[ii]=char(int(name[ii])-32);
		}
	 }
         
                ;
     if(name.find("NH4")!=string::npos) {
		if(NH4NIpos.ivar==0) 
			NH4NIpos.ivar=i+1;
		else
			Message("Warning - too many MANNH variables, extra not used is :");
		  
	 }	
	 else if(name.find("NO3")!=string::npos) {;
		if(NO3NIpos.ivar==0) 
			NO3NIpos.ivar=i+1;
		else
          Message("Warning - too many ManNLN variables, extra not used is :");
	}
   	
     else {
          Imiss++;
		  str="Note - Not identified :";
		  str+=name.data();
          Message(str);
     }
   }


	NumValidDeposition=m_PG_Deposition.GetNumVariables();
	if(Imiss==0&&NumValidDeposition==2) return true;
		else 
			return false;

};

bool NC_ExternalInputs::End()
{
return true;
}
bool NC_ExternalInputs::ReadAndSetParameterValues(string newname)
{


	ifstream filestream;
	filestream.open(newname);

	if (!filestream.good()) return false;

	string line;
	line.resize(2000);
	size_t ich=1;
	filestream.getline(&line[0], 2000);
	size_t index=0;
	line.clear();
	line.resize(2000);
	while (filestream.getline(&line[0], 2000)) {
		size_t icom;
		icom = 1;
		double value;
		size_t var = 0;
		while (icom != string::npos) {
			string str;
			icom = line.find(',');
			if (icom == string::npos) {
				str = line;
				line = "";
			}
			else {
				str = line.substr(0, icom);
				line = line.substr(icom + 1);
				
			}
			value = FUtil::AtoFloat(str);
			switch (var) {
			case 0:
				N_Fert_Year[index] = value;
				p_NC_Plant->m_StartYears.at(index) = value;
				break;
			case 1:
				p_NC_Plant->m_SowingDays.at(index) = value;
				break;
			case 2:
				p_NC_Plant->m_EmergenceDays.at(index) = value;
				break;
			case 3:
				N_Fert_DayNo1[index] = value;
				break;
			case 6:
				N_Fert_Rate1[index] = value;
				break;
			case 4:
				N_Fert_DayNo2[index] = value;
				break;
			case 7:
				N_Fert_Rate2[index] = value;
				break;
			case 5:
				N_Fert_DayNo3[index] = value;
				break;
			case 8:
				N_Fert_Rate3[index] = value;
				break;
			case 9:
				p_NC_Plant->m_HarvestDays.at(index) = value;
				break;
			}
			var++;
		}
		line.clear();
		line.resize(300);
		index++;

	}







	return true;
}
NC_ExternalInputs::~NC_ExternalInputs(void)
{
}
