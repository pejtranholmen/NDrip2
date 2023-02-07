#include "Meteorological_Data.h"
#include "../SOIL/Soil_HeatF.h"
#include "../Structure/Additional_Variables.h"
#include "../../NewBase/ModelBase.h"
#include "../../PG/PGUtil.h"
#include "../../Simulator/Simulator.h"
#include "../Structure/Structure.h"
#include "./Functors/Met_Functors.h"
#include "../Structure/Functors/Structure_Functors.h"




						   
Meteorological_Data::Meteorological_Data(void)
{
	ModuleName="Meteorological Data";
	ModuleNo=7;
	ModuleConditions="";
	TairIpos.ivar = TSurfIpos.ivar = PrecIpos.ivar = RadGlobIpos.ivar = HumRelIpos.ivar = VapourAirIpos.ivar = WetBulbIpos.ivar = RadNetIpos.ivar = 0;
	RadInLongIpos.ivar = TrafficIpos.ivar = 0;
	CloudFIpos.ivar = TempDiff_Ipos.ivar = 0;
	DBSunIpos.ivar = WSpeedIpos.ivar = IrrigIpos.ivar = PotEvapIpos.ivar = 0;
	}

bool Meteorological_Data::Ini()	{
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();

    DrivPG1=false;
    //Prec_Index=1;
	VapourPAir=0;

	if(WaterEq_Sw(m_pModelStructure->WaterEq)== WaterEq_Sw::On_Richard_Equation) {
		if(PrecInput_Sw(PrecInput)>= PrecInput_Sw::PG_FILE_FIRST)
			DrivPG1=true;
		else if (PotTranspInput_Sw(PotTranspInput) == PotTranspInput_Sw::PG_FILE_FIRST&&m_pModelStructure->PlantType==1&&m_pModelStructure->Evaporation==1)
			DrivPG1=true;
		else if (m_pModelStructure->RadiationInput>0) {
			if(HumRelInput_Sw(HumRelInput) >= HumRelInput_Sw::PG_FILE_FIRST)
				DrivPG1=true;
			else if(WSpeedInput_Sw(WSpeedInput) >= WSpeedInput_Sw::PG_FILE_FIRST||RadNetInput_Sw(RadNetInput) == RadNetInput_Sw::PG_FILE_FIRST
				||RadGlobInput_Sw(RadGlobInput) == RadGlobInput_Sw::PG_FILE_FIRST||RadInLongInput_Sw(RadInLongInput) == RadInLongInput_Sw::PG_FILE_FIRST)
				DrivPG1=true;
			else if(CloudInput_Sw(CloudInput) >= CloudInput_Sw::PG_FILE_FIRST||DBSunInput_Sw(DBSunInput) == DBSunInput_Sw::PG_FILE_FIRST)
				DrivPG1= true;
		}	
		else if(m_pModelStructure->IrrigationInput==0&&m_pModelStructure->Irrigation>0) 
			DrivPG1=true;
	}
    else if(HBV_Soil_Sw(m_pModelStructure->HBV_Soil)> HBV_Soil_Sw::Off) {
        if(PrecInput_Sw(PrecInput)>=PrecInput_Sw::PG_FILE_FIRST) 
			DrivPG1=true;
		else if (PotTranspInput_Sw(PotTranspInput)== PotTranspInput_Sw::PG_FILE_FIRST&&m_pModelStructure->PlantType==1&&m_pModelStructure->Evaporation==1)
			DrivPG1=true;
	}
		
	if(TAirInput_Sw(TAirInput)>=TAirInput_Sw::PG_FILE_FIRST||TSurfInput_Sw(TSurfInput) == TSurfInput_Sw::PG_FILE_FIRST)
		DrivPG1=true;

	MISSRNT=false;
	MISSRIS=false;
	MISSCLOUD=false;
   	MISSRIL=false;	//       ! DG 27/6

	
    if(DrivPG1) {
		string filename;
        if(ClimIndexFileName_Sw(ClimIndexFileName) == ClimIndexFileName_Sw::SINGLE_PG_FILE) {
			filename=m_PG_Meteor.GetFileName();
			size_t LL=filename.rfind("_");
   			 if(LL!=string::npos) {
				string newname, num;
				int n=int(PGFileName_Index+0.1);
				num=FUtil::STD_ItoAscii(int(PGFileName_Index));
				newname= filename.substr(0,LL+1)+num;
				newname+=".bin";
				m_PG_Meteor.SetFileName(newname);
				m_PG_Meteor.CloseFile();
			 } 

        }

		if(m_pModelInfo->ActualNoMulRun<=1||ClimIndexFileName_Sw(ClimIndexFileName) == ClimIndexFileName_Sw::SINGLE_PG_FILE) {
			//m_PG_Meteor.ReOpen();
			string koll = m_PG_Meteor.GetFileName();
			if (!m_PG_Meteor.ReadContentOfPGFileAndOpenMainPGStreamForReading(m_PG_Meteor.GetFileName())) {
				Message("Error when reading Meteorological input file\n" + m_PG_Meteor.GetFileName() + "\nPlease check file status");
				return false;

			};
			m_pModelInfo->Run_Res_Id=CheckEcoDriv1();
			m_PG_Meteor.ResetPos();
			if (!m_pModelInfo->Run_Res_Id) {
				Message("Error when reading Meteorological input file\n"+ m_PG_Meteor.GetFileName()+"\nPlease check file status");
				return false;
			}
			if(TAirInput_Sw(TAirInput) == TAirInput_Sw::PG_FILE_FIXED) TairIpos.ivar=int(TAir_Index);
			if(PrecInput_Sw(PrecInput) == PrecInput_Sw::PG_FILE_FIXED) PrecIpos.ivar=int(Prec_Index);
			if (HumRelInput_Sw(HumRelInput) == HumRelInput_Sw::PG_FILE_FIXED) HumRelIpos.ivar = int(HumRel_Index);
			if(VapourAirInput_Sw(VapourAirInput) == VapourAirInput_Sw::PRESSURE_PG_FILE_FIXED) VapourAirIpos.ivar=int(VapourAir_Index);
			if(WSpeedInput_Sw(WSpeedInput)== WSpeedInput_Sw::PG_FILE_FIXED) WSpeedIpos.ivar=int(WSpeed_Index);
			if(RadNetInput_Sw(RadNetInput)== RadNetInput_Sw::PG_FILE_FIXED) RadNetIpos.ivar=int(RNT_Index);
			if(RadGlobInput_Sw(RadGlobInput) == RadGlobInput_Sw::PG_FILE_FIXED) RadGlobIpos.ivar=int(RadGlob_Index);
			if(CloudInput_Sw(CloudInput) == CloudInput_Sw::PG_FILE_FIXED) CloudFIpos.ivar=int(Cloud_Index);
			if(DBSunInput_Sw(DBSunInput) == DBSunInput_Sw::PG_FILE_FIXED) DBSunIpos.ivar=int(DBSun_Index);
			if(RadInLongInput_Sw(RadInLongInput) == RadInLongInput_Sw::PG_FILE_FIXED) RadInLongIpos.ivar=int(RadInLong_Index);
		}
		else {
			m_PG_Meteor.ReadContentOfPGFileAndOpenMainPGStreamForReading(m_PG_Meteor.GetFileName());
			m_pModelInfo->Run_Res_Id = CheckEcoDriv1();
			m_PG_Meteor.ResetPos();


		}
		if(ClimIndexFileName_Sw(ClimIndexFileName) == ClimIndexFileName_Sw::MULTI_PG_FILE) {
			m_PG_Meteor.ResetPos();
			if(p_AdditionalVar->Multi_PGFile_ID_Selector==0)
				m_PG_Meteor.SetRepititionSequence(m_pModelInfo->ActualNoMulRun);
			else if(p_AdditionalVar->Multi_PGFile_ID_Selector==1) {
				size_t repno=size_t(p_AdditionalVar->SeqNo_MetFile+0.2);
				m_PG_Meteor.SetRepititionSequence(repno);
			}
			else if(p_AdditionalVar->Multi_PGFile_ID_Selector==2) {
				long repid=long(p_AdditionalVar->IDNo_MetFile+0.2);
				m_PG_Meteor.SetRepititionIdNo(repid);
			}
			auto spatialId=m_PG_Meteor.GetRepititionSequence();
			p_AdditionalVar->Y_coordinate=m_PG_Meteor.GetVarLat(1, spatialId);
			p_AdditionalVar->X_coordinate=m_PG_Meteor.GetVarLong(1, spatialId);
			p_AdditionalVar->Z_coordinate=m_PG_Meteor.GetVarAlt(1, spatialId);
			if (p_AdditionalVar->Multi_PGFile_Var_Selector == 1) {
				Latitude= m_PG_Meteor.GetVarLat(1, spatialId);
			}
		}
    }    
	if (SweClimScenarious_Sw(SweClimScenarious) > SweClimScenarious_Sw::No) {
		if (!OpenSweClimFile()) {
			return false;
		}
	}
	if(LateralInput_Sw(m_pModelStructure->LateralInput)== LateralInput_Sw::In_driving_file) {
		string filenamein=m_PG_LaterWater.GetFileName();
		m_PG_Source.SetFileName(filenamein);
		m_PG_Source.ResetPos();
		m_PG_Source.SetCompleteRead(true);
		if(filenamein.size()>0) {
			if(m_PG_Source.Open(filenamein)) {
				m_PG_Source.CloseFile();
				if(CheckDriveHorizontal()) {
					m_pModelStructure->LateralGWSource=1;
					m_PG_Source.ResetPos();

				}
			}
			else  {
				m_pModelStructure->LateralGWSource=0;
				return false;
			}
		}

	}

	else if(LateralInput_Sw(m_pModelStructure->LateralInput) ==LateralInput_Sw::WaterShed_approach) {
		string filenamein=((ModelBase*)m_pModelMap)->GetWShedFileName_In();
		m_PG_Source.SetFileName(filenamein);
		m_PG_Source.ResetPos();
		m_PG_Source.SetCompleteRead(true);
		if(filenamein.size()>0) {
			if(m_PG_Source.Open(filenamein)) {
				m_PG_Source.CloseFile();
				if(CheckDriveHorizontal()) {
					m_pModelStructure->LateralGWSource=1;
					m_PG_Source.ResetPos();

				}
			}
			else  {
				m_pModelStructure->LateralGWSource=0;
				return false;
			}
		}
		else {
			LateralIpos.ivar = 0;
			LateralIpos.ivar = RunOnIpos.ivar = SnowImportIpos.ivar = SaltDrainIpos.ivar = 0;
	

		}
		Wsource.assign(Wsource.size(), 0.);

	}


	return true;
}
bool Meteorological_Data::IniFunctors() {



	f_pgen = Get_D1_Functor(MyFunc::PREC_GENERATOR); //PrecGenerator(PrecRate, int(PrecFreq + 0.001), int(PrecStartDay + 0.001));
	f_TempAlt = Get_D1_Functor(MyFunc::AIRTEMP_ALT_CORR); //; AirTempAltCorrection(AltMetStation, AltSimPosition, TairLapseRate);
	f_avp_rh = Get_D2_PhysFunctor(FUNC_CHOICE::AVP_TEMP_RH);
	auto koll = f_pgen(10.);


	f_MeanDailyTemp = DailyAirMeanTemp(m_pModelStructure->TimeResolution, TAir, T_Step);
	f_PrecCorrFactor = Get_D1_Functor(MyFunc::PRECCORR);// PrecCorrFactor(PRainLim, PSnowLim, PrecA0Corr, PrecA1Corr);
	f_svp = Get_D1_PhysFunctor(FUNC_CHOICE::SVP);
	f_svp = Physf(FUNC_CHOICE::SVP);
	f_SinDay = Get_D1_PhysFunctor(FUNC_CHOICE::AIRTEMP_DAY);
	auto koll1 = f_SinDay(0.5);
	f_SinYear = Get_D1_PhysFunctor(FUNC_CHOICE::AIRTEMP_YEAR);
	f_TempAlt = Get_D1_Functor(MyFunc::AIRTEMP_ALT_CORR);

	return true;
}
bool Meteorological_Data::Dyn()
{
	Flux();
	return true;
}

