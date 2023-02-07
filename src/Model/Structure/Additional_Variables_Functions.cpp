#include "Additional_Variables_Functions.h"
#include "../PLANT/PlantModel.h"
#include "../Atmosphere/AtmModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../../Util/StatUtil.h"
#include "../Structure/Structure.h"




Additional_Variables_Functions::Additional_Variables_Functions(void)
{


}
Additional_Variables_Functions::~Additional_Variables_Functions(void)
{
}


bool Additional_Variables_Functions::Def()

{
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	p_Plant=&pPlant->m_WaterUptake;	
 	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	p_Irrigation=&pAtm->m_Irrigation;
 	SoilModel* pSoil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF=&pSoil->m_SoilFrost;
	p_Soil_HeatF=&pSoil->m_HeatPump;
	PlantOrgModel* pNCPlant = (PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	p_NC_Plant = &pNCPlant->m_NC_Plant;

	SoilOrgModel* pSoilOrg=(SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	//NC_SoilManagement *pNCSoil;
	m_pNCSoil=&pSoilOrg->m_NC_SoilOrg;



	ModuleNo=23;
	Sw* pSw;
	pSw= new Sw(&ResetAccValues,SINGLE, GENERAL, NOFPROC, NOBPROC,NORMAL); 
	Define(pSw, "Reset Acc Variables",0,"Off|On","WaterEq = 1");
	pSw= new Sw(&Soil_TempSensors,SINGLE, TEMPERATURE, SENSOR, NOBPROC,NORMAL);
	Define(pSw,  "Soil Temp Sensors",0,"Off|On","HeatEq = 1") ; 
	pSw= new Sw(&Soil_ThetaSensors,SINGLE,WATER ,SENSOR , NOBPROC,NORMAL);
	Define(pSw,  "Soil Theta Sensors",0,"Off|On","WaterEq = 1");  

	pSw= new Sw(&IGBP_LandID_Selector,SINGLE,WATER ,SENSOR , NOBPROC,NORMAL);
	Define(pSw,  "IGBP LandInfo",0,"Off|for Latitude|for Also CSoil|For All Soil Properties|Read info Multi-PG Metfile","WaterEq = 1");  

	pSw= new Sw(&Multi_PGFile_Var_Selector,SINGLE,WATER ,SENSOR , NOBPROC,NORMAL);
	Define(pSw,  "MetFile Extended Use",0,"Off|Retrieve GeoRef info|Also Retrieve Soil Info|Also Soil and Plant Info","WaterEq = 1");  
	pSw= new Sw(&Multi_PGFile_ID_Selector,SINGLE,GENERAL, NOFPROC , NOBPROC,NORMAL);
	Define(pSw,  "MetFile Station Selection",0,"MultiRunNo|Sequence No|Station ID No","WaterEq = 1|Dynamic indexed input file=2");  

	Ps *pPs;
	pPs= new Ps(&X_coordinate,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL); 

   	Define(pPs,"X-coordinate",0.,"m"," ",-1000.,1.E8);
	pPs= new Ps(&Y_coordinate ,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL); 
	Define(pPs,"Y-coordinate",0.,"m"," ",-1000.,1.E8);
	pPs= new Ps(&Z_coordinate,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL);    	
	Define(pPs,"Z-coordinate",0.,"m"," ",0.,1.E6);
	pPs= new Ps(&Land_ID,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL);    	
	Define(pPs,"LandID",0.,"-","IGBP LandInfo>0",1.,1.E8,1.,1.E8,10001);

	pPs= new Ps(&SeqNo_MetFile,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL);    	
	Define(pPs,"SeqNo Met File",1.,"-","Dynamic indexed input file=2|MetFile Station Selection=1",1.,1.E8);
	pPs= new Ps(&IDNo_MetFile,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL);    	
	Define(pPs,"IDno Met File",0.,"-","Dynamic indexed input file=2|MetFile Station Selection=2"  ,1.,1.E8);

	pPs= new Ps(&Scale,SINGLE, GENERAL, GEOPOS,NOBPROC, NORMAL);    	
	Define(pPs,"Spatial_Scale",1.,"m"," ",1.,1.E7);

//	call PnE(NSonds,1,1,100,1,100)				!Element is Table
	NE *pNSonds, *pNTempSensors, *pNThetaSensors, *pNZadokDays;

	pNZadokDays = new NE(&NZadokDays, 3, 10, 100, 10, 100);
	Define(pNZadokDays, "Number of Zadok Output Stages");
	P* pP;
	vector<double> initv;
	Tab* pTab;
	G* pG;

	pP = new P(&ZadokDays, pNZadokDays, SINGLE, GENERAL, SENSOR, NOBPROC, NORMAL);
	initv = { 30.,55.,90. };
	Define(pP, pNZadokDays, "Zadok_Stages", initv, "Zadok", "ZadokScale>0", 1., 100., 0., 100.);
	mTab_Zadoks.push_back(pP);
	pTab = new Tab(&mTab_Zadoks, SINGLE, TEMPERATURE, SENSOR, NOBPROC, NORMAL);
	Define(pTab, "ZadokStages", "Zadok", "ZadokScale>0");
	pG = new G(&ZadokDayValues, pNZadokDays, ATMBOUNDARY, TEMPERATURE, EBAL_PARTITIONING, NOBPROC, NORMAL);
	initv = { 0.,0.,0. };
	Define(pG, pNZadokDays, "ZadokDays", initv, "Days", "ZadokScale>0");

	f_ZadokShifts = ZadokShift2(p_NC_Plant->ZadokIndex, ZadokDays);



	pNSonds= new NE(&NSonds,1,1,100,1,20);
	Define(pNSonds, "Number of Sonds");

	pP= new P(&SondDepth,pNSonds, SINGLE, GENERAL, SENSOR,NOBPROC, NORMAL); 
	
	initv.assign(NSonds,0.2);
		Define(pP,pNSonds,"Depth",initv,"m","",0.001,100.,0.1, 100.);
		mTab_Sonds.push_back(pP);
	//minvalue=0.001,maxvalue=100.,warningmin=0.01,warningmax=10.,UserLevel=0)
	pNTempSensors= new NE(&NTempSensors,1,1,100,1,100);
	Define(pNTempSensors, "Number of Temp Sonds");
    pNThetaSensors= new NE(&NThetaSensors,1,1,100,1,100);
	Define(pNThetaSensors, "Number of Theta Sonds");
	initv.assign(NTempSensors,0.15);

	pP= new P(&TempSensorHigh,pNTempSensors, SINGLE, TEMPERATURE, SENSOR,NOBPROC, NORMAL); 

		Define(pP,pNTempSensors,"Upper TempSensor",initv,"m","",0.001,100.,0.01,10.);
		mTab_Temp.push_back(pP);
	pP= new P(&TempSensorLow,pNTempSensors, SINGLE, TEMPERATURE, SENSOR,NOBPROC, NORMAL); 
		initv.assign(NTempSensors,0.17);
		Define(pP,pNTempSensors,"Lower TempSensor",initv,"m","",0.001,100.,0.01,10.);
		mTab_Temp.push_back(pP);
		initv.assign(NThetaSensors,0.15);
	pP= new P(&ThetaSensorHigh,pNThetaSensors, SINGLE, WATER, SENSOR,NOBPROC, NORMAL); 
		Define(pP,pNThetaSensors ,"Upper ThetaSensor",initv,"m","",0.001,100.,0.01,10.);
		mTab_Theta.push_back(pP);
	initv.assign(NThetaSensors,0.17);
	pP= new P(&ThetaSensorLow,pNThetaSensors, SINGLE, WATER, SENSOR,NOBPROC, NORMAL); 
		Define(pP,pNThetaSensors,"Lower ThetaSensor",initv,"m","", 0.001,100.,0.01,10.);	
		mTab_Theta.push_back(pP);

	
	pTab= new Tab(&mTab_Temp, SINGLE, TEMPERATURE, SENSOR,NOBPROC, NORMAL); 
		Define(pTab, "Temperature Sensors", "Upper TempSensor|Lower TempSensor","HeatEq = 1|Soil Temp Sensors =1");
	pTab= new Tab(&mTab_Theta, SINGLE, WATER, SENSOR,NOBPROC, NORMAL); 
		Define(pTab,"Theta Sensors", "Upper ThetaSensor|Lower ThetaSensor","WaterEq = 1|Soil Theta Sensors = 1");
	pTab= new Tab(&mTab_Sonds, SINGLE, GENERAL, SENSOR,NOBPROC, NORMAL); 
		Define(pTab,"Depths of Sonds", "Depth","WaterEq = 1");

	pG= new G(&ThetaSensors ,pNThetaSensors, SINGLE, WATER, SENSOR,NOBPROC, NORMAL); 
	initv.assign(NThetaSensors,0.);
   Define(pG,pNThetaSensors ,"Theta Sensors" ,initv,"vol %","WaterEq = 1");
	initv.assign(NTempSensors,0.);
	pG= new G(&TempSensors ,pNTempSensors, SINGLE, WATER, SENSOR,NOBPROC, NORMAL); 
	initv.assign(NTempSensors,0.);
   Define(pG, pNTempSensors,"Temp Sensors" ,initv,UNIT_TYPES::TEMP_UNIT,"HeatEq = 1");
   Gs *pGs;
   pGs= new Gs(&Evapotranspiration, ATMBOUNDARY, WATER, EVAPOTRANSPIRATION,NOBPROC, NORMAL);
	  Define(pGs, "Evapotranspiration", 5.,UNIT_TYPES::WATERFLOW_UNIT,"Evaporation > 0|WaterEq = 1");
  pGs= new Gs(&TotalLatentFlow, ATMBOUNDARY, HEAT, EVAPOTRANSPIRATION,NOBPROC, NORMAL);
		Define(pGs, "TotalLatentFlow", 5.,UNIT_TYPES::HEATFLOW_UNIT,"Evaporation > 0|WaterEq = 1");
  pGs= new Gs(&TotalSensibleFlow, ATMBOUNDARY, HEAT, EVAPOTRANSPIRATION,NOBPROC, NORMAL);
		Define(pGs,"TotalSensibleFlow",5.,UNIT_TYPES::HEATFLOW_UNIT,"Evaporation >0|WaterEq = 1");

  NE *pNumPlants;
  pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");


  pGs= new Gs(&p_Plant->TempCanopy, ATMBOUNDARY, TEMPERATURE, EBAL_PARTITIONING,NOBPROC, NORMAL);
		Define(pGs ,"TempCanopy",5.,UNIT_TYPES::TEMP_UNIT,"Evaporation >0|WaterEq = 1");

  pGs= new Gs(&PlantWaterStorage, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
    Define(pGs, "PlantAvailableWater" ,0.,"mm","WaterEq = 1");

	initv.assign(NSonds,0.);
  pG= new G(&AccWStorage,pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
	Define(pG,pNSonds ,"Acc_WStorage" ,initv,"mm","WaterEq = 1");
  pG= new G(&AccSStorage,pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
    Define(pG,pNSonds, "Acc_Salt_Storage" ,initv,"mg/m2","WaterEq = 1|SaltTracer>0");
  pG= new G(&AccNH4Storage, pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
	Define(pG,pNSonds, "Acc_NH4_Storage" ,initv,"gN/m2","WaterEq = 1|Nitrogen and Carbon>0");
  pG= new G(&AccNO3Storage,pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
	Define(pG,pNSonds, "Acc_NO3_Storage" ,initv,"gN/m2","WaterEq = 1|Nitrogen and Carbon>0");
  pG= new G(&AccMinNStorage,pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
	Define(pG,pNSonds, "Acc_MinN_Storage" ,initv,"g/Nm2","WaterEq = 1|Nitrogen and Carbon>0");
  pG= new G(&AccCSoilStorage,pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
	Define(pG,pNSonds ,"Acc_OrgC_Storage" ,initv,"g/Nm2","WaterEq = 1|Nitrogen and Carbon>0");
  pG= new G(&AccNSoilStorage,pNSonds, ACCUMULATOR , WATER, SENSOR,NOBPROC, NORMAL);
	Define(pG,pNSonds,"Acc_OrgN_Storage" ,initv,"g/Nm2","WaterEq = 1|Nitrogen and Carbon>0");
  pG= new G(&AccRStorage,pNSonds, ACCUMULATOR , SALT, SENSOR,NOBPROC, NORMAL);
    Define(pG,pNSonds, "AccM_Resistivity" ,initv,"Ohm m","WaterEq = 1|SaltTracer>0");

//	Call G(Hidden, MC_TDiff,"MC_TDiff" ,NumPlants,0.,	"-","PlantType = 3");
//	Call G(Hidden, MC_TDiffC,"MC_TDiffC" ,NumPlants,0.,	"-","PlantType = 3");

	Xs *pXs;
	pXs= new Xs(&AccSurfaceRunoff, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs,  "AccSurfaceRunoff", 0.0,"mm","WaterEq >= 1");
	pXs= new Xs(&AccTotalRunoff, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs,  "AccTotalRunoff", 0.0,"mm","WaterEq >= 1");
	pXs= new Xs(&AccSoilEvap, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs,  "AccSoilEvap", 0.0,"mm","Evaporation > 0|WaterEq >= 1");
	pXs= new Xs(&AccSoilEvapAndTransp, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs,  "AccSoilEvapAndTransp", 0.0,"mm","Evaporation > 0|WaterEq >= 1");
	pXs= new Xs(&AccTotalEvap, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs,  "AccTotalEvap", 0.0,"mm","Evaporation > 0|WaterEq >= 1");
	pXs= new Xs(&TotalDiffStorage, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "TotalDiffStorage",0.0,"mm","WaterEq >= 1");
	pXs= new Xs(&WaterBalanceCheck, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "WaterBalanceCheck",0.0,"mm","WaterEq >= 1");
	pXs= new Xs(&AccPrecipitation, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccPrecipitation",0.0,"mm","WaterEq >= 1");
	pXs= new Xs(&AccPrecAdjustSnow	, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccPrecAdjustSnow",0.0,"mm","WaterEq >= 1|SnowAdjustment>0");
	pXs= new Xs(&AccPrecandIrrig	, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccPrecandIrrig",0.0,"mm","WaterEq >= 1|Irrigation >= 1");
	pXs= new Xs(&AccLateralInflowWater, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccLateral_WInflow",0.0,"mm","WaterEq >= 1|LateralInput = 1");
	pXs= new Xs(&AccSpoolRunOn, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccSpoolRunOn",0.0,"mm","WaterEq >= 1|LateralInput = 1");
	pXs= new Xs(&AccTotalWaterInput, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccTotalWaterInput",0.0,"mm","WaterEq >= 1|LateralInput = 1");
	pXs= new Xs(&AccThroughfall, ACCUMULATOR , WATER, BUDGET,NOBPROC, NORMAL);
		Define(pXs, "AccThroughFall",0.0,"mm","WaterEq >= 1")	;


return true;
}


double Additional_Variables_Functions::GetTotalWaterStorage() {
	double out;
	out=p_Irrigation->SurfacePool+p_Irrigation->WTotSnow+p_Plant->IntStorage;
	for(size_t i=0; i<p_Soil_WaterF->WaterStorage.size();i++) {
		out+=p_Soil_WaterF->WaterStorage[i]+p_Soil_WaterF->WaterHFD[i];
	}
	if(p_Plant->PlantWaterStorage>0)
		out+=StatUtil::sum(p_Plant->PlantWater);
	return out;

}
double Additional_Variables_Functions::GetWaterBalanceCheck() {
	double out;
	if(m_pModelStructure->Irrigation>0)
		out=AccPrecandIrrig;
	else
		out=AccPrecipitation;
	if(p_Irrigation->SnowAdjustment>0)	out+=AccPrecAdjustSnow;
	if(m_pModelStructure->LateralInput>0||p_Soil_WaterF->GWSoFlow>0) out+=AccLateralInflowWater;
	out-=TotalDiffStorage+AccTotalEvap+AccTotalRunoff;
	return out;
}
vector<double> Additional_Variables_Functions::GetSensorValues(vector<double> Value, vector<SOIL_SENSORS> *Sens) {
	vector<double> out;
	out.resize(Sens->size());
	for(size_t i=0; i<Sens->size(); i++) {
		out[i]=(Sens->at(i).UpperWeight1*Value[Sens->at(i).UpperLayer1]+Sens->at(i).UpperWeight2*Value[Sens->at(i).UpperLayer2]+
					Sens->at(i).LowerWeight1*Value[Sens->at(i).LowerLayer1]+Sens->at(i).LowerWeight2*Value[Sens->at(i).LowerLayer2])/2.;
	}
	return out;
}
double Additional_Variables_Functions::GetSondStorage(size_t isond, vector<double> *Values) {
	double out=0;

	for(size_t i=0; i<Values->size(); i++) {
		if(p_Soil_WaterF->DepthLayers[i]<=SondDepth[isond]) 
			out+=Values->at(i);
		else if(i==0)
			out+=SondDepth[isond]/p_Soil_WaterF->ThicknessLayers[i]*Values->at(i);
		else if(SondDepth[isond]>=p_Soil_WaterF->DepthLayers[i-1])
			out+=(SondDepth[isond]-p_Soil_WaterF->DepthLayers[i-1])/p_Soil_WaterF->ThicknessLayers[i]*Values->at(i);
	}
	return out;
}
ORG_PROBES Additional_Variables_Functions::Get_Org_SondStorage(size_t isond) {
	ORG_PROBES  out;
	out.Carbon=0;
	out.Nitrogen=0;
	double carbon=0;
	double nitrogen=0;

	for(size_t i=0; i<m_pNCSoil->O_State[_H][_C_].size(); i++) {
		carbon=m_pNCSoil->O_State[_H][_C_][i]+m_pNCSoil->O_State[_L1][_C_][i]+m_pNCSoil->O_State[_F][_C_][i]+m_pNCSoil->O_State[_L2][_C_][i]+m_pNCSoil->O_State[_MIC][_C_][i];
		nitrogen=m_pNCSoil->O_State[_H][_N_][i]+m_pNCSoil->O_State[_L1][_N_][i]+m_pNCSoil->O_State[_F][_N_][i]+m_pNCSoil->O_State[_L2][_N_][i]+m_pNCSoil->O_State[_MIC][_N_][i];
		if(p_Soil_WaterF->DepthLayers[i]<=SondDepth[isond]) {
			out.Carbon+=carbon;
			out.Nitrogen+=nitrogen;
		}
		else if(i==0) {
			out.Carbon+=SondDepth[isond]/p_Soil_WaterF->ThicknessLayers[i]*carbon;
			out.Nitrogen+=SondDepth[isond]/p_Soil_WaterF->ThicknessLayers[i]*nitrogen;

			}
		else if(SondDepth[isond]>=p_Soil_WaterF->DepthLayers[i-1]) {
			out.Carbon+=(SondDepth[isond]-p_Soil_WaterF->DepthLayers[i-1])/p_Soil_WaterF->ThicknessLayers[i]*carbon;
			out.Nitrogen+=(SondDepth[isond]-p_Soil_WaterF->DepthLayers[i-1])/p_Soil_WaterF->ThicknessLayers[i]*nitrogen;
		}
	}
	return out;
}
