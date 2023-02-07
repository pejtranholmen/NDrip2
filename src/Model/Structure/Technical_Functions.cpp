#include "Technical_Functions.h"
#include "../SOIL/SoilModel.h"
#include "../Atmosphere/AtmModel.h"
#include "../PLANT/PlantModel.h"
#include "../HBV/HBVModel.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../../NewBase/ModelBase.h"

Technical_Functions::Technical_Functions(void)
{



}
Technical_Functions::~Technical_Functions(void)
{
}


bool Technical_Functions::Def()
{
		ModuleNo=1;
		ModuleConditions="";
 		SoilModel* pSoil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
		p_Soil_WaterF=&pSoil->m_SoilFrost;
		PlantOrgModel *pPlantNC=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
		p_NC_Plant=&pPlantNC->m_NC_Plant;
		PlantModel *pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
		p_Plant=&pPlant->m_WaterUptake;
		SoilOrgModel *pSoilNC=(SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
		p_NC_Soil=&pSoilNC->m_NC_SoilOrg;
		Sw* pSw;
		pSw= new Sw(&Develop_Option, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,  "Develop",0,"No|Yes","Develop <100");
		pSw= new Sw(&EnableInputPGSalt, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,  "EnableInputPGSalt",0,"No|Yes","Develop <100");
	   	pSw= new Sw(&EnableInputN, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,  "EnableInputN",0,"off|on","Develop < 100");
		
	
		pSw= new Sw(&AVERAGEX, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Average_State",1,"off|on");
		pSw= new Sw(&AVERAGET, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Average_Flow",1,"off|on");
		pSw= new Sw(&AVERAGEG, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Average_Auxiliary",1,"off|on");
		pSw= new Sw(&AVERAGED, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Average_Driving",1,"off|on");
	   	pSw= new Sw(&StartWithPrevious, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "StartWithPrevious",0,"off|on");
	//!   	Define(pSw,   DeleteOutFile,"DeleteOutputBinFile",0,"off|on")	   	
	   	pSw= new Sw(&CreateFileResiduals, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "TimeSerie Errors File",1,"off|on", "ValidationMode>0");
	//!   
		pSw= new Sw(&OUTFORN, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "OUTFORN",0,"off|on","Develop <100");

		pSw= new Sw(&INSTATE, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "INSTATE",0,"off|on","Develop <100");
		pSw= new Sw(&OUTSTATE, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "OUTSTATE",0,"off|on","Develop <100");
		pSw= new Sw(&DRIVPG, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "TypeOfDrivingFile",0,"No standard climate file|Standard driving file|Standard+Additional file", "Develop < 100");
		pSw= new Sw(&DRIVNPG, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "TypeOfNDrivingFile",0,"No standard N input file|Standard driving file", "Develop <100");
	   	pSw= new Sw(&LISALLV, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "LISALLV",1,"off|on","Develop <100");
	   	pSw= new Sw(&CHAPAR, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "CHAPAR",0,"off|on","Develop < 100");
	   	pSw= new Sw(&ADDSIM, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "ADDSIM",0,"off|on","Develop < 100");

	   	pSw= new Sw(&EnableSaltSource, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "EnableSaltSource",0,"off|on","Develop < 100");
        pSw= new Sw(&DrainFileUse, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,  "DrainFileUse",0,"off/on","Develop <0");
	   	
	   	pSw= new Sw(&ValidationMode, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,"ValidationMode",0,"off|one|two|three|four|five|six|seven|eight|nine|ten|eleven|tvelve|firteen|fourteen|fithteen|sixteen");
        
		//! Deleted by pej nov 05 if(StartStateValues) StartWithPrevious=1
		pSw= new Sw(&PressureHeadSign, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "PressureHeadSign",0,"Negative|Positive","Develop < 100");
		pSw= new Sw(&TimeResolution, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "TimeResolution",0,"Within day (timestep)|Daily mean values|Hourly|10 minutes");
	    pSw= new Sw(&IndexForValid, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "IndexForValidation",0,"From Curson in Menu|From serie","Develop <100");

		pSw = new Sw(&DynamicValidFileIndex, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "Dynamic Validation File", 0, "Off|On", "Develop <100");


	    pSw= new Sw(&LateralInputFile, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Lateral Input File",0,"By given name|From previous RunNo","LateralInput =1");
        pSw= new Sw(&NoSingleRunSimFile, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "No SingleRunSimFile",1,"off|on");
        
        pSw= new Sw(&TimeSerieOutputLevels, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Time Serie Outputs",1,"Allways PG time serie file|No PG File for MultiRun|No PG File and no statistics");
        pSw= new Sw(&ValidationOutputLevels, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Validation Outputs",1,"Allway PG file|only statistics", "ValidationMode>0");
        pSw= new Sw(&BayesianStepFunction, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Marcov Chain Step",1,"Constant|Exponential decrease|Linear decrease|Cycling","ValidationMode > 0");
        pSw= new Sw(&BayesianLikelihoodFunction, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Likelihood Function",0,"Gaussian|Sivias","ValidationMode > 0");
        pSw= new Sw(&BoxCoxTransformed, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Box-Cox Tranformation",0,"No|Yes","ValidationMode > 0");
    
        pSw= new Sw(&HeatFluxUnit, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw,   "Heat Flux Unit",0,"J/day|Watt");

		pSw = new Sw(&DataBaseCSV, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "CSV File for Smart Change of Parameter Values", 0, "No|MASCUR project", "CalibrationMode>0");
  
		pSw = new Sw(&UserDefinedCalibrationMode, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "CalibrationMode", 0, "off|one|two|three|four|five|six|seven|eight|nine|ten|eleven|tvelve|firteen|fourteen|fithteen|sixteen");

		pSw = new Sw(&Mascur_Soil_Data, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "Mascur soil info file", 0, "No|MASCUR project", "IGBP LandInfo>0");
		
		pSw = new Sw(&SimulationPeriodFromMetFile, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "Apply Simulation Period from Sim File", 0, "No|Yes", "TypeOfDrivingFile >= 1");

		pSw = new Sw(&Nelder_Mead_Objective, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "Nelder-Mead Objective", 0, "SSE|AIC|BIC", "ValidationMode > 0");

          	    
  //!	CALL F(Group_01, FileDriv7a,"Lateral Water Inflow","GWInflow","LateralInputFile=0|WaterEq > 0|LaterInput=1")
  Ps *pPs;
  pPs= new Ps(&Val_Index, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		
	Define(pPs,  		"Index in val file",1.,"#","IndexForValidation>0",1.,100.);
	  pPs= new Ps(&Val_IndexMax, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs,  		"Size of internal loop",1.,"#","IndexForValidation>0",1.,100.);
	  pPs= new Ps(&ValFile_Index, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs,  	"Index for selection of File (first val file)",1.,"#","Dynamic Validation File=1",1.,100.);
	pPs = new Ps(&Val_IndexRepeat, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "No of internal index loop", 1., "#", "IndexForValidation>0", 1., 100.);
	  pPs= new Ps(&FRandomSeed, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs,  	"Random Seed",1.,"#","Develop<100",1.,100000.);
	  pPs= new Ps(&BayesianStepChangeSens, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs,   "BayesianChangeCoef", 0.003,"-","Marcov Chain Step=1",0., 0.1);
	  pPs= new Ps(&BayesianStepMinimumScaling, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs,   "BayesianMinScaling", 0.001,"-","Marcov Chain Step>0",0.1E-10, 1.);
	  pPs= new Ps(&MCStepChangePeriod, SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs,   "MC StepChangePeriod", 1000.,"-","Marcov Chain Step>1",100., 1.E6);
		pPs = new Ps(&AggregateCalibNumber, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Aggregate Calib Number", 1., "-", "Develop<100", 1., 100.);
    
	Tab *pTab;
	NE *pScaling;
	pScaling= new NE(&NumScaling, 10,1,100,2,100);
	Define(pScaling,"No Scaling Observed");
	vector<double> initv;
	initv.resize(NumScaling);
	initv.assign(NumScaling,0.);
//	call PnE(,10,1,100,2,100)	
	P* pP;
	 pP= new P(&ObservedScaling ,pScaling, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pP,  pScaling, "Scaling Observed", initv,"-","ValidationMode>1",-0.5, 0.5);
		ScalingObsTab.push_back(pP);
	pP= new P(&ObservedScalingIndex ,pScaling, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pP,  pScaling,  "Scaling Index", initv,"-","ValidationMode>1",0., 1000.);
		ScalingObsTab.push_back(pP);

	
	pP= new P(&BoxCox_p1, pScaling, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		initv.assign(NumScaling,0.);
		Define(pP,  pScaling,   "Box Cox power", initv,"-","Box-Cox Tranformation>1",0., 1.);
		ScalingObsTab.push_back(pP);
	pP= new P(&BoxCox_p2 ,pScaling, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pP,  pScaling, "Box Cox offset", initv,"-","Box-Cox Tranformation>1",0., 1.E10);
		initv.assign(NumScaling,.10);
		ScalingObsTab.push_back(pP);

	pTab= new Tab(&ScalingObsTab, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pTab,"Scaling of Observed Variables","Scaling Observed|Scaling Index|Box Cox power|Box Cox offset","");

	size_t nss=pTab->GetNumOfRow();
	pP= new P(&Abs_Error ,pScaling, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
      Define(pP,  pScaling,  "Abs Error", initv,"-","Develop<100",0., 100.);
		initv.assign(NumScaling,.15);
		ErrorTab.push_back(pP);
	pP= new P(&Rel_Error ,pScaling, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pP,  pScaling,   "Rel Error", initv,"-","Develop<100",0., 1.);
		ErrorTab.push_back(pP);

	pTab= new Tab(&ErrorTab, OBSERVED,  GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pTab,"Errors for Calibration","Abs Error|Rel Error","");



	NE *pIndexNum;
	pIndexNum = new NE(&NumberParGroups, 0, 0, 10, 0, 5);
	Define(pIndexNum, "Number of User Defined MultiPar");


	initv.assign(NumberParGroups, 1);
	pP = new P(&IndexParGroups,pIndexNum,  FILEDEFINEDPARAMETERS, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pP, pIndexNum, "Key Index", initv, "", "Develop<100", 0., 100000.,0.,10000.,7);
		IndexMultiParGroups.push_back(pP);
		
	pTab = new Tab(&IndexMultiParGroups, FILEDEFINEDPARAMETERS, GENERAL, NOFPROC, NOBPROC, NORMAL);
		Define(pTab, "Index in User Defined Parameter Files", "Key Index","");


	F* pF;
	pF = new F(&FileValidation[0], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
  	Define(pF,"Validation File1","Valid1.bin","ValidationMode>0");
	pF = new F(&FileValidation[1], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File2","Valid2.bin","ValidationMode>1");
	pF = new F(&FileValidation[2], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File3","Valid3.bin","ValidationMode>2");
	pF = new F(&FileValidation[3], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File4","Valid4.bin","ValidationMode>3");
	pF = new F(&FileValidation[4], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File5","Valid5.bin","ValidationMode>4");
	pF = new F(&FileValidation[5], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File6","Valid6.bin","ValidationMode>5");
	pF = new F(&FileValidation[6], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File7","Valid7.bin","ValidationMode>6");
	pF = new F(&FileValidation[7], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File8","Valid8.bin","ValidationMode>7");			
	pF = new F(&FileValidation[8], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File9","Valid9.bin","ValidationMode>8");
	pF = new F(&FileValidation[9], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File10","Valid10.bin","ValidationMode>9");
	pF = new F(&FileValidation[10], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File11","Valid11.bin","ValidationMode>10");
	pF = new F(&FileValidation[11], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File12","Valid12.bin","ValidationMode>11");
	pF = new F(&FileValidation[12], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File13","Valid13.bin","ValidationMode>12");
	pF = new F(&FileValidation[13], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File14","Valid14.bin","ValidationMode>13");
	pF = new F(&FileValidation[14], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File15","Valid15.bin","ValidationMode>14");
	pF = new F(&FileValidation[15], ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 	Define(pF,"Validation File16","Valid16.bin","ValidationMode>15");
    
	pF = new F(&OutputFile, ATMBOUNDARY,  GENERAL, NOFPROC, NOBPROC, NORMAL);                            	
 	Define(pF, "Output File","COUP_XXXXX.bin");

	FCSV *pCSV;
	pCSV = new FCSV(&m_DataBaseCSV, OTHER, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pCSV, "MASCUR File", "SOIL_WP1_Beckuma.csv", "Mascur soil info file>0", 2);

	pCSV = new FCSV(&m_MultiParStream[0], ATMBOUNDARY, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pCSV, "ParCalibrationFile1", "ParameterCalFile1.csv", "CalibrationMode>0",7);
	pCSV = new FCSV(&m_MultiParStream[1], ATMBOUNDARY, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pCSV, "ParCalibrationFile2", "ParameterCalFile2.csv", "CalibrationMode>1",7);
	pCSV = new FCSV(&m_MultiParStream[2], ATMBOUNDARY, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pCSV, "ParCalibrationFile3", "ParameterCalFile3.csv", "CalibrationMode>2", 7);
	pCSV = new FCSV(&m_MultiParStream[3], ATMBOUNDARY, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pCSV, "ParCalibrationFile4", "ParameterCalFile4.csv", "CalibrationMode>3");
	pCSV = new FCSV(&m_MultiParStream[4], ATMBOUNDARY, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pCSV, "ParCalibrationFile5", "ParameterCalFile5.csv", "CalibrationMode>4");



return true;
}

double Technical_Functions::SumLikelihood(size_t index, double sim, double obs) {
	double E_Meas, Meas_Corr, Sigma_Error;
	double out;

	if(BayesianLikelihoodFunction==0) {
		if(Rel_Error[index]*abs(obs)<Abs_Error[index])	E_Meas=Abs_Error[index];
		else if(abs(obs)>0)	E_Meas=max(Rel_Error[index], 0.15)*abs(obs);
		else if(abs(sim-obs)>0&&abs(sim)*Rel_Error[index]>Abs_Error[index])		E_Meas=Rel_Error[index]*abs(sim);
		else E_Meas=Abs_Error[index];




		if(ObservedScaling[size_t(ObservedScalingIndex[index])]!=0)
			Meas_Corr=obs*pow(10., ObservedScaling[size_t(ObservedScalingIndex[index])]);
		else
			Meas_Corr=obs;
		if(BoxCoxTransformed==1) {
			if(Abs_Error[index]>0)
				Sigma_Error=Abs_Error[index];
			else
				Sigma_Error=Rel_Error[index]*abs(Meas_Corr);
			//Prev=
			Meas_Corr=max(1.E-6, Meas_Corr);
			out=-0.5*pow((boxcox(sim, BoxCox_p1[index], BoxCox_p2[index])-
				boxcox(Meas_Corr, BoxCox_p1[index], BoxCox_p2[index]))/Sigma_Error,2)
				-Fix::LOG_PI-log(Sigma_Error)+ 
				log(abs(boxcox_derive(Meas_Corr, BoxCox_p1[index], BoxCox_p2[index])));
		}
		else {
			Sigma_Error=E_Meas;
			out=-0.5*pow((sim-Meas_Corr)/Sigma_Error,2)-Fix::LOG_PI-log(Sigma_Error);
		}
	}
	else {
		if(abs(obs)*Rel_Error[index]<Abs_Error[index]&&Abs_Error[index]>0)	E_Meas=Abs_Error[index];
		else if(abs(obs)>0)			E_Meas=Rel_Error[index]*abs(obs);
		else if(abs(sim-obs)>0)		E_Meas=Rel_Error[index]*abs(sim);
		else						E_Meas=1;

		if(ObservedScaling[size_t(ObservedScalingIndex[index])]!=0)
			Meas_Corr=obs*pow(10., ObservedScaling[size_t(ObservedScalingIndex[index])]);
		else
			Meas_Corr=obs;
		double ri=(sim-Meas_Corr)/E_Meas;
		if(abs(ri)<1.E-10)
			out=-Fix::LOG_PI-log(2*E_Meas);
		else
			out=log(1-exp(-0.5*pow(ri,2)))-log(pow(ri,2))-Fix::LOG_PI-log(E_Meas);

	}

	return out;
}
double Technical_Functions::SumLikelihood(size_t index, vector<double> *psim, vector<double> *pobs) {

	double out=0;

	for(size_t i=0; i<psim->size(); i++)
		out+=SumLikelihood(index, psim->at(i), pobs->at(i));

	return out;
}
double Technical_Functions::MC_ScalingStepFunction(size_t IndexRun) {

	double fIndex=double(IndexRun);
	if(BayesianStepFunction==0)
		return 1.;
	else if(BayesianStepFunction==1)
		return (1-BayesianStepMinimumScaling)*exp(-BayesianStepChangeSens*fIndex)+BayesianStepMinimumScaling;
	else if(BayesianStepFunction==2)
		return max(BayesianStepMinimumScaling, 1-fIndex/MCStepChangePeriod);
	else
		return abs(sin(fIndex/MCStepChangePeriod*Fix::PI+Fix::PI/2))*(1-BayesianStepMinimumScaling)+BayesianStepMinimumScaling;
}