void Meteorological_Data::Flux()
{
	if (TAirInput_Sw(TAirInput) >= TAirInput_Sw::PG_FILE_FIRST) {	//! Air Temperature
		auto test = GetEcoDat(&m_PGPOS_Meteor[TairIpos.ivar], TairIpos);
		if (test > 0.9 * MISSING) TAir = f_TempAlt(test);
	}
	else {
		if (TairCycle_Sw(TairCycle) == TairCycle_Sw::DIURNAL) TAir = f_SinDay(m_pModelInfo->T_Current);
		else TAir = f_SinYear(m_pModelInfo->JDayNum);
	}
	if (SweClimScenarious_Sw(SweClimScenarious) > SweClimScenarious_Sw::No) TAir = NewTemp(TAir, m_pModelInfo->JDayNum);

	//! rullande dygnsmedelvärde
	TAirDailyMean = f_MeanDailyTemp(TAir);
	//! Markytetemperatur=Lufttemperatur
	if (SoilEvap <= 0 || m_pModelStructure->WaterEq == 0)
		TempBareSoil = TAir;

	//! Markytetemperatur=indata
	if (TSurfInput_Sw(TSurfInput) == TSurfInput_Sw::PG_FILE_FIRST)
		((Soil_HeatF*)p_Soil_HeatF)->TempSoilSurf = GetEcoDat(&m_PGPOS_Meteor[TSurfIpos.ivar], TSurfIpos);

	//! Traffic input
	if (TrafficInput_Sw(TrafficInput) == TrafficInput_Sw::ON)
		TrafficIntensity = GetEcoDat(&m_PGPOS_Meteor[TrafficIpos.ivar], TrafficIpos);

	//! Drivdata för vattenekvationen
	if (TAirGlobRad_Sw(TAirGlobRad) == TAirGlobRad_Sw::NOT_USED && m_pModelStructure->OnlyNC > 0)
		return;
	else if (m_pModelStructure->WaterEq > 0) {

		//	! Nederbörd
		if (PrecInput_Sw(PrecInput) == PrecInput_Sw::PARAMETERS) {
			PrecMeas = f_pgen(m_pModelInfo->JDayNum);
			PrecCorr = PrecMeas;
		}
		else {
			PrecMeas = GetEcoDat(&m_PGPOS_Meteor[PrecIpos.ivar], PrecIpos);
			double Factor = f_PrecCorrFactor(TAir);
			if (Factor > 0.05 && Factor < 20.)   PrecCorr = PrecMeas * Factor;   // Adjustment
			else PrecCorr = PrecMeas;
		}

		if (SweClimScenarious_Sw(SweClimScenarious) > SweClimScenarious_Sw::No)
			PrecCorr = NewPrec(PrecCorr, m_pModelInfo->JDayNum);

		//! Relativ luftfuktighet
		if (HumRelInput_Sw(HumRelInput) == HumRelInput_Sw::PARAMETERS || HumRelIpos.ivar == 0)
			HumRel = HumRelMean;
		else if (HumRelInput_Sw(HumRelInput) >= HumRelInput_Sw::PG_FILE_FIRST) {
			HumRel = GetEcoDat(&m_PGPOS_Meteor[HumRelIpos.ivar], HumRelIpos);
			HumRel = min(100., HumRel);
		}


		//! Ångtryck

		if (VapourAirInput_Sw(VapourAirInput) == VapourAirInput_Sw::RELATIVEHUMIDITY || (VapourAirIpos.ivar == 0 && WetBulbIpos.ivar == 0))
			VapourPAir = f_avp_rh(TAir, HumRel);
		else  if (VapourAirInput_Sw(VapourAirInput) >= VapourAirInput_Sw::PRESSURE_PG_FILE_FIRST && VapourAirIpos.ivar > 0) {
			if (VapourAirInput_Sw(VapourAirInput) == VapourAirInput_Sw::VPD_PG_FILE_FIRST) {
				float Def = GetEcoDat(&m_PGPOS_Meteor[VapourAirIpos.ivar], VapourAirIpos);
				VapourPAir = f_svp(TAir) - Def;
			}
			else
				VapourPAir = GetEcoDat(&m_PGPOS_Meteor[VapourAirIpos.ivar], VapourAirIpos);

			if (VapourPAir < 0)
				VapourPAir = f_avp_rh(TAir, HumRel);
			if (SweClimScenarious_Sw(SweClimScenarious) > SweClimScenarious_Sw::No)
				VapourPAir = NewVapourP(VapourPAir, TAir, m_pModelInfo->JDayNum);
		}
		else  if (WetBulbIpos.ivar > 0)
			VapourPAir = f_svp(GetEcoDat(&m_PGPOS_Meteor[WetBulbIpos.ivar], WetBulbIpos));

		if (VapourPAir > f_svp(TAir)) VapourPAir = f_svp(TAir);

		if (HumRelInput == 0 || HumRelIpos.ivar == 0 && VapourAirIpos.ivar > 0)
			HumRel = 100 * (VapourPAir / f_svp(TAir));

		VapourPDef = f_svp(TAir) - VapourPAir;

		//! Vindhastighet
		if (WSpeedInput_Sw(WSpeedInput) == WSpeedInput_Sw::PARAMETERS || WSpeedIpos.ivar == 0 || !DrivPG1)
			WindSpeed = WSpeedMean;
		else if (DrivPG1)
			WindSpeed = max(0.1, GetEcoDat(&m_PGPOS_Meteor[WSpeedIpos.ivar], WSpeedIpos));
		//! Molnighet
		if (CloudInput_Sw(CloudInput) == CloudInput_Sw::ESTIMATED_SUNSHINE || CloudInput_Sw(CloudInput) == CloudInput_Sw::ESTIMATED_TEMPDAYDIFF)
			MISSCLOUD = true;
		else if (CloudFIpos.ivar > 0) {
			CloudFrac = GetEcoDat(&m_PGPOS_Meteor[CloudFIpos.ivar], CloudFIpos);
			if (CloudFrac < 0.) {
				MISSCLOUD = true;
				CloudFrac = CloudFMean;
			}
			else if (CloudFrac >= 0 && CloudFrac <= 1.)
				MISSCLOUD = false;
			else {
				MISSCLOUD = true;
				CloudFrac = CloudFMean;
			}
		}
		else if (CloudInput_Sw(CloudInput) == CloudInput_Sw::PARAMETERS&&DBSunInput_Sw(DBSunInput)==DBSunInput_Sw::NOT_USED)
			CloudFrac = CloudFMean;
		else if(DBSunInput_Sw(DBSunInput) != DBSunInput_Sw::NOT_USED)
			MISSCLOUD = true;

		if (TempDayDiffInput_Sw(TempDayDiffInput) >= TempDayDiffInput_Sw::PG_FILE_FIRST && TempDiff_Ipos.ivar > 0)
			TempDayDifference = GetEcoDat(&m_PGPOS_Meteor[TempDiff_Ipos.ivar], TempDiff_Ipos);
		else if (TempDayDiffInput_Sw(TempDayDiffInput) == TempDayDiffInput_Sw::PARAMETERS)
			TempDayDifference = TempDayDiff_Fixed;

		//! Sunshine
		if (DBSunInput_Sw(DBSunInput) == DBSunInput_Sw::PG_FILE_FIRST && DBSunIpos.ivar > 0) {
			DurBriSun = GetEcoDat(&m_PGPOS_Meteor[DBSunIpos.ivar], DBSunIpos);
			MISSCLOUD = true;
		}

		//! Nettostrålning
		if (RadNetInput_Sw(RadNetInput) >= RadNetInput_Sw::PG_FILE_FIRST) {
			RadNetTot = GetEcoDat(&m_PGPOS_Meteor[RadNetIpos.ivar], RadNetIpos);
			if (m_pModelStructure->HeatFluxUnit == 1) RadNetTot = RadNetTot * m_pModelInfo->SecPerDay;
			if (abs(RadNetTot) < 1.E-4 || abs(RadNetTot) > 1.E30) {
				RadNetTot = 0.;
				MISSRNT = true;
			}
			else
				MISSRNT = false;
		}
		else
			MISSRNT = true;

		//! Långvågig strålning

		if (RadInLongInput_Sw(RadInLongInput) == RadInLongInput_Sw::PG_FILE_FIRST && RadInLongIpos.ivar > 0) {
			RadInLong = GetEcoDat(&m_PGPOS_Meteor[RadInLongIpos.ivar], RadInLongIpos);
			if (m_pModelStructure->HeatFluxUnit == 1) RadInLong = RadInLong * m_pModelInfo->SecPerDay;
			if (abs(RadInLong) < 1.E-4)
				MISSRIL = true;
			else
				MISSRIL = false;
		}
		else
			MISSRIL = true;


		//! Kortvågig strålning

		if (RadGlobInput_Sw(RadGlobInput) >= RadGlobInput_Sw::PG_FILE_FIRST && RadGlobIpos.ivar > 0) {
			RadInShort = GetEcoDat(&m_PGPOS_Meteor[RadGlobIpos.ivar], RadGlobIpos);
			if (m_pModelStructure->HeatFluxUnit == 1) RadInShort = RadInShort * m_pModelInfo->SecPerDay;
			MISSRIS = false;
			if (RadInShort < -1.E6)
				MISSRIS = true;
			else if (RadInShort > 1.E8)
				MISSRIS = true;

			if (RadInShort < 0.)
				RadInShort = 0.;
		}
		else
			MISSRIS = true;


		// Horisontellt vatteninflöde

		if (m_pModelStructure->LateralInput >= 1 && LateralIpos.ivar != string::npos && LateralIpos.ivar > 0) {

			if (m_pModelStructure->LateralGWSource == 1)
				Wsource = GetEcoDat(&m_PGPOS_Source, Wsource.size(), LateralIpos);
			if (Wsource[0] < -1.E38) Wsource.assign(Wsource.size(), 0.);

			if (SnowPloughInput == 1)
				SnowImport = GetEcoDat(&m_PGPOS_Source, SnowImportIpos);
			else
				SnowImport = 0.;
		}
		else {
			SpoolRunOn = 0.;
			Wsource.assign(Wsource.size(), 0.);
			SnowImport = 0.;
		}
		if (RunOnInput == 1 && RunOnIpos.ivar > 0)
			SpoolRunOn = GetEcoDat(&m_PGPOS_Source, RunOnIpos);

	}
}


