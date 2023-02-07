#include "Salt.h"
#include "../Atmosphere/Surface_Water.h"
#include "../Atmosphere/Irrigation.h"
#include "../../Simulator/Simulator.h"
						   
Salt::Salt(void)
{
	ModuleName="Salt";
	ModuleNo=22;
	ModuleConditions="SaltTracer >0";
}

bool Salt::Ini()	{
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	Interception::Ini();
	if(IrrigConcInput==1) {
		string filename;
		filename=m_PG_Salt.GetFileName();
		m_PG_Salt.Open(filename);
	    //Run_res_id=INECO(FileDriv4, NUnitDriv4)

		/*if(Run_res_id>=0) {
			if(SaltRoadStorage==1) {
				CALL INIECO(2, 1, 1, NUnitdriv4, ISALT) ! Discrete consideration of saltInput
			else
				CALL INIECO(1, 1, 1, NUnitdriv4, ISALT)
			}
		else
			Return
		}
		if(!allocated(VUTPG_FILE)) allocate(VUTPG_FILE(IVARMAX(ISALT)))*/
	}
	if(RoadSaltApplication==1) {
		string filename;
		filename=m_PG_RoadSurface.GetFileName();
		m_PG_RoadSurface.Open(filename);
	    /*Run_res_id=INECO(FileDriv11, NUnitDriv11)

		if(Run_res_id>=0) {
			CALL INIECO(2, 1, 1, NUnitdriv11, ISALTDEP) ! Discrete consideration of saltInput
		else
			Return
		}
		if(!allocated(VUTPG_FILE)) allocate(VUTPG_FILE(IVARMAX(ISALTDEP)))*/
	}

	else if(SaltDeposition>=1) {
		string filename;
		filename=m_PG_RoadSurface.GetFileName();
	    if(SaltDeposition==2) {
	
	        size_t LL=filename.rfind("_");
	        /*Do ICount=LL,1,-1
	            if(FileDriv11(ICount:ICount)=='_') {
	               LL=ICount
	               exit 
	            }
            EndDo
            	           
            WRITE(FileDriv11(LL+1:LL+5),'(I5.5)') INT(Dep_IndexStart+Dep_IndexInc)  
	    }*/
	    //Run_res_id=INECO(FileDriv11, NUnitDriv11)

	
			m_pModelInfo->Run_Res_Id=CheckFromRoadFile();
	
		}
	
	}


//	!C*************** Initial Amount of Salt in soil layers ****************
//!
		if(!m_pModelInfo->StartStateValue) 
			AccSaltInput=AccSaltOutput=SaltOnRoad=SaltCloseToRoad=SaltInSnow=0.;
		
		SaltInfiltration=SaltFromSnow=SaltToSnow=0.;

		if(m_pModelStructure->SaltTracer>=1&&!m_pModelInfo->StartStateValue) {

		//	p_Soil_WaterF->Ini();

			SaltOnRoad=0.;
			SaltCloseToRoad=0.;
		    for(size_t i=0; i<SaltStorage.size();i++) {
				if(InitialSaltConc==0) 
					SaltStorage[i]=p_Soil_WaterF->WaterStorage[i]*SaltInitConc;
				else
					SaltStorage[i]=p_Soil_WaterF->WaterStorage[i]*SaltInitConcDist[i];
				
			}
		}

		
	 if(TraceElementUptake) {
		if(ActiveUptakeLayers.size()==0) ActiveUptakeLayers.resize(p_Soil_WaterF->NL);
		TE_UptActiveReset=true;
		TE_UptPassiveReset=true;
		TE_UptReset=true;

		if(!m_pModelInfo->StartStateValue) {
			 TE_Grain.assign(NumPlants,0.);
			 TE_Stem.assign(NumPlants,0.);
			 TE_Roots.assign(NumPlants,0.);
			 TE_Leaf.assign(NumPlants,0.);
			 TE_OldStem.assign(NumPlants,0.);
			 TE_OldRoots.assign(NumPlants,0.);
			 TE_OldLeaf.assign(NumPlants,0.);
			 TE_Stem.assign(NumPlants,0.);
			 TE_Roots.assign(NumPlants,0.);
			 TE_SurfaceLitter=0;
			 TE_Litter1.assign(p_Soil_WaterF->NL,0.);
			 TE_Humus.assign(p_Soil_WaterF->NL,0.);
		}

		TE_TotPlant=sum(TE_Grain)+sum(TE_Stem)+sum(TE_Roots)+sum(TE_Leaf)+sum(TE_OldStem)+sum(TE_OldRoots)+sum(TE_OldLeaf);
		TE_TotHumus=sum(TE_Humus);
		TE_TotLitter=sum(TE_Litter1)+TE_SurfaceLitter;
		TE_TotMineral=sum(SaltStorage);

//!		sum(SaltStorage(:)/WaterStorage(:))
//!		TE_TotMinDiss=sum(SaltStorage)
//!		TE_TotMinAds=sum(SaltStorage)

		TE_TotStorage=TE_TotPlant+TE_TotHumus+TE_TotLitter+TE_TotMineral;
		TE_InitStorage=TE_TotPlant+TE_TotHumus+TE_TotLitter+TE_TotMineral;




	 }

	return true;
}
bool Salt::IniFunctors() {

	Interception::IniFunctors();

	return true;
}
void Salt::Deposition() {

	Irrigation *pMet=(Irrigation*)p_Atm;
    double SnowConcSalt;
 
 	if(m_pModelStructure->Irrigation>0) {

 		if(IrrigConcInput==1) {
			/*if(pModelStructure->TimeResolution>=1) 
				DCH=0.5;
		   else
				  DCH=0.;
			Get    
			Call READECO(T_Current+DCH, ISALT, int(ConcV_Index), VUTPG_FILE) */
			//SaltIrrigationConc=GetEcoDat(&m_PGPos_Salt, )
		}
		else
		    SaltIrrigationConc=SaltIrrigationConcPar;
		
		if(pMet->Dripper==1) 
		    SaltInfiltration=max(0.,pMet->SoilInfil-pMet->IrrigationRate)*SaltInputConcPar;
		else
		    SaltInfiltration=max(0.,pMet->SoilInfil-pMet->IrrigationRate)*SaltInputConcPar+pMet->IrrigationRate*SaltIrrigationConc;
	}
	else {
		SaltInfiltration=pMet->SoilInfil*SaltInputConcPar;
		if(RoadSaltApplication==1) {
 			//Call READECO(T_Current+DCH, ISALTDEP, 1, VUTPG_FILE) 
		//	SaltToRoad=VUTPG_File.front()/TTSTEP_SNOW
//! Emission of Salt from Road
			SaltAirEmission		=(1-CaptureFracEmission)*SaltAirEmissionFunc();
			SaltPloughEmission	=(1-CaptureFracEmission)*SaltPloughEmissionFunc();
			SaltRoadCaptured=CaptureFracEmission*SaltAirEmissionFunc()+CaptureFracEmission*SaltPloughEmissionFunc();
			SaltRunOffEmission	=SaltRunOffEmissionFunc();
			SaltRoadLossed=SaltRunOffCloseToRoadFunc();
			SaltRunOffEmission2=SaltRunOffEmission+SaltRoadLossed;
//		
			SaltEmissions=SaltAirEmission+SaltPloughEmission+SaltRunOffEmission;

//! Infiltration of emissions into soil
	//		! From the same run
		//	!SaltInfiltration=SaltInfiltration+SaltEmissions*FractionOfRoad Removed 29 may 2007
		}
		else if (SaltDeposition>=1) {
 		//	Call READECO(T_Current+DCH, ISALTDEP, IVARMAX(ISALTDEP), VUTPG_FILE) 
		//	SaltAirDeposition=SaltAirDepositionFunc(VUTPG_FILE(AIRDEPPOS))
			//SaltPloughDeposition=SaltPloughDepositionFunc(VUTPG_FILE(PloughDEPPOS))
		//	SaltRunOffDeposition=SaltRunOffDepositionFunc(VUTPG_FILE(RunOffDEPPOS))

			SaltInfiltration=SaltInfiltration+ SaltAirDeposition+SaltPloughDeposition+SaltRunOffDeposition;
		}
		else if(RoadSaltApplication==0) {
			if(pMet->TAir>TempSaltLowLimit&&pMet->TAir<TempSaltHighLimit) 
				SaltInfiltration=SaltInfiltration+SaltAppRate;
		}
		
		if(SaltSnowStorage>0) {
		    if(pMet->WTotSnow>0) {
		            SaltToSnow=SaltInfiltration;
		            SaltInfiltration=0.;
			}
		    else {
		            SaltToSnow=0.;
		            SaltFromSnow=SaltInSnow/T_Step;
		    }
		    
		    if(pMet->SnowOutFlow>0) {
		            if(pMet->SWATS>0.1) {
		                SnowConcSalt=SaltInSnow/pMet->SWATS;
		                SaltFromSnow=min(SaltInSnow/T_Step,pMet->SnowOutFlow*SnowConcSalt);
					}
		            else
		                SaltFromSnow=0.;
		    }
		    else if(pMet->WTotSnow>0) 
		        SaltFromSnow=0.;
	 
		    
		    SaltInfiltration=SaltInfiltration+SaltFromSnow;
			
		}
	}

	if(pMet->SoilInfil>0) 
		SaltInfilConc=SaltInfiltration/pMet->SoilInfil;
	

	if(RoadSaltApplication==1) {
		if(pMet->TAir>TempSaltLowLimit&&pMet->TAir<TempSaltHighLimit) 
			SaltInfiltration=SaltInfiltration+SaltAppRate;
		
	}


}
bool Salt::Dyn()
{
	Interception::Dyn();
	Flux();
	Integ();



	if(SaltGWSource==1) 
		SaltSourceFlow=GetEcoDat(&((Surface_Water*)p_Atm)->m_PGPOS_Source,SaltSourceFlow.size(), ((Surface_Water*)p_Atm)->SaltDrainIpos);

	return true;
}