bool Meteorological_Data::End()
{
	if(m_pModelInfo->ActualNoMulRun==m_pModelInfo->TotalNoMulRun||m_pModelInfo->ActualNoMulRun==0) {
		if(DrivPG1) m_PG_Meteor.CloseFile();
	}
	else {
		m_PG_Meteor.ResetPos();
	}
/*	if(DrivPG1)				Close(Unit=NunitDriv1)
	if(LateralInput>=1)	Close(Unit= NUnitDriv7)
	if(allocated(TairMeanVector)) deallocate(TairMeanVector)

	if(	Allocated(DT_A))	  DeAllocate(DT_A)    
	if(	Allocated(DT_B))	  DeAllocate(DT_B)    
	if(	Allocated(PrecCoef))  DEAllocate(PrecCoef)
	*/

return true;
}
Meteorological_Data::~Meteorological_Data(void)
{
}
bool Meteorological_Data::CheckEcoDriv1()
{
	string str;

	int  Imiss,  IrrigCount;//	! Local Variable
	int RadInLong_Count, RadGlob_Count, Prec_Count, Tair_Count, HumRel_Count, VapourAir_Count;
	int WSpeed_Count, RNT_Count, Cloud_Count, DBSun_Count, TempDiff_Count, TrafficCount;     
	if(!m_PG_Meteor.ReOpen()) {
		m_PG_Meteor.Open(m_PG_Meteor.GetFileName());
		m_PG_Meteor.CloseFile();
		if(!m_PG_Meteor.ReOpen()) return false;



	}
   m_pModelInfo->Run_Res_Id=true;
   m_PG_Meteor.CheckEquiDistance();

   m_PG_Meteor.SetEndPoints_ValidValue();
   TairIpos.ivar=TSurfIpos.ivar=PrecIpos.ivar=RadGlobIpos.ivar=HumRelIpos.ivar=VapourAirIpos.ivar=WetBulbIpos.ivar=RadNetIpos.ivar=0;
   RadInLongIpos.ivar=TrafficIpos.ivar=0;
   CloudFIpos.ivar=TempDiff_Ipos.ivar=0;
   DBSunIpos.ivar=WSpeedIpos.ivar=IrrigIpos.ivar=PotEvapIpos.ivar=0;   

   TairIpos.pgfile=TSurfIpos.pgfile=PrecIpos.pgfile=RadGlobIpos.pgfile=HumRelIpos.pgfile=VapourAirIpos.pgfile=WetBulbIpos.pgfile=RadNetIpos.pgfile=&m_PG_Meteor;
   RadInLongIpos.pgfile=TrafficIpos.pgfile=&m_PG_Meteor;
   CloudFIpos.pgfile=TempDiff_Ipos.pgfile=&m_PG_Meteor;
   DBSunIpos.pgfile=WSpeedIpos.pgfile=IrrigIpos.pgfile=PotEvapIpos.pgfile=&m_PG_Meteor; 

   TairIpos.discrete=TSurfIpos.discrete=HumRelIpos.discrete=VapourAirIpos.discrete=WetBulbIpos.discrete=false;
   RadInLongIpos.discrete=TrafficIpos.discrete=false;
   CloudFIpos.discrete=TempDiff_Ipos.discrete=PotEvapIpos.discrete=false;
   DBSunIpos.discrete=WSpeedIpos.discrete=false;
   IrrigIpos.discrete=RadNetIpos.discrete=PrecIpos.discrete=RadGlobIpos.discrete=true;

   int minshift;

 //  pModelStructure = ((Simulator*)pMod)->GetSimStructurePointer();
  // auto kalle = pModelStructure->TimeResolution;
	switch(m_pModelStructure->TimeResolution) {
	case 0:
		minshift=0;
		break;
	case 1:
		minshift=720;
		if(m_PG_Meteor.IsEquiDistance()) {
	   // Check time within day when using daily mean value.
	   // Normally the time should be at 12:00 and minshift of 720 minutes are applied.
	   // In case of have time at 00:00 this minshift should be 0 minutes.
			auto imin=m_PG_Meteor.GetLongTimeStart();
			auto hour=PGUtil::HourFunction(imin);
			if(hour==0) minshift=0;
		}
		break;
	
	case 2:
		minshift=30;
		if(m_PG_Meteor.IsEquiDistance()) {
	   // Check time within day when using daily mean value.
	   // Normally the time should be at 12:00 and minshift of 720 minutes are applied.
	   // In case of have time at 00:00 this minshift should be 0 minutes.
			auto imin=m_PG_Meteor.GetLongTimeStart();
			auto min=PGUtil::MinOfHourFunction(imin);
			if(min==0) minshift=0;
		}
		break;


		break;
	case 3:
		minshift=5;
		break;
	}
   TairIpos.shift=TSurfIpos.shift=PrecIpos.shift=RadGlobIpos.shift=HumRelIpos.shift=VapourAirIpos.shift=WetBulbIpos.shift=RadNetIpos.shift=minshift;
   RadInLongIpos.shift=TrafficIpos.shift=minshift;
   CloudFIpos.shift=TempDiff_Ipos.shift=minshift;
   DBSunIpos.shift=WSpeedIpos.shift=IrrigIpos.shift=PotEvapIpos.shift=minshift;   


   if(DBSunInput==1) CloudInput=0;
   if(VapourAirInput>0&&m_pModelStructure->Evaporation<1) 
	   VapourAirInput=0;
   
   if(CloudInput>0&&m_pModelStructure->RadiationInput<=0) 
		CloudInput=0;
   
   Imiss=RadInLong_Count=IrrigCount=TrafficCount=0;
   RadGlob_Count=Prec_Count=Tair_Count=HumRel_Count=VapourAir_Count=0;
   WSpeed_Count=RNT_Count=Cloud_Count=DBSun_Count=TempDiff_Count=0;



	for(size_t i=0; i<m_PG_Meteor.GetNumVariables(); i++) {
     string name=m_PG_Meteor.GetVarName(i+1);
	 string Id=m_PG_Meteor.GetVarId(i+1);

	 for(size_t ii=0; ii<20;ii++) {
		if(int(name[ii])>=97) {
			name[ii]=char(int(name[ii])-32);
		}
	 }
                
      if(name.find("TEMP")!=string::npos) {
        if( name.find("DIFF")!=string::npos) {
           TempDiff_Count++;
           if(TempDiff_Count==TempDiff_Index) 
                TempDiff_Ipos.ivar=i+1;
        }
		else if(name.find("AIR")!=string::npos) {
           Tair_Count++;
		   if(Tair_Count==TAir_Index) 
                TairIpos.ivar=i+1;
        } 
        else if(name.find("SURFACE")!=string::npos&&TSurfIpos.ivar==0) 
           TSurfIpos.ivar=i+1;
		else if(name.find("WET")!=string::npos&&WetBulbIpos.ivar==0) 
		   WetBulbIpos.ivar=i+1;
		else if(name.find("DEW POINT")!=string::npos&&WetBulbIpos.ivar==0) 
		   WetBulbIpos.ivar=i+1;           
		else if(TairIpos.ivar==0){
		   Tair_Count++;
           if(Tair_Count==TAir_Index) 
                TairIpos.ivar=i+1;
     
        }
	  }
     else if(name.find("PREC")!=string::npos) {
        Prec_Count++;
        if(Prec_Count==Prec_Index||PrecInput==1)    
			PrecIpos.ivar=i+1;
	 }	
     else if(name.find("GLOBAL")!=string::npos&&RadGlobIpos.ivar==0) {
        RadGlob_Count++;
        if(RadGlob_Count==RadGlob_Index||RadGlobInput==1) 
			RadGlobIpos.ivar=i+1;
	 }	
     else if(name.find("RIS")!=string::npos&&RadGlobIpos.ivar==0) {
        RadGlob_Count++;
        if(RadGlob_Count==RadGlob_Index||RadGlobInput==1) 
			RadGlobIpos.ivar=i+1;
	}     
	 else if(name.find("HUM")!=string::npos&&HumRelIpos.ivar==0) {
	    HumRel_Count++;
	    if(HumRel_Count==HumRel_Index||HumRelInput==1) 
			HumRelIpos.ivar=i+1;
	}	
	 else if(name.find("VAPOUR")!=string::npos&&VapourAirIpos.ivar==0) {
	    VapourAir_Count++;
		if (name.find("DEF")!=string::npos) VapourAirInput = 5;
	    if(VapourAir_Count==VapourAir_Index||VapourAirInput==1) 
			VapourAirIpos.ivar=i+1;
	}	
	else if(name.find("HR")!=string::npos&&HumRelIpos.ivar==0) {
	    HumRel_Count++;
	    if(HumRel_Count==HumRel_Index||HumRelInput==1) 
			HumRelIpos.ivar=i+1;
	}	
	else if(name.find("WIND")!=string::npos&&WSpeedIpos.ivar==0) {
	    WSpeed_Count++;
	    if(WSpeed_Count==WSpeed_Index||WSpeedInput==1)
			WSpeedIpos.ivar=i+1;
	}	
	else if(name.find("RNT")!=string::npos&&RadNetIpos.ivar==0) {
	    RNT_Count++;
	    if(RNT_Count==RNT_Index||RadNetInput==1) 
			RadNetIpos.ivar=i+1;
	}	
	else if(name.find("RAD")!=string::npos&&RadNetIpos.ivar==0) {
		if(name.find("NET")!=string::npos) {
	        RNT_Count++;
	        if(RNT_Count==RNT_Index||RadNetInput==1) 
			    RadNetIpos.ivar=i+1;
		}
		else if(name.find("LONG")!=string::npos&&RadInLongIpos.ivar==0) {
	         RadInLong_Count++;
	        if(RadInLong_Count==RadInLong_Index||RadGlobInput==1) 
		        RadInLongIpos.ivar=i+1;
		    
		 }  
	}
	else if(name.find("RIL")!=string::npos&&RadInLongIpos.ivar==0) {
			 RadInLong_Count++;
			if(RadInLong_Count==RadInLong_Index||RadGlobInput==1) 
				RadInLongIpos.ivar=i+1;
	}
	else if(name.find("CLOUD")!=string::npos&&CloudFIpos.ivar==0) {
	     Cloud_Count++;
	     if(Cloud_Count==Cloud_Index||CloudInput==1) 
			CloudFIpos.ivar=i+1;
	 }	
	 else if((name.find("SUNSHINE") != string::npos || name.find("Duration of")!=string::npos||Id.find("DBS")!=string::npos)&&DBSunIpos.ivar==0) {
	     DBSun_Count++;
	     if(DBSun_Count==DBSun_Index||DBSunInput==1) 
			DBSunIpos.ivar=i+1;
	}	
	else if(name.find("IRRIG")!=string::npos) {
	        IrrigCount++;
			if(IrrigCount==int(IrrigV_Index)) IrrigIpos.ivar=i+1;
	}
	 else if(name.find("TRAFFIC")!=string::npos) {
	        TrafficCount++;
			TrafficIpos.ivar=i+1;
	 }
	 else if(name.find("POTEN")!=string::npos&&PotEvapIpos.ivar==0) 
			PotEvapIpos.ivar=i+1;
	 else if(name.find("EPOT")!=string::npos&&PotEvapIpos.ivar==0) 
			PotEvapIpos.ivar=i+1;
	 else if(name.find("DURA")!=string::npos&&DBSunIpos.ivar==0) {
	 	  if(name.find("SUN")!=string::npos) {
	        DBSun_Count++;
	        if(DBSun_Count==DBSun_Index||DBSunInput==1) 
			    DBSunIpos.ivar=i+1;
		    }	 	  

	 }
     else {
          Imiss++;
		  str="Not identified :"+name;
		  Message(str);

     }
 }
 if(Imiss>0)  { 
		 Message(FUtil::STD_ItoAscii(Imiss)+" - Driving variable not identified");

		 if(TairIpos.ivar==0&&TAirInput==1) {
		   Message("ERROR! - Air Temperature variables missing");
		   m_pModelInfo->Run_Res_Id=false;
		   Message("Necessary key words are: Air Temperature");
		 }
		 if(TempDiff_Ipos.ivar==0&&TempDayDiffInput>0) {
			Message("ERROR! - Within Day Temperature Difference (TempDayDiff) variables missing");
		   m_pModelInfo->Run_Res_Id=false;
		   Message("Necessary key words are: Diff and Temp");

		 }
		 if(TSurfIpos.ivar<=0&&TSurfInput==1) {
		   Message("ERROR! - Surface temperature variable missing");
		   Message("Necessary key words are: Surface Temperature");
		   m_pModelInfo->Run_Res_Id=false;
		 }
		 if(PrecIpos.ivar==0&&PrecInput==1) {
		   Message("ERROR! - Precipitation variables missing");
		  Message("Necessary key words are: Precipitation");
		   m_pModelInfo->Run_Res_Id=false;
		 }

		 if(RadGlobIpos.ivar<=0&&RadGlobInput==1&&m_pModelStructure->RadiationInput>0) {
		   Message("ERROR! - Global radiation variable missing");
		  Message("Necessary key words are: Global or RIS");
		   m_pModelInfo->Run_Res_Id=false;
		 }

		if(HumRelIpos.ivar<=0&&HumRelInput==1&&m_pModelStructure->Evaporation>1) {
			Message("Error - Relative humidity variable missing");
		   Message("Necessary key words are: Humidity or Vapour or HR");
			m_pModelInfo->Run_Res_Id=false;
		}

		if(WetBulbIpos.ivar<=0&&VapourAirInput==2&&m_pModelStructure->Evaporation>1) {
			Message("Error - Wet bulb temperature variable missing");
		   Message("Necessary key words are: Temp and Wet");
			m_pModelInfo->Run_Res_Id=false;
		}


		if(RadNetIpos.ivar<=0&&RadNetInput==1&&m_pModelStructure->RadiationInput>0) {
			Message("Error - Net radiation variable missing");
		   Message("Necessary key words are: RNT or Net Radiation");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(RadInLongIpos.ivar<=0&&RadInLongInput==1&&m_pModelStructure->RadiationInput>0) {
			Message("Error - Incoming Longwave radiation variable missing");
			Message("Necessary key word is: RIL");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(CloudFIpos.ivar<=0&&CloudInput==1&&m_pModelStructure->RadiationInput>0) {
			Message("Error - Cloudiness variable missing");
		   Message("Necessary key word is : Cloudiness");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(DBSunIpos.ivar<=0&&DBSunInput==1&&m_pModelStructure->RadiationInput>0) {
			Message("Error - Durations of bright sunshine missing");
		   Message("Necessary key words are: DBS or Dura and Sun");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(WSpeedIpos.ivar<=0&&WSpeedInput==1&&m_pModelStructure->RadiationInput>0&&m_pModelStructure->OnlyNC==0) {
			Message("Error - Wind speed is missing ");
			Message("Necessary key words is: Wind");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(IrrigIpos.ivar<=0&&m_pModelStructure->Irrigation==1&&m_pModelStructure->IrrigationInput==0) {
			Message("Error - Irrigation variable missing");
			Message("Necessary key words are: Irrigation");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(PotEvapIpos.ivar<=0&&PotTranspInput==1&&m_pModelStructure->Evaporation==1) {
			Message("Error - Potential evaporation is missing");
			Message("Necessary key words are: Potential or EPOT");
			m_pModelInfo->Run_Res_Id=false;
		}
		if(VapourAirIpos.ivar<=0&&VapourAirInput==1) {
			Message("Error - Vapour pressure is missing");
			Message("Necessary key word is: Vapour");
			m_pModelInfo->Run_Res_Id=false;
		}

		

	}
	

	return m_pModelInfo->Run_Res_Id;
}
//Subroutine CheckDriveHorizontal


bool Meteorological_Data::CheckDriveHorizontal() {

	if(!m_PG_Source.ReOpen()) return false;
	int Imiss;
	size_t DrainageCount, SaltDrainCount;
   Imiss=0;
   LateralIpos.ivar=RunOnIpos.ivar=SnowImportIpos.ivar=SaltDrainIpos.ivar=0;
   DrainageCount=SaltDrainCount=0;
   LateralIpos.pgfile=&m_PG_Source;
   RunOnIpos.pgfile = &m_PG_Source;

    int minshift;
	switch(m_pModelStructure->TimeResolution) {
	case 0:
		minshift=0;
		break;
	case 1:
		minshift=720;
		break;
	
	case 2:
		minshift=30;
		break;
	case 3:
		minshift=5;
		break;
	}
	RunOnIpos.shift=LateralIpos.shift=minshift;
	RunOnIpos.discrete=LateralIpos.discrete=true;

      
   for(size_t i=0; i<m_PG_Source.GetNumVariables(); i++) {
		 string name=m_PG_Source.GetVarName(i+1);
		 string Id=m_PG_Source.GetVarId(i+1);

		 for(size_t ii=0; ii<20;ii++) {
			if(int(name[ii])>=97) {
				name[ii]=char(int(name[ii])-32);
			}
			if (int(Id[ii]) >= 97) {
				Id[ii] = char(int(Id[ii]) - 32);
			}

		 }

     if(name.find("WATERDRAINFLOW")!=string::npos||Id.find("WSOURCE")!=string::npos) {
		if(LateralIpos.ivar==0) LateralIpos.ivar=i+1;
		if(DrainageCount<Wsource.size()) 
			DrainageCount++;
		else 
          Message("Warning - too many drainage variables, extra not used is :"+name);
	 }
	 else if(name.find("SPOOLRUNOFF")!=string::npos|| name.find("SPOOL") != string::npos) {
		if(RunOnIpos.ivar==0) 
			RunOnIpos.ivar=i+1;
		else
          Message("Warning - too many drainage variables, extra not used is :"+name);
	 }
	 else if(name.find("SNOW MASS EXPORT")!=string::npos) {
		if(SnowImportIpos.ivar==0) 
			SnowImportIpos.ivar=i+1;
		else
          Message("Warning - too many snow export, extra not used is :"+name);
	 }
	 else if(name.find("SALTDRAINFLOW")!=string::npos) {
		if(SaltDrainIpos.ivar==0) 	SaltDrainIpos.ivar=i;
		if(SaltDrainCount<Wsource.size()) 
			SaltDrainCount++;
		else
          Message("Warning - too many snow export, extra not used is :"+name);
	 }
  }

  if(DrainageCount!=Wsource.size()&&m_pModelStructure->LateralGWSource==1) {
          Message("Error Too few lateral input source flow variables in file :"); 
		  m_pModelInfo->Run_Res_Id=false;
  }
  if(SaltDrainCount!=Wsource.size()&&m_pModelStructure->SaltGWSource==1) {
	  Message("Error Too few lateral input salt source flow variables in file :"+m_PG_Source.GetFileName());
	  m_pModelInfo->Run_Res_Id=false;
  }

  if(RunOnIpos.ivar==0&&RunOnInput==1) {
          Message("Error Too few lateral input variables in file :"+m_PG_Source.GetFileName());
		  m_pModelInfo->Run_Res_Id=false;
  }
  if(SnowImportIpos.ivar==0&&SnowPloughInput>0) {
          Message("Error Too few lateral input variables in file :"+m_PG_Source.GetFileName());
		  m_pModelInfo->Run_Res_Id=false;
  }

  return m_pModelInfo->Run_Res_Id;
}