void Salt::Flux()
{
	
	double MatricFlow;
	Deposition();
	for(size_t i=0; i<SaltConc.size(); i++) {
		SaltConc[i]=SaltSolutionFunctionL(i);
		OsmoticPressure[i]=-OsmoticPressureFuncL(i);
	}

	if(ResistivityOutput) {
	    
		for(size_t i=0; i<SaltConc.size(); i++) {
	        WaterResistivity[i]=WaterResistivityL(i);
	        SoilResistivity[i]=ArchieL(i);
		}
	}


   
	for(size_t i=0; i<SaltDrainFlow.size()-1; i++) {
        if(m_pModelStructure->GroundWaterFlow>=1&&p_Soil_WaterF->Theta[i]>10.)
			SaltDrainFlow[i]=SaltConc[i]*p_Soil_WaterF->WaterDrainFlow[i];
		else
			SaltDrainFlow[i]=0.;
		if(int(p_Soil_WaterF->GWSoLayer+0.1)==i&&int(p_Soil_WaterF->GWSoFlow+0.1)>0) {
			 SaltDrainFlow[i]=SaltDrainFlow[i]+SaltConc[i]*p_Soil_WaterF->GWSoFlow;
			 SaltDrainFlow[i]=SaltDrainFlow[i]-p_Soil_WaterF->GWSoFlow*SaltGWSoFlowConc;			
		}		
		
/*!		WBypassflow[i]=0.
!        MatricFlow=WaterFlow[i]-WBYPASSflow[i]
!        if(MatricFlow>=0.) {
!           SALTFLow[i]=SaltConc[i]*MatricFlow+WBypassFlow[i]*SaltInfilConc
!        else if(i<=NL-1) {
!           SALTFLow[i]=SaltConc(i+1)*MatricFlow*/

        MatricFlow=p_Soil_WaterF->WaterFlow[i]-p_Soil_WaterF->WBypassFlow[i];
		if(MatricFlow>0) {	
			SaltFlow[i]=MatricFlow*SaltConc[i]+p_Soil_WaterF->WBypassFlow[i]*SaltInfilConc;
			if(SaltFlow[i]*T_Step>0.1*abs(SaltStorage[i])) {
				SaltFlow[i]=0.;
			}
		}
		else if(p_Soil_WaterF->WBypassFlow[i]>0) {
			SaltFlow[i]=p_Soil_WaterF->WBypassFlow[i]*SaltInfilConc;
		}
    	else {
			SaltFlow[i]=MatricFlow*SaltConc[i+1];
			if(SaltFlow[i]*T_Step<-0.1*abs(SaltStorage[i+1])) {
				SaltFlow[i]=0.;

			}
		 }
	}

    if(m_pModelStructure->GroundWaterFlow>=1) 
        SaltDrainFlow.back()=SaltConc.back()*p_Soil_WaterF->WaterDrainFlow.back();
    
    
	TotalSaltDrainFlow=sum(SaltDrainFlow);

	SaltDeepPercolation=SaltConc.back()*p_Soil_WaterF->DeepPerc;

	SaltSurfaceOutFlow=p_Soil_WaterF->SurfaceOutFlow*SaltConc.front();

	DepthofFront=FrontLowerBoundary();


}
void Salt::Integ()
{
	Irrigation *pMet=(Irrigation*)p_Atm;
	InFlow(SaltStorage.front(), SaltInfiltration);
	OutFlow(SaltStorage.front(), SaltSurfaceOutFlow);

	IntFlow(SaltStorage, SaltFlow);

	if(pMet->Dripper==1) {
		 InFlow(SaltStorage, SaltSourceFlow);
		 TotSaltSourceFlow=sum(SaltSourceFlow);
		 InFlow(AccSaltInput, TotSaltSourceFlow);
	}
	else if(SaltGWSource>=1) { //! The same function as Drip Irrigation can not be combined
		 InFlow(SaltStorage, SaltSourceFlow);
		 TotSaltSourceFlow=sum(SaltSourceFlow);
		 InFlow(AccSaltInput, TotSaltSourceFlow);
	}
	if(SaltRoadStorage) {
		InFlow(SaltOnRoad, SaltToRoad);
		OutFlow(SaltOnRoad, SaltEmissions);
		InFlow(SaltCloseToRoad, SaltRoadCaptured);
		OutFlow(SaltCloseToRoad, SaltRoadLossed);
	}
	
	if(SaltSnowStorage) {
	    InFlow(SaltInSnow, SaltToSnow);
	    OutFlow(SaltInSnow, SaltFromSnow);
	}


	if(m_pModelStructure->GroundWaterFlow>0) {
		OutFlow(SaltStorage, SaltDrainFlow);
		for(size_t i=0; i<SaltDrainFlow.size();i++) InFlow(AccSaltOutput, SaltDrainFlow[i]);
		
	}	//! Why is SaltDeepPercolation omitted if GroundWaterFlow>0 ???
	else {
		OutFlow(SaltStorage.back(), SaltDeepPercolation);
		InFlow(AccSaltOutput,SaltDeepPercolation);
	}

	InFlow(AccSaltInput, SaltInfiltration);

	TE_TotMineral=sum(SaltStorage);

	if(TraceElementUptake>0) {
		OutFlow(SaltStorage, SaltPlantUptake);
 		TE_TotPlant=sum(TE_Grain)+sum(TE_Stem)+sum(TE_Roots)+sum(TE_Leaf)+sum(TE_OldStem)+sum(TE_OldRoots)+sum(TE_OldLeaf);
		TE_TotHumus=sum(TE_Humus);
		TE_TotLitter=sum(TE_Litter1)+TE_SurfaceLitter;
		TE_TotMineral=sum(SaltStorage);
		TE_TotStorage=TE_TotPlant+TE_TotHumus+TE_TotLitter+TE_TotMineral;

		TE_TotLitterFall=sum(TE_RootsLitter)+sum(TE_LeafSurfaceLitter)+sum(TE_StemSurfaceLitter)+sum(TE_GrainSurfaceLitter)+
						  sum(TE_OldRootsLitter)+sum(TE_OldStemSurfaceLitter)+sum(TE_OldLeafSurfaceLitter);
		TE_TotPlantUptake=sum(TE_PlantLeafUptake)+sum(TE_PlantStemUptake)+sum(TE_PlantRootUptake);
		TE_TotMineralisation=sum(TE_Litter1MinRate)+sum(TE_HumusMinRate);	

		 TE_Balance=AccSaltInput+TE_InitStorage-AccSaltOutput-TE_TotPlant-TE_TotHumus-TE_TotLitter-TE_TotMineral;
		 TE_UptActiveReset=true;
		 TE_UptPassiveReset=true;
		 TE_UptReset=true;
	}

}

bool Salt::End()
{
return true;
}
Salt::~Salt(void)
{
}
bool Salt::Def()
{
	Interception::Def();
	ModuleNo = 22;
	ModuleConditions = "SaltTracer >0";
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;

	Func *pFunc;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;




	Sw* pSw;
	pSw = new Sw(&RoadSaltApplication, ROADSURFACE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pSw, "RoadSaltApplication", 0, "Parameter Driven|On Road- PG File", "SaltRoadStorage>0", 116);
	pSw = new Sw(&SaltRoadStorage, ROADSURFACE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pSw, "SaltRoadStorage", 0, "off|on", "");
	pSw = new Sw(&SaltSnowStorage, ROADSURFACE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pSw, "SaltSnowStorage", 0, "off|on", "SnowPack >0");

	pSw = new Sw(&SaltDeposition, ROADSURFACE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pSw, "SaltDeposition", 0, "Parameter Driven|From Road- PG File|From PG File(par)", "SaltRoadStorage=0", 116);//	 !fixa common true Salt road pg file
	pSw = new Sw(&BlomquistEquation, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pSw, "Blomquist Equation", 0, "off|on", "|SaltRoadStorage=0");
	pSw = new Sw(&TraceElementUptake, PLANT, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw, "TraceElementUptake", 0, "off|on", "");
	pSw = new Sw(&PassiveUptake, PLANT, SALT, TRANSPORT, NOBPROC, NORMAL);

	Define(pSw, "PassiveUptake", 0, "off|on", "TraceElementUptake >0");
	//! To be handled in Wateruptake module
	pSw = new Sw(&ActiveUptake, PLANT, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw, "ActiveUptake", 0, "off|on", "TraceElementUptake >0");
	//! To be handled in NC_Plant module
	pSw = new Sw(&InitialSaltConc, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	Define(pSw, "Initial Salt Concentration", 0, "Uniform conc|conc(z)", "");
	pSw = new Sw(&Adsorption, SIMPROFILE, SALT, ADSORPTION, NOBPROC, NORMAL);
	Define(pSw, "Adsorption", 0, "off|on", "");
	pSw = new Sw(&IrrigConcInput, SINGLE, SALT, INFILTRATION, NOBPROC, NORMAL);
	Define(pSw, "IrrigConcInput", 0, "Parameter|PG-file", "|Irrigation>0");

	pSw = new Sw(&SaltGWSource, SINGLE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pSw, "Salt GW Source", 0, "off|Lateral Input File", "|GroundWaterFlow >=1", 114);
	pSw = new Sw(&ResistivityOutput, SIMPROFILE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "Resisitivy Output", 0, "off|on", "");

	Xs *pXs = new Xs(&AccSaltInput, SINGLE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pXs, "AccSaltInput", 0.,UNIT_TYPES::mMASS_UNIT, "");
	pXs = new Xs(&AccSaltOutput, SINGLE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pXs, "AccSaltOutput", 0.,UNIT_TYPES::mMASS_UNIT, "");
	pXs = new Xs(&TE_Balance, SINGLE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pXs, "TE_Balance", 0.,UNIT_TYPES::mMASS_UNIT, "");
	pXs = new Xs(&SaltOnRoad, ROADSURFACE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pXs, "SaltOnRoad", 0.,UNIT_TYPES::mMASS_UNIT, "SaltRoadStorage >0");
	pXs = new Xs(&SaltCloseToRoad, ROADSURFACE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pXs, "SaltCloseToRoad", 0.,UNIT_TYPES::mMASS_UNIT, "SaltRoadStorage >0");
	pXs = new Xs(&SaltInSnow, ROADSURFACE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pXs, "Salt in SnowPack", 0.,UNIT_TYPES::mMASS_UNIT, "SaltSnowStorage >0");
	X* pX;
	NE *pNL; pNL = m_pModelMap->GetNEPointer("Number of Layers");
	vector<double> initv;
	initv.assign(p_Soil_WaterF->NL, 0.);
	pX = new X(&SaltStorage, pNL, SIMPROFILE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNL, "SaltStorage", initv,UNIT_TYPES::mMASS_UNIT, "");
	T* pT = new T(&SaltFlow, pNL, SIMPROFILE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pT, pNL, "SaltFlow", initv,UNIT_TYPES::mMASSFLOW_UNIT, "");
	pT = new T(&SaltDrainFlow, pNL, SIMPROFILE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pT, pNL, "SaltDrainFlow", initv,UNIT_TYPES::mMASSFLOW_UNIT, "");
	pT = new T(&SaltSourceFlow, pNL, SIMPROFILE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pT, pNL, "SaltSourceFlow", initv,UNIT_TYPES::mMASSFLOW_UNIT, "EnableSaltSource =1");

	Ts *pTs = new Ts(&TotSaltSourceFlow, SINGLE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pTs, "TotSaltSourceFlow", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "EnableSaltSource =1");
	pT = new T(&SaltPlantUptake, pNL, SIMPROFILE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pT, pNL, "SaltPlantUptake", initv,UNIT_TYPES::mMASSFLOW_UNIT, "TraceElementUptake > 0");

	pTs = new Ts(&SaltToSnow, SINGLE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pTs, "Salt To SnowPack", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltSnowStorage>0");
	pTs = new Ts(&SaltFromSnow, SINGLE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pTs, "Salt From SnowPack", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltSnowStorage>0");

	pTs = new Ts(&SaltInfiltration, SINGLE, SALT, INFILTRATION, NOBPROC, NORMAL);
	Define(pTs, "SaltInfiltration", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "");

	pTs = new Ts(&SaltEmissions, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pTs, "SaltEmissions", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");
	//! New Emission variables created August 2005
	pTs = new Ts(&SaltAirEmission, ROADSURFACE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pTs, "Salt AirEmission", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");
	pTs = new Ts(&SaltPloughEmission, ROADSURFACE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pTs, "Salt PloughEmission", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");
	pTs = new Ts(&SaltRunOffEmission, ROADSURFACE, SALT, MASSBALANCE, NOBPROC, NORMAL);
	Define(pTs, "Salt RunoffEmission", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");


	//! New Outputs Resisvity
	//call PnE(NRESPROBE,1,1,10,1,10)	

	Ps *pPs = new Ps(&Archie_A, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Archie A", 1., "-", "Resisitivy Output>0", 0.5, 2.5);
	pPs = new Ps(&Archie_M, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Archie M", 1.5, "-", "Resisitivy Output>0", 1.3, 2.5);
	pPs = new Ps(&Archie_N, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Archie N", 2., "-", "Resisitivy Output>0", 1., 3.);


	pPs = new Ps(&Res_Temp_N, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Resisitivy NormT", 25.,UNIT_TYPES::TEMP_UNIT, "Resisitivy Output>0", 15., 30.);
	pPs = new Ps(&Res_Temp_Sens, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Resisitivy SensT", 0.019, "", "Resisitivy Output>0", 0.00, 0.020);
	pPs = new Ps(&Res_IonCoef, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Resisitivy IonCoef", 35.5, "g/mol", "Resisitivy Output>0", 20., 100.);


	pPs = new Ps(&Dep_IndexStart, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Dep Index Start", 0., "-", "SaltDeposition=2", 0., 100000.);
	pPs = new Ps(&Dep_IndexInc, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Dep Index Add", 1., "-", "SaltDeposition=2", 0., 100000.);

	pT = new T(&WaterResistivity, pNL, SIMPROFILE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pT, pNL, "Water Resistivity", initv, "Ohm m", "");
	pT = new T(&SoilResistivity, pNL, SIMPROFILE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pT, pNL, "Soil Resistivity", initv, "Ohm m", "");


	pTs = new Ts(&SaltToRoad, ROADSURFACE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pTs, "SaltToRoad", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");
	pTs = new Ts(&SaltDeepPercolation, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pTs, "SaltDeepPercolation", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "");
	pTs = new Ts(&SaltSurfaceOutFlow, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pTs, "SaltSurfaceOutflow", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "");

	pTs = new Ts(&SaltRunOffEmission2, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pTs, "Salt RunoffTotal", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");
	pTs = new Ts(&SaltRoadCaptured, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pTs, "Salt RoadCaptured", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");
	pTs = new Ts(&SaltRoadLossed, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pTs, "Salt RoadLossed", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltRoadStorage >0");


	Gs *pGs = new Gs(&DepthofFront, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pGs, "Depth of Front", 0.0, "m", "");
	pPs = new Ps(&ConcForFront, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "ConcForFront", 2., "mg/l", "", 1.E-20, 1.E3);
	pPs = new Ps(&SaltInitConc, SINGLE, SALT, INITF, NOBPROC, NORMAL);
	Define(pPs, "SaltInitConc", 2., "mg/l", "Initial Salt Concentration =0", 1.E-20, 1.E5);
	pPs = new Ps(&SaltInputConcPar, SINGLE, SALT, INITF, NOBPROC, NORMAL);
	Define(pPs, "SaltInputConc", 1., "mg/l", "SaltTracer = 1", 1.E-6, 1.E6);
	pPs = new Ps(&SaltIrrigationConcPar, SINGLE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pPs, "SaltIrrigationConc", 1., "mg/l", "SaltTracer = 1|Irrigation > 0|IrrigConcInput < 1", 1.E-20, 1.E6);

	pPs = new Ps(&SaltGWSoFlowConc, SINGLE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pPs, "SaltGW Source Conc", 1., "mg/l", "SaltTracer = 1|Irrigation > 0|IrrigConcInput < 1|GroundWaterFlow >= 1", 1.E-20, 1.E6);
	pPs = new Ps(&ConcV_Index, SINGLE, SALT, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Index in PG-file", 1., "#", "IrrigConcInput >= 1", 1., 100.);


	pPs = new Ps(&SaltAppRate, ROADSURFACE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pPs, "Salt Application Rate", 10000.,UNIT_TYPES::mMASSFLOW_UNIT, "RoadSaltApplication = 0", 0., 10000.);
	pPs = new Ps(&TempSaltLowLimit, ROADSURFACE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pPs, "Temp Salt Low Limit", -6.,UNIT_TYPES::TEMP_UNIT, "RoadSaltApplication = 0", -20., 1.);
	pPs = new Ps(&TempSaltHighLimit, ROADSURFACE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pPs, "Temp Salt High Limit", 2.,UNIT_TYPES::TEMP_UNIT, "RoadSaltApplication = 0", -1., 20.);
	pPs = new Ps(&FractionOfRoad, ROADSURFACE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pPs, "Fraction of Road", 0.01, "-", "SaltRoadStorage >0|RoadSaltApplication >= 1", 0., 1.);
	pPs = new Ps(&EmissionRateCoef, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "EmissonRateCoef", 0.05, "-", "SaltRoadStorage >0", 0., 10.);
	//! New August 2005
	pPs = new Ps(&SaltRunOffEffCoef, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "SaltRunOffEffCoef", 0.5, "-", "SaltRoadStorage >0", 0., 10.);
	pPs = new Ps(&SaltPloughEffCoef, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "SaltPloughEffCoef", 0.5, "-", "SaltRoadStorage >0", 0., 10.);
	pPs = new Ps(&FractAirEmission, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "FractAirEmission", 0.1, "-", "Blomquist Equation=0|SaltRoadStorage =0", 0., 10.);
	pPs = new Ps(&FractPloughEmission, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "FractPloughEmission", 0.1, "-", "Blomquist Equation=0|SaltRoadStorage =0", 0., 10.);

	pPs = new Ps(&FractRunOffEmission, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "FractRunOffEmission", 0.1, "-", "SaltRoadStorage =0", 0., 10.);

	pPs = new Ps(&CaptureFracEmission, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "FractCapturedEmission", 0.0, "-", "SaltRoadStorage >0", 0., 10.);

	pPs = new Ps(&DistKSplash, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Dist k Splash", 0.5, "-", "SaltRoadStorage = 0|Blomquist Equation>0|", 0., 10.);
	pPs = new Ps(&DistKAir1, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Dist k Air1", 0.5, "-", "SaltRoadStorage = 0|Blomquist Equation>0|", 0., 10.);
	pPs = new Ps(&SnowPloughFrac, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Snow Plough Frac", 0.2, "-", "SaltRoadStorage = 0|Blomquist Equation>0", 0., 10.);
	pPs = new Ps(&DistFromRoad, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Dist From Road", 20., "m", "SaltRoadStorage = 0", 0., 1000.);

	pPs = new Ps(&DistKAir2, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Dist k Air2", 0.05, "-", "SaltRoadStorage = 0|Blomquist Equation>0", 0., 10.);
	pPs = new Ps(&FracAir1, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Frac Air1", 0.5, "-", "SaltRoadStorage = 0|Blomquist Equation>0", 0., 10.);
	pPs = new Ps(&RoadWidth, ROADSURFACE, SALT, EMISSION, NOBPROC, NORMAL);
	Define(pPs, "Road Width", 6., "-", "SaltRoadStorage = 0|Blomquist Equation>0", 0., 100.);

	P* pP;
	NE *pNumPlants;
	pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	pP = new P(&SaltInitConcDist, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	initv.assign(p_Soil_WaterF->NL, 2.);
	Define(pP, pNL, "Init Salt Conc", initv, "mg/l", "Initial Salt Concentration > 0", 0., 1.E3);
	mTab_Salt.push_back(pP);
	Tab *pTab;
	pTab = new Tab(&mTab_Salt, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	Define(pTab, "Initial Salt Concentrations", "mUpper Depth|mLower Depth|Init Salt Conc", "Initial Salt Concentration > 0");

	pP = new P(&Ad_c, pNL, SIMPROFILE, SALT, NOFPROC, SORPTION, NORMAL);
	initv.assign(p_Soil_WaterF->NL, 1.);
	Define(pP, pNL, "Ad_c", initv, "-", "SaltTracer>0|Adsorption>0", 0., 1.);
	mTab_Ad.push_back(pP);
	pTab = new Tab(&mTab_Ad, SIMPROFILE, SALT, NOFPROC, SORPTION, NORMAL);
	Define(pTab, "Adsorption Coefficients", "mUpper Depth|mLower Depth|Ad_c", "SaltTracer>0|Adsorption>0");


	G* pG = new G(&SaltConc, pNL, SIMPROFILE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNL, "SaltConc", initv, "mg/l", "SaltTracer >= 1");
	pG = new G(&OsmoticPressure, pNL, SIMPROFILE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNL, "OsmoticPressure", initv, "cm", "SaltTracer >= 1");
	pGs = new Gs(&TotalSaltDrainFlow, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pGs, "TotalSaltDrainFlow", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "");

	Ds *pDs = new Ds(&SaltInfilConc, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pDs, "SaltInfilConc", 0., "mg/l", "SaltTracer >= 1");
	pDs = new Ds(&SaltIrrigationConc, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pDs, "SaltIrrigationConc", 0., "mg/l", "SaltTracer >= 1");
	pDs = new Ds(&SaltAirDeposition, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pDs, "SaltAirDeposition", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltTracer >= 1|SaltRoadStorage=0|SaltDeposition>0");
	pDs = new Ds(&SaltPloughDeposition, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pDs, "SaltPloughDeposition", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltTracer >= 1|SaltRoadStorage=0|SaltDeposition>0");
	pDs = new Ds(&SaltRunOffDeposition, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pDs, "SaltRunOffDeposition", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "SaltTracer >= 1|SaltRoadStorage=0|SaltDeposition>0");


	//! Trace Elements below
	initv.assign(NumPlants, 0.);
	pX = new X(&TE_OldRoots, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "TE OldRoots", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");

	pX = new X(&TE_Grain, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);

	Define(pX, pNumPlants, "TE Grain", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");
	pX = new X(&TE_Stem, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "TE Stem", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");
	pX = new X(&TE_Roots, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "TE Roots", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");
	pX = new X(&TE_Leaf, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "TE Leaf", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");

	pX = new X(&TE_OldStem, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "TE OldStem", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");
	pX = new X(&TE_OldLeaf, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "TE OldLeaf", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");

	pXs = new Xs(&TE_SurfaceLitter, SINGLE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pXs, "TE Surface Litter", 0.,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");

	pX = new X(&TE_Litter1, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	initv.assign(p_Soil_WaterF->NL, 10.);
	Define(pX, pNL, "TE Litter1", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");
	pX = new X(&TE_Humus, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);

	Define(pX, pNL, "TE Humus", initv,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake >0");


	//! Trace elements Transfer
	pT = new T(&TE_PlantLeafUptake, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);

	Define(pT, pNumPlants, "TE PlantLeafUptake", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_PlantStemUptake, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE PlantStemUptake", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_PlantRootUptake, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE PlantRootUptake", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_StemGrain, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE StemGrain", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0|TraceElementUptake>0");
	pT = new T(&TE_LeafGrain, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE LeafGrain", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0|TraceElementUptake>0");
	pT = new T(&TE_RootsGrain, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE RootsGrain", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0|TraceElementUptake>0");
	//! Define(pT,  NGrainHarvest		,"N GrainHarvest" 	,NumPlants,		0.,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	//!	Define(pT,  NStemHarvest		,"N StemHarvest"  	,NumPlants,		0.,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	//!	Define(pT,  NOldStemHarvest	,"N OldStemHarvest"		,initv,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	//!	Define(pT,  NLeafHarvest		,"N LeafHarvest"  	,NumPlants,		0.,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	//!	Define(pT,  NOldLeafHarvest	,"N OldLeafHarvest"  	,initv,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	//!	Define(pT,  NRootsHarvest		,"N RootsHarvest"  	,NumPlants,		0.,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	//!	Define(pT,  NOldRootsHarvest	,"N OldRootsHarvest"  	,initv,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
	pT = new T(&TE_RootsLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE RootsLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_LeafSurfaceLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE LeafSurfaceLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_StemSurfaceLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE StemSurfaceLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_GrainSurfaceLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE GrainSurfaceLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	//!	Call T("Hidden", N_NewMobileSurfaceLitter,"N NewMobilSurfLitter",initv,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0")	!LK 010906
	//!	Call T("Hidden", NMobileSurfaceLitter,"N MobileSurfLitter",initv,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0")	!LK 010906
	//!	Define(pT,  NPlantUptake		,"N SoilNewMobile"	,initv0D0,MassFlowUnit, "Nitrogen and Carbon >= 1|Growth >0")
	pT = new T(&TE_OldRootsLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE OldRootsLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_OldLeafSurfaceLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE OldLeafSurfaceLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_OldStemSurfaceLitter, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE OldStemSurfaceLitter", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_StemOldStem, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE StemOldStem", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_LeafOldLeaf, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE LeafOldLeaf", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pT = new T(&TE_RootsOldRoots, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "TE RootsOldRoots", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");


	pTs = new Ts(&TE_SurfaceLitter_L1, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pTs, "TE SurfaceLitter_L1", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake>0");
	pTs = new Ts(&TE_SurfaceLitter_Hum, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pTs, "TE SurfaceLitter_Humus", 0.,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes >0|TraceElementUptake>0");

	pT = new T(&TE_RootsLitter1, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	initv.assign(p_Soil_WaterF->NL, 0.);
	Define(pT, pNL, "TE RootsLitter1", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake>0");

	pT = new T(&TE_Litter1MinRate, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	Define(pT, pNL, "TE Litter1MinRate", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes = 0|TraceElementUptake>0");
	pT = new T(&TE_HumusMinRate, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	Define(pT, pNL, "TE HumusMinRate", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake>0");
	pT = new T(&TE_Litter1HumusRate, pNL, SIMPROFILE, SALT, INITF, NOBPROC, NORMAL);
	Define(pT, pNL, "TE Litter1HumusRate", initv,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|TraceElementUptake>0");


	pG = new G(&TEC_RatioLeaf, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);

	Define(pG, pNumPlants, "TEC RatioLeaf", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pG = new G(&TEC_RatioStem, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "TEC RatioStem", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pG = new G(&TEC_RatioGrain, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "TEC RatioGrain", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pG = new G(&TEC_RatioRoots, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "TEC RatioRoots", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");

	pG = new G(&TEC_RatioOldLeaf, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "TEC RatioOldLeaf", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pG = new G(&TEC_RatioOldStem, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "TEC RatioOldStem", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pG = new G(&TEC_RatioOldRoots, pNumPlants, PLANT, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "TEC RatioOldRoots", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");

	pGs = new Gs(&TE_TotPlant, SINGLE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pGs, "TE Total Plant", 0.0,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");

	pGs = new Gs(&TE_TotHumus, SINGLE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pGs, "TE Total Humus", 0.0,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pGs = new Gs(&TE_TotLitter, SINGLE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pGs, "TE Total Litter", 0.0,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pGs = new Gs(&TE_TotMineral, SINGLE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pGs, "TE Total Mineral", 0.0,UNIT_TYPES::mMASS_UNIT, "");
	//!	Define(pGs,  TE_TotMinDiss,"TE Total Mineral Dissolved", 0.0,mMASS_UNIT, "");
	//!	Define(pGs,  TE_TotMinAds,"TE Total Mineral Adsorbed", 0.0,mMASS_UNIT, "");

	pGs = new Gs(&TE_TotStorage, SINGLE, SALT, STORAGE, NOBPROC, NORMAL);
	Define(pGs, "TE Total Storage", 0.0,UNIT_TYPES::mMASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");

	pGs = new Gs(&TE_TotPlantUptake, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pGs, "TE Total PlantUptake", 0.0,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pGs = new Gs(&TE_TotLitterFall, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pGs, "TE Total Litterfall", 0.0,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");
	pGs = new Gs(&TE_TotMineralisation, SINGLE, SALT, TRANSPORT, NOBPROC, NORMAL);
	Define(pGs, "TE Total Mineralisation", 0.0,UNIT_TYPES::mMASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|TraceElementUptake>0");

	pPs = new Ps(&PassiveUptScaling, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "PassiveUptScaling", 1.E-3, "-", "TraceElementUptake>0|PassiveUptake>0", 0., 1.);
	pPs = new Ps(&PassiveUptAlloFLeaf, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "PassiveUptAll0FLeaf", .2, "-", "TraceElementUptake>0|PassiveUptake>0", 0., 1.);
	pPs = new Ps(&PassiveUptAlloFStem, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "PassiveUptAll0FStem", .1, "-", "TraceElementUptake>0|PassiveUptake>0", 0., 1.);

	pPs = new Ps(&ActiveUptEffLeaf, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "ActiveUptEffLeaf", 1.E-6, "mg/g", "TraceElementUptake>0|ActiveUptake>0", 0., 10.);
	pPs = new Ps(&ActiveUptEffStem, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "ActiveUptEffStem", 1.E-6, "mg/g", "TraceElementUptake>0|ActiveUptake>0", 0., 10.);
	pPs = new Ps(&ActiveUptEffRoots, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "ActiveUptEffRoots", 1.E-6, "mg/g", "TraceElementUptake>0|ActiveUptake>0", 0., 10.);
	pPs = new Ps(&ActiveUptMaxEffConc, SINGLE, SALT, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "ActiveUptMaxEffConc", 1.E-6, "mg/l", "TraceElementUptake>0|ActiveUptake>0", 0., 10.);

	F* pF = new F(&m_PG_RoadSurface, ROADSURFACE, SALT, SALTSOURCE, NOBPROC, NORMAL);
	Define(pF, "Salt Road Deposition", "SaltRoadFile.bin", "WaterEq = 1|SaltTracer>0|EnableInputPGSalt>=1");

	//	CALL F(Group_22, FileDriv11a,"Salt Road Deposition","BIN",.TRUE.,"SaltRoadFile","WaterEq = 1|SaltTracer>0|EnableInputPGSalt>=1");

	return true;
}
void Salt::TE_ActiveUptake(vector<double> Dist, size_t IndexP) {

	//Use CommonStrucN, only: P_CarbonAlloNewMobile.Leaf,CNewMobileStem ,CNewMobileRoots
	//real*8 Dist(*)
	//Integer IndexP, Nlayers, II
	NC_Plant* pNC;
	pNC = (NC_Plant*)pNC_Plant;

	ActiveUptakeLayers.assign(ActiveUptakeLayers.size(), 0.);
	size_t NLayers = ActiveUptakeLayers.size();
	//	Do II=1, Nlayers
	for (size_t II = 0; II<NLayers; II++)
		ActiveUptakeLayers[II] = Dist[II] * ActiveUptakeLeafFunc(SaltConc[II])*pNC->P_CarbonAlloNewMobile.Leaf[IndexP];

	for (size_t II = 0; II<NLayers; II++) SaltPlantUptake[II] += ActiveUptakeLayers[II];

	TE_PlantLeafUptake[IndexP] += sum(ActiveUptakeLayers);

	for (size_t II = 0; II<NLayers; II++)
		ActiveUptakeLayers[II] = Dist[II] * ActiveUptakeStemFunc(SaltConc[II])*pNC->P_CarbonAlloNewMobile.Stem[IndexP];

	for (size_t II = 0; II<NLayers; II++) SaltPlantUptake[II] += ActiveUptakeLayers[II];

	TE_PlantStemUptake[IndexP] += sum(ActiveUptakeLayers);


	for (size_t II = 0; II<NLayers; II++)
		ActiveUptakeLayers[II] = Dist[II] * ActiveUptakeRootsFunc(SaltConc[II])*pNC->P_CarbonAlloNewMobile.Roots[IndexP];

	for (size_t II = 0; II<NLayers; II++)	SaltPlantUptake[II] += ActiveUptakeLayers[II];

	TE_PlantRootUptake[IndexP] += sum(ActiveUptakeLayers);

}
double  Salt::ActiveUptakeLeafFunc(double Conc) {
	//! Unit of Function will be g/g of Carbon
	//Real Conc ! in mg/l

	return max(0., min(Conc / ActiveUptMaxEffConc, 1.))*ActiveUptEffLeaf;

}
double  Salt::ActiveUptakeStemFunc(double Conc) {
	//! Unit of Function will be g/g of Carbon
	//	Real Conc ! in mg/l

	return max(0., min(Conc / ActiveUptMaxEffConc, 1.))*ActiveUptEffStem;
}
double  Salt::ActiveUptakeRootsFunc(double Conc) {
	//! Unit of Function will be g/g of Carbon
	//	Real Conc ! in mg/l

	return max(0., min(Conc / ActiveUptMaxEffConc, 1.))*ActiveUptEffRoots;
}
double  Salt::ActiveMaxEffFunc(double conc) {
	//! Unit of Function will be g/g of Carbon
	//	Real Conc ! in mg/l

	return min(conc / ActiveUptMaxEffConc, 1.);
}
void Salt::TE_PassiveUptake(vector<double> Wflow, size_t IndexP) {
	//! Conc mg/l and Wflow in mm/day

	double AccLayer, ULayer;
	//Integer IndexP, NLayers

	AccLayer = 0.;
	//Do i=1, Nlayers
	for (size_t i = 0; i<Wflow.size(); i++) {
		ULayer = max(0., SaltConc[i] * Wflow[i])*PassiveUptScaling*PassiveUptAlloFLeaf;
		AccLayer += ULayer;
		SaltPlantUptake[i] += ULayer;
	}
	//EndDo
	TE_PlantLeafUptake[IndexP] = TE_PlantLeafUptake[IndexP] + AccLayer;


	AccLayer = 0.;
	//Do i=1, Nlayers
	for (size_t i = 0; i<Wflow.size(); i++) {
		ULayer = max(0., SaltConc[i] * Wflow[i])*PassiveUptScaling*PassiveUptAlloFStem;
		AccLayer += ULayer;
		SaltPlantUptake[i] += ULayer;
	}//EndDo
	TE_PlantStemUptake[IndexP] += AccLayer;

	AccLayer = 0.;
	for (size_t i = 0; i<Wflow.size(); i++) {
		ULayer = max(0., SaltConc[i] * Wflow[i])*PassiveUptScaling*(1 - PassiveUptAlloFLeaf - PassiveUptAlloFStem);
		AccLayer += ULayer;
		SaltPlantUptake[i] += ULayer;
	}
	TE_PlantRootUptake[IndexP] += AccLayer;

}//End Subroutine TE_PassiveUptake
double Salt::PassiveUptakeRootsFunc(size_t Index, double Wflow) {
	//! Conc mg/l and Wflow in mm/day
	double out;
	out = max(0., SaltConc[Index] * Wflow*PassiveUptScaling)*max(0., 1. - PassiveUptAlloFLeaf - PassiveUptAlloFStem);
	SaltPlantUptake[Index] = SaltPlantUptake[Index] + out;
	return out;
}
double Salt::PassiveUptakeStemFunc(size_t Index, double Wflow) {
	//! Conc mg/l and Wflow in mm/day

	double out;

	out = max(0., SaltConc[Index] * Wflow*PassiveUptScaling*PassiveUptAlloFStem);
	SaltPlantUptake[Index] = SaltPlantUptake[Index] + out;
	return out;
}
double Salt::SaltSolutionFunctionL(size_t index) {

	return SaltSolutionFunction(SaltStorage[index], p_Soil_WaterF->WaterStorage[index], Ad_c[index]);
}
double Salt::SaltSolutionFunction(double SaltAmount, double WaterAmount, double Ad_c) {
	double out;
	if (WaterAmount>1.0&&Adsorption>0)
		out = SaltAmount*(1 - Ad_c) / WaterAmount;
	else if (WaterAmount>1.0)
		out = SaltAmount / WaterAmount;
	else
		out = 0.001;

	out = max(0.001, out);

	//if(out<SaltInputConcPar/10.) 
	//	 out=SaltInputConcPar/10.;
	return out;
}
double Salt::FrontLowerBoundary() {

	double Position;

	Position = 0.;
	for (size_t i = SaltConc.size() - 1; i>0; i--) {
		//ProfileLoop: DO  i=NL,2,-1
		if (SaltConc[i] <= ConcForFront&&SaltConc[i - 1]>ConcForFront) {
			return -p_Soil_WaterF->DepthMidpLayer[i - 1] -
				(1 - (ConcForFront - SaltConc[i]) /
				(SaltConc[i - 1] - SaltConc[i]))*p_Soil_WaterF->DistanceMidpLayers[i - 1];
		}
	}
	return Position;
}
bool Salt::CheckFromRoadFile() {
	string str;
	int  Imiss;  //, I2	! Local Variable

	if (!m_PG_RoadSurface.ReOpen()) return false;


	Imiss = 0;
	AirDepPos.ivar = 0;
	PloughDepPos.ivar = 0;
	RunOffDepPos.ivar = 0;

	//ResetReadWrite=true

	for (size_t i = 0; i<m_PG_RoadSurface.GetNumVariables(); i++) {
		string name = m_PG_RoadSurface.GetVarName(i + 1);
		string Id = m_PG_RoadSurface.GetVarId(i + 1);

		for (size_t ii = 0; ii<20; ii++) {
			if (int(name[ii]) >= 97) {
				name[ii] = char(int(name[ii]) - 32);
			}
		}
		if (name.find("SALT AIREMISSION")>0) {
			if (AirDepPos.ivar == 0)
				AirDepPos.ivar = i;
			else
				Message("Warning Air deposition variable not found in file :" + name);

		}
		else if (name.find("SALT PLOUGHEMISSION")>0) {
			if (PloughDepPos.ivar == 0)
				PloughDepPos.ivar = i;
			else
				Message("Warning Plough deposition variable not found in file :" + name);
		}
		else if (name.find("SALT RUNOFFEMISSION")>0) {
			if (RunOffDepPos.ivar == 0)
				RunOffDepPos.ivar = i;
			else
				Message("Warning Runoff deposition variable not found in file :" + name);


		}

	}  //EndDo

	if (AirDepPos.ivar == 0) {
		Message("Error Too few Salt deposition variables in file :" + m_PG_RoadSurface.GetFileName());
		m_pModelInfo->Run_Res_Id = false;
	}
	if (PloughDepPos.ivar == 0) {

		Message("Missing Plough Deposition variables in file :" + m_PG_RoadSurface.GetFileName());
		m_pModelInfo->Run_Res_Id = false;

	}
	if (RunOffDepPos.ivar == 0) {
		Message("Error Too few Salt deposition variables (RunOff) in file :" + m_PG_RoadSurface.GetFileName());
		m_pModelInfo->Run_Res_Id = false;

	}

	return m_pModelInfo->Run_Res_Id;
}
double Salt::DistanceDepFunction(double length) {
	Additional_Variables *pAdd = (Additional_Variables*)p_Structure;
	if (length>pAdd->Scale / 2)
		return 0.5*(exp(-(length - pAdd->Scale / 2)*DistKSplash) - exp(-(length + pAdd->Scale / 2)*DistKSplash));
	else
		return 1 - exp(-(length + pAdd->Scale / 2)*DistKSplash) - exp(-(pAdd->Scale / 2 - length)*DistKSplash);

}
double Salt::DistanceDepAirFunction(double length) {
	Additional_Variables *pAdd = (Additional_Variables*)p_Structure;

	if (length>pAdd->Scale / 2)
		return 0.5*(FracAir1*exp(-(length - pAdd->Scale / 2)*DistKAir1) + (1 - FracAir1)*exp(-(length - pAdd->Scale / 2)*DistKAir2) -
		(FracAir1*exp(-(length + pAdd->Scale / 2)*DistKAir1) + (1 - FracAir1)*exp(-(length + pAdd->Scale / 2)*DistKAir2)));
	else
		return 1 - 0.5*((FracAir1*exp(-(length + pAdd->Scale / 2)*DistKAir1) + (1 - FracAir1)*exp(-(length + pAdd->Scale / 2)*DistKAir2)) +
		(FracAir1*exp(-(pAdd->Scale / 2 - length)*DistKAir1) + (1 - FracAir1)*exp(-(pAdd->Scale / 2 - length)*DistKAir2)));

}
double Salt::SaltRunOffDepositionFunc(double EmissionV) {
	Additional_Variables *pAdd = (Additional_Variables*)p_Structure;

	if (DistFromRoad>pAdd->Scale / 2)
		return 0.;
	else
		return EmissionV*RoadWidth / pAdd->Scale*FractRunOffEmission;
}
double Salt::Temp_Sensitivity(double Temperature) {
	return 1 + Res_Temp_Sens*(Temperature - Res_Temp_N);
}
double Salt::Cond_Estimate(double Conc) {
	return 10.*Conc / Res_IonCoef;
}
double Salt::WaterResistivityL(size_t Layer) {
	return 1000. / (Cond_Estimate(SaltConc[Layer]) / Temp_Sensitivity(((Soil_HeatF*)p_Soil_HeatF)->Temp[Layer]));
}
double Salt::Archie(double Porosity, double  WaterContent, double  WaterResistivity) {
	return Archie_A*pow(Porosity / 100, -Archie_M)*pow(WaterContent / Porosity, -Archie_N)*WaterResistivity;
}
double Salt::ArchieL(size_t Layer) {
	return Archie(p_Soil_WaterF->M_Saturation[Layer], p_Soil_WaterF->Theta[Layer], WaterResistivity[Layer]);
}
double Salt::OsmoticPressureFuncL(size_t index) {

	return OsmoticPressureFunc(SaltConc[index], ((Soil_HeatF*)p_Soil_HeatF)->Temp[index]);
}
double Salt::OsmoticPressureFunc(double Conc, double Temp) {

	return -Fix::RGAS*(Temp + 273.15)*Conc / Fix::MolMassSalt*Fix::PatoCmWater;
}
double Salt::SaltAirDepositionFunc(double EmissionV) {
	Additional_Variables *pAdd = (Additional_Variables*)p_Structure;
	if (BlomquistEquation>0)
		return EmissionV*RoadWidth / pAdd->Scale*DistanceDepAirFunction(DistFromRoad);
	else
		return EmissionV* FractAirEmission;
}
double Salt::SaltPloughDepositionFunc(double EmissionV) {
	Additional_Variables *pAdd = (Additional_Variables*)p_Structure;
	if (BlomquistEquation>0)
		return EmissionV*RoadWidth / pAdd->Scale*DistanceDepFunction(DistFromRoad);
	else
		return EmissionV*FractPloughEmission;
}
double Salt::SaltAirEmissionFunc() {

	if (SaltOnRoad>0)
		return SaltOnRoad*EmissionRateCoef;
	else
		return 0.;
}
double Salt::SaltPloughEmissionFunc() {
	SnowPack *pMet = (SnowPack*)p_Atm;
	if (pMet->SnowExport>0 && SaltOnRoad>0)
		return SaltOnRoad*min(pMet->SnowExport, 1.)*SaltPloughEffCoef;
	else
		return 0.;

}
double Salt::SaltRunOffEmissionFunc() {
	//Use Surface_Water,  Only: SurfacePool, SpoolRunOff
	//Use TimeInfo, Only		: T_Step
	//Real SaltsurfaceConc
	Irrigation *pMet = (Irrigation*)p_Atm;
	double SaltSurfaceConc;

	if (pMet->SurfacePool>0.1&&SaltOnRoad>0)
		SaltSurfaceConc = SaltOnRoad / pMet->SurfacePool;
	else
		SaltSurfaceConc = 0.;

	return max(0., min(pMet->SpoolRunoff*SaltSurfaceConc*SaltRunOffEffCoef, SaltOnRoad / T_Step));

}
double Salt::SaltRunOffCloseToRoadFunc() {
	//Use Surface_Water,  Only: SurfacePool, SpoolRunOff
	//Use TimeInfo, Only		: T_Step
	Irrigation *pMet = (Irrigation*)p_Atm;
	double SaltSurfaceConc = 0;

	if (pMet->SurfacePool>0.1&&SaltCloseToRoad>0.&&pMet->TAir>0.)
		SaltSurfaceConc = SaltCloseToRoad / pMet->SurfacePool;



	return max(0., min(pMet->SpoolRunoff*SaltSurfaceConc*SaltRunOffEffCoef, SaltCloseToRoad / T_Step));

}
