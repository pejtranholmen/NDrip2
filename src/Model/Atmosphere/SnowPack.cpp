#include "SnowPack.h"
#include "../PhysConst.h"
#include "../SOIL/Soil_HeatF.h"
#include "../../Simulator/Simulator.h"
#include "./Functors/SnowPack_Functors.h"
						   
SnowPack::SnowPack(void)
{
	ModuleName="SnowPack";
	ModuleNo=14;
	ModuleConditions="SnowPack>0";
}

bool SnowPack::Ini()	{
	
	if(!Radiation::Ini()) return false;
	reset_snow=true;
	if(SnowPack_Option_Sw(m_pModelStructure->SnowPack_Option)== SnowPack_Option_Sw::off)
		FracBare=1.;
	else {	
		reset_snow=false;
		if(!m_pModelInfo->StartStateValue) {
			HSnow=SnowDepthInitial;
			WTotSnow=SnowMassInitial;
		}
		WICE=WTotSnow;
		f_BareSoilCover_Snow = Get_D1_Functor(MyFunc::BARESOIL_SNOWHEIGHT);
		if(HSnow>0) DENSS=WTotSnow/HSnow;
		FracBare=f_BareSoilCover_Snow(HSnow);
	}


	DENSS=100.;
	if(SnowAdjustment_Sw(SnowAdjustment)>= SnowAdjustment_Sw::FORCED_TO_CONTINOUS) {
		string filename=m_PG_Snow.GetFileName();
		m_pModelInfo->Run_Res_Id = m_PG_Snow.Open(filename);
		m_PG_Snow.SetEndPoints_ValidValue();
		ISnowAdjust.ivar = 1;
		if (SnowAdjustment_Sw(SnowAdjustment) == SnowAdjustment_Sw::FORCED_TO_CONTINOUS)
			ISnowAdjust.discrete = false;
		else
			ISnowAdjust.discrete = true;
		ISnowAdjust.pgfile = &m_PG_Snow;
		ISnowAdjust.shift = 0;
		ISnowAdjust.pgfile->ResetPos();
	}
	
	if(p_Plant->DistFromRoad<0.5) 
	    p_Plant->DistFromRoad=0.501;
	return true;
}
bool SnowPack::IniFunctors() {

	if (!Radiation::IniFunctors()) return false;
	f_BareSoilCover_Snow = Get_D1_Functor(MyFunc::BARESOIL_SNOWHEIGHT);

	f_StabCorrAnalytical = Get_D4_Functor(MyFunc::STABILITY_ANALYTICAL);
	f_ResAirMom = Get_D4_PhysFunctor(FUNC_CHOICE::RES_AIR_MOM_HEAT);
	//	f_ResAir_Snow = Get_D2_Functor(MyFunc::RA_SNOW_NEUTRAL);

	if (StabilitySnow_Sw(StabilitySnow) == StabilitySnow_Sw::RICHARDSON_NUMBER) f_ResAirSnow = Get_D4_Functor(MyFunc::RA_SNOW_ANALYTICAL);
	else f_ResAirSnow = Get_D4_Functor(MyFunc::RA_SNOW_NUMERICAL);


	f_SensibleFlowByResistance = Get_D2_PhysFunctor(FUNC_CHOICE::SENSHEAT_AIRFLUX);
	f_LatentFlowByResistance = Get_D2_PhysFunctor(FUNC_CHOICE::LATENTHEAT_AIRFLUX);

	f_ThCond_Snow = Get_D1_Functor(MyFunc::SNOW_THERMALCOND);
	f_SaltMelt_Road = Get_D2_Functor(MyFunc::ROADICE_SALTMELT);
	f_SnowMelt = Get_D5_Functor(MyFunc::SNOW_MELTING_EMPIRICAL);


	if (NewSnowDensityModel_Sw(NewSnowDensityModel) == NewSnowDensityModel_Sw::LINEAR_AIRTEMP)
		f_NewSnowDensity = Get_D2_Functor(MyFunc::NEWSNOW_DENSITY_LINEAR);
	else 
		f_NewSnowDensity = Get_D2_Functor(MyFunc::NEWSNOW_DENSITY_EXP);
	return true;
}
bool SnowPack::Dyn()
{
	if(m_pModelStructure->SnowPack_Option>0) {
		Flux();
		Integ();
	}
	return true;
}

void SnowPack::Flux()
{
	
//	Real ImportSnowMass, HSnowImport

	Snow_Prev=WTotSnow;
//! if snow was removed last timestep, reset fluxes to 0 to avoid massbalance error
	SnowOutFlow=0.;
	SnowInFlow=0.;
	if(reset_snow) reset_snow=Reset_SnowFluxes();

//! Next will be removed after final testing - it is probably a remnant from older versions:
//!****** CHECK if REPEATED CALL WITHIN DAY OR HOUR/10 MINUTES ***********
	if(TimeResolution_Sw(m_pModelStructure->TimeResolution)!=TimeResolution_Sw::DAILY_MEAN&&!m_pModelInfo->FIRST)
		return;
	

//!=====================================================================
//! 0.2 INITIALISATION OF SNOW ADJUSTMENT ROUTINE
//!=====================================================================
	if(SnowAdjustment>=1) {
		SnowMeasured =GetEcoDat(&m_PGPOS_AdjustSnow, ISnowAdjust);
		OldSnowDepth=HSnow;
		PrecAdjustSnow=0.0;
	}

//!====================================================================
//! 0.1 SET LOCAL TIME STEP IN SNOW MODULE
//!==================================================================== 
      if(TimeResolution_Sw(m_pModelStructure->TimeResolution)== TimeResolution_Sw::WITHIN_DAY)
        TISTEP=T_Step;
      else
        TISTEP=m_pModelInfo->T_StepSnow;
    
//!	  SnowOutFlow=0.
//!====================================================================
//! 0. CHECK if SNOW IS DEEPER THAN 0.1 MM OR SNOWING, else REMOVE SNOW
//!====================================================================

      if(WTotSnow<=1.E-8&&TAir>PRainLim&&ThQualP<=0.&&SnowImport<=0.) {
			FracBare=1.;
			snow_endstep();
			return;
	  }

//! =======================================================================
//! 0.3 Density and intensity of precipitation
//! =======================================================================
	//new_density(ThQualP,SNOWP,RAIN,DENSICE_N,DENSLIQ_N,DENSS_N, TPrecip,ThroughFall);
	SnowPrec NewSnow=new_density(p_Plant->ThroughFall);

//! Next if-statement will be removed after final testing
	if(WTotSnow>0. ||NewSnow.Rate_Snow>0.) {   
//! =======================================================================
//! 1. Preliminary mass balance - assuming all precip. in beginning of time step
//! =======================================================================
//! save old snow-, ice- and liquid water mass
	double WICEOLD=WICE;
	double SWATSOLD=SWATS;
	double WTotSnowOld=WTotSnow;
//! 1.1 Update of snow mass variables   
	SnowInFlow=p_Plant->ThroughFall;

	InFlow(WTotSnow,SnowInFlow);
	InFlow(WICE,NewSnow.Rate_Snow);
	//WICE+=NewSnow.Rate_Snow*TISTEP; 
	double ImportSnowMass;
	if(SnowPloughInput_Sw(SnowPloughInput)== SnowPloughInput_Sw::LATERAL_INPUTFILE) {
	    ImportSnowMass=p_Plant->RoadWidth*SnowImport*TISTEP*SnowPloughDistFunction();    
	    WTotSnow+=ImportSnowMass;    
		WICE+=p_Plant->RoadWidth*ThQualP*SnowImport*TISTEP*SnowPloughDistFunction(); 
	}

	SWATS=WTotSnow-WICE;

//! Continue if snowmass is greater than zero
//if(WTotSnow>0.){
//! 1.2 DENSITY AND THICKNESS OF PRECIPITION
	double HSNOW_NEW=p_Plant->ThroughFall*TISTEP/NewSnow.Dens_Snow; 
	double HSnowImport;
		if(SnowPloughInput==1) 	    
		        HSnowImport=ImportSnowMass/SDensNewImport;
		else
		        HSnowImport=0.;        
	

//! 1.3  DENSITY AND THICKNESS OF UPDATED SNOWPACK - COMPACTION AFTER MASS BALANCE
	DENSS=(NewSnow.Dens_Snow*HSNOW_NEW+SDensNewImport*HSnowImport+DENSS*HSnow)/(HSNOW_NEW+HSnow+HSnowImport) ;
	HSnow+=HSNOW_NEW+HSnowImport;

//!*************** FRACTION OF BARE SOIL ********************************

		 FracBare= f_BareSoilCover_Snow(HSnow);
	
	double BULKDENS_ICE, BULKDENS_LIQ;	
    if(HSnow>0.) {
	    BULKDENS_ICE=WICE/HSnow;
	    BULKDENS_LIQ=SWATS/HSnow;
	}
	else {
	    BULKDENS_ICE=WICE;
	    BULKDENS_LIQ=SWATS;
	}    
	
//! 1.4 Age of snow pack
	if(NewSnow.Rate_Snow>SAgeZP && (NewSnow.Rate_Snow/p_Plant->ThroughFall>SAgeZQ)) SnowAge=0.; 
	if(NewSnow.Rate_Snow<=0.) SnowAge+=TISTEP;

//! ======================================================================= 
//! 2. HEAT AND MASS BALANCE OF SNOW 
//! =======================================================================

	//! 2.0 Select snowmelt model Empirical(0) or Heat balance.front()	 
	//! ---------------------------------------------------------	
	if(SnowMeltFunctionSwitch_Sw(SnowMeltFunctionSwitch)== SnowMeltFunctionSwitch_Sw::Empirical){
	  if(Actual_EmpiricalMelting()) {
 				Reset_SnowPack();
				snow_endstep();
				return;
	  }

	//! snow surface energy balance - steady state solution
		RETENS=RetentionFunction(WTotSnow); // moved to this position prior Snow Temperature function call
		QMeltSurface=Snow_Ebal_Temperatures(0);
		if(SnowSurfaceMassBalance() ){
			snow_endstep();
			return;
		}
		HeatChangeSnow=0.;
		TempSnow=(TempSnowSurf+TempSnowSoil)/2.;
	}
	else {
		// heatcontent of snow
		double HeatContentSnow=SnowHeatContentFunction(NewSnow.Temperature, NewSnow.Rate_Snow, NewSnow.Rate_Rain)*TISTEP+
						SnowHeatContentFunction(TempSnow, WICEOLD, SWATSOLD);
		// Melting because of precipitation
		MELTPREC=Actual_Phys_PrecipMelting(HeatContentSnow);		
		
		// update of ice and water storage
		WICE=max(0.,WICE-MELTPREC); 
		SWATS=max(0.,SWATS+MELTPREC); 
		
		// Temperature of Snow
		TempSnow=SnowTempFunction(HeatContentSnow-MELTPREC*Fix::LATMELT, WICE, SWATS);

	
		if(WICE<=0){  //if no ice exist in snow all snow disappear because of melting
			 SnowOutFlow=WTotSnow/TISTEP;
			 Reset_SnowPack();
			 snow_endstep();
			 return;
		}


	//! 2B.2 Snow heat balance - calculation of fluxes
	//! ----------------------------------------------
		if(HSnow>=0.01) { //! snow surface energy balance - dynamic heat storage in snow

			QMeltSurface=Snow_Ebal_Temperatures(1);
			HeatChangeSnow=(1.-FracBare)*(SnowSurfHeatF-QSnowSoil)*TISTEP;
		}
		else {
		//! snow surface energy balance - steady state solution
			QMeltSurface=Snow_Ebal_Temperatures(0);
			HeatChangeSnow=0.;
			TempSnow=(TempSnowSurf+TempSnowSoil)/2.;
		}
		
   //! 2B.3 Surface melting and sublimation
   //! ------------------------------------

		if(SnowSurfaceMassBalance()) {
			snow_endstep();
			return;
		}

		 if(QMeltSurface>=0.){
			MELTSURF=Actual_Phys_SurfaceMelting(QMeltSurface);
			if(MELTSURF==WICE) {
				SnowOutFlow=WTotSnow/TISTEP;
 				Reset_SnowPack();
				snow_endstep();
				return;
			}
			else {
				MELTHEATSURF=MELTSURF*Fix::LATMELT; 
      			WICE=WICE-MELTSURF;
      			SWATS=SWATS+MELTSURF; 
			}
		  }
	   
	//! 2B.4 Final snow heat and mass balance
		if(HeatChangeSnow<=0.){
			//Freezing corresponds to negative melting
			MELT=Actual_Phys_SubSurfaceMelting(HeatChangeSnow);
			WICE=WICE-MELT; 
			SWATS=SWATS+MELT; 
			MELTHEAT=MELT*Fix::LATMELT; 
			TCHANGE=(HeatChangeSnow-MELTHEAT)/(WICE*Fix::SPECI); 
			TempSnow=max(TempSnow+TCHANGE, TempSnowSurf);
		}///! Added to avoid melting of snow -PEJ 030309
		else {
			TCHANGE=min(-TempSnow+TempSnowSurf,HeatChangeSnow/(WICE*Fix::SPECI));// ! Added TempSnowSurf - pej 030309
			TempSnow=TempSnow+TCHANGE; 
			POTMELT=max(0.,(HeatChangeSnow-TCHANGE*WICE*Fix::SPECI)/Fix::LATMELT); 
			if(TempSnow<-2.0) POTMELT=0.;//	! Added to avoid melting of cold snow  - PEJ 030309
			if(POTMELT>=WICE){
				MELT=WICE; 
				SnowOutFlow=(WICE+SWATS)/TISTEP;
 				Reset_SnowPack();
				snow_endstep();
				return;
			}
			else {
				MELT=POTMELT;
				MELTHEAT=MELT*Fix::LATMELT;
      			WICE=WICE-MELT; 
      			SWATS=SWATS+MELT;
			}
		}
	}
	//! 3. RETENTION CAPACITY
	//! ------------------------------- 
	RETENS=RetentionFunction(WTotSnow);

	//! 4. OUTFLOW OF WATER FROM SNOWPACK
	//!----------------------------------
		SnowOutFlow=max(0.,(SWATS-RETENS)/TISTEP);
		

		if(SnowOutFlow>0&&RETENS<.02) { //! Mkt konstigt, kolla upp!!!
			SnowOutFlow=WTotSnow/TISTEP;
			Reset_SnowPack();
			snow_endstep();
			return;
		}
		else {
//!*************** REMAINING WATER ***************************************
			SWATS=min(SWATS,RETENS);
			if(SWATS<.01){
				david=SWATS;
				SWATS=0.;
			}
			
			WTotSnow-=SnowOutFlow*TISTEP; 		
			
//!*************** DENSIFICATION OF REMAINING SNOWPACK ******************     
	//		! SWE

			if(WTotSnow<=0){
				Reset_SnowPack();
				snow_endstep();
				return;
			}
			else {
				if(SnowDensification_Sw(SnowDensification)== SnowDensification_Sw::ICE_LIQUID_CONTENT) {
					DENS_SNOW a=SimpleDensityFunction(SWATS/RETENS, WTotSnow);
					DENSS=a.Density;
					HSnow=a.Height;
					CR=a.CR;
				}
				else {
					POROSITY=1.-BULKDENS_ICE/Fix::DENSICE;
					SATURATION=min(1.,BULKDENS_LIQ/(Fix::DENSW*POROSITY));
					DENS_SNOW a=SNTermDensityFunction();
					DENSS=a.Density;
					HSnow=a.Height;
					CR=a.CR;
				}
				Snow_On_RadImpact(HSnow);
				return;


			}
		} //! SnowOutFow
	}  //! WTotSnow


     snow_endstep();


}
void SnowPack::Integ()
{
}

bool SnowPack::Actual_EmpiricalMelting()
{
		//! 2A.1 EMPIRICAL FREEZING AND MELTING FUNCTION
	//! -------------------------------------------
	
	//! snowmelt/refreezing function
		double MPOT=f_SnowMelt(TAir,RadInShort,SnowAge ,HSnow, ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow);
	//! liquid water in snow pack
		double POTMELT=MPOT*TISTEP;
		if(ChlorideMelt_Sw(ChlorideMelt)==ChlorideMelt_Sw::ON) POTMELT=POTMELT+f_SaltMelt_Road(TAir, p_Plant->SaltOnRoad);


		if(POTMELT>=0.){
			if(POTMELT>=WICE){
				MELTSURF=WICE; 
				SnowOutFlow=WTotSnow/TISTEP;
				return true; 
			}
			else {
				MELTSURF=POTMELT ;
      			WICE=WICE-MELTSURF; 
      			SWATS=SWATS+MELTSURF; 
			}
		}
		else {
	//! refreezing
			MELTSURF=0.;
			WICE=min(WTotSnow,WICE-POTMELT);
			SWATS=WTotSnow-WICE;
		}
		return false;

}
bool SnowPack::End()
{
	Radiation::End();
return true;
}
SnowPack::~SnowPack(void)
{
}
bool SnowPack::Reset_SnowPack()
{

	HSnow=SnowAge=SWATS=WICE=WTotSnow=DENSS=TempSnowSurf=TempSnowSoil=0.0;
	FracBare=1.;
	SnowReduceLAIFactor.assign(p_Plant->NumPlants, 1.);
	return true;
}
bool SnowPack::Reset_SnowFluxes()
{
	SnowOutFlow=SnowSensF=SnowLatentF=SnowSurfHeatF=QMeltSurface=SnowEvaporation=PrecAdjustSnow=0.0;
	FracBare=1.;
	return false;
}
double SnowPack::MeasuredSnowFunction()
{	double DCH;
	if(TimeResolution_Sw(m_pModelStructure->TimeResolution)== TimeResolution_Sw::DAILY_MEAN)
          DCH=0.5;
    else if(TimeResolution_Sw(m_pModelStructure->TimeResolution)== TimeResolution_Sw::HOURLY)
		  DCH=1./48;
	else if(TimeResolution_Sw(m_pModelStructure->TimeResolution)== TimeResolution_Sw::TEN_MINUTES)
		  DCH=1./288;
	else
          DCH=0.;
    

	return 0.;
//	MeasuredSnowResult=READECOScalar(T_Current+DCH, ISNOW_FILE) 
	
}
void SnowPack::snow_endstep()
{
	reset_snow=true;

	f_BareSoilCover_Snow(HSnow);
	HSnow=0;

	Snow_Increase=WTotSnow-Snow_Prev;

	if(Snow_Increase!=p_Plant->ThroughFall-SnowOutFlow&&ThQualP>0) {
		double diff=Snow_Increase-p_Plant->ThroughFall+SnowOutFlow;

	}
	Snow_On_RadImpact(HSnow);

	// !*************** Forcing snow depth to match observations ****************	
  if(SnowAdjustment_Sw(SnowAdjustment)>= SnowAdjustment_Sw::FORCED_TO_CONTINOUS&&SnowMeasured>0.) {
	  SnowDensity = UpdateSnowDensity();
	  PrecAdjustSnow = AdjustedSnowPrecAndSnow(); 
 // Force Snow depth to be equal measured
 // Adjusted also mass of Snow

	  FracBare = f_BareSoilCover_Snow(HSnow);

  }

}
//subroutine new_density(fice,snowrate,rainrate,dens_i,dens_l,dens_w,tprecip,prcp)
SnowPrec SnowPack::new_density(double prcp) {

	SnowPrec out;
/*use genfunctions, only: SVP
use commonstruc, only: SnowInterception
! arguments
real snowrate,rainrate,dens_i,dens_l,dens_w,tprecip,prcp
real VapourPAirSat, VapourPAir
integer model
! locals
real fice,fliq,FliqMax,e0,bp,rh
! koden*/
double FliqMax=0.5;
double rh, VapourPAirSat;
//! 1. Thermisk kvalitet på nederbörden
ThQualP=ThermQualPrecFunction(TAir);
//if(SnowInterception<=0||fice<=0.) 
//! 2. densitet och temperatur på nederbörden
out.Dens_Snow=f_NewSnowDensity(ThQualP,TAir);   
//se(NewDensModel)
switch (NewSnowDensityModel) {
case 0://	!% NEW SOIL MODEL
   if(ThQualP>0.0) 
      out.Temperature=min(0.0,TAir);
  else 
	out.Temperature=TAir;

   out.Dens_Ice=out.Dens_Snow*ThQualP;
   out.Dens_Liq=out.Dens_Snow*(1-ThQualP);
   out.Rate_Snow=ThQualP*prcp;
   out.Rate_Rain=(1-ThQualP)*prcp;
   break;
case 1: //! COMBINED HEDSTROM/NEW SOIL
   //double e0=6.1368;
   //double bp=1013.0;
	VapourPAirSat=ModelFunctions::SVP(TAir);			//	! - saturated vapour pressure of air at ref.height
	VapourPAir=VapourPAirSat-VapourPDef;
	rh=100.*(VapourPAir/SVP(TAir));
//!   dens_i=67.92+51.25*exp(TAir/2.59)
   if(ThQualP>0.0){
       out.Dens_Ice=out.Dens_Snow*ThQualP;
      out.Dens_Liq=out.Dens_Snow-out.Dens_Ice;
	   out.Temperature=min(0.0,TAir);
     // out.Temperature=min(0., wetbulb(TAir+273.15,fvapri(TAir+273.15,rh,e0),bp)  );
   }
   else {
      out.Dens_Liq=1000.0;
	  out.Dens_Ice=0.0;
	  out.Temperature=TAir;
      //out.Temperature=wetbulb(TAir+273.15,fvaprw(TAir+273.15,rh,e0),bp);
   }
   out.Rate_Snow=prcp*ThQualP;
   out.Rate_Rain=prcp*(1-ThQualP);
   break;
default:
	out.Dens_Ice=out.Dens_Liq=out.Dens_Snow=out.Rate_Rain=out.Rate_Snow=out.Temperature=0;
   ThQualP=0;
}
//! To avoid unrealistic snow densities when using the "density-based" densificationfunction
//! ----------------------------------------------------------------------------------------
if(SnowDensification_Sw(SnowDensification)== SnowDensification_Sw::ICE_LIQUID_CONTENT) out.Dens_Snow=min(out.Dens_Snow  ,0.5*Fix::DENSW);
return out;
}

double SnowPack::SnowPloughDistFunction() {

//!if(DistFromRoad>0.49) {
//!    SnowPloughDistFunction=exp(-DistKSnowPlough*(DistFromRoad-0.5))-exp(-DistKSnowPlough*(DistFromRoad+0.5))    
//!}    
return p_Plant->SnowPloughFrac;
}



double SnowPack::Snow_Ebal_Temperatures(int INDICATOR) {

  LateVapSnow=LateSnowFunc();

  double ThetaU=p_Soil_WaterF->WaterStorage.front()/p_Soil_WaterF->ThicknessLayers.front()/10.;

  if(TimeResolution_Sw(m_pModelStructure->TimeResolution)== TimeResolution_Sw::DAILY_MEAN) {
     Rsoil=ThCond_R20(ThetaU, ((Soil_HeatF*)p_Soil_HeatF)->ThQual.front());
     KSOIL=p_Soil_WaterF->ThicknessLayers.front()/(2.*Rsoil);
  }
  else
     KSOIL=ThCond_Upper(ThetaU, ((Soil_HeatF*)p_Soil_HeatF)->ThQual.front());
  

  KSNOW=f_ThCond_Snow(DENSS);

  if(SnowSurEbal_Sw(SnowSurEbal)<= SnowSurEbal_Sw::AIR_TEMP) {
     //! ***** TSNOWSURF=TAIR *****
     TempSnowSurf=min(0.,TAir);
     TempSnowSoil=SteadyStateSol(HSnow, KSNOW, p_Soil_WaterF->ThicknessLayers.front()/2., KSOIL, TempSnowSurf, ((Soil_HeatF*)p_Soil_HeatF)->Temp.front());
  }
  else {
//     ! ***** ITERATIVE ENERGY BALANCE ******
     switch(INDICATOR) {
	 case 0:
        /*!	TBelow=Temp.front()
        !	KSNOWSOIL=KSNOW*KSOIL/(KSNOW*ThicknessLayers.front()/2 & 
        !             +KSOIL*sngl(HSNOW))
        !	RSNOW=max(0.1,1./KSNOWSOIL)*/
        ZSnow=HSnow;
        Rsnow=max(0.0001,ZSnow/KSNOW);
		break;
       // !			RSNOW=max(0.0001,SNGL(Hsnow)/Thcond_Snow(DENSS))
	 case 1:
        Tbelow=TempSnow;
        ZSnow=HSnow*0.5;
        Rsnow=max(0.0001,ZSnow/KSNOW);

     
	 }
     KSNOW=ZSnow/Rsnow;
     //! C1. Initialization of variables used in B2.
     QMeltSurface=0.0;//					! - Surface heat flux used for snow melt

     RoughLHeatSnow=RoughLMomSnow/exp(p_Plant->KBMinusOne);//	! - z0 HEAT
     Irep=1;//								! - number of iterations counter
     VapourPAirSat=ModelFunctions::SVP(TAir);//				! - saturated vapour pressure of air at ref.height
     VapourPAir=VapourPAirSat-VapourPDef;// ! - vapour pressure of air at ref. height
     TempSnowSurf=TAir;				//	! - first guess Tsnow=Tair
     SnowEbal=1.e6;					//	! - default energy balance closure
     POS=false;						//	! - sign indicator of temperature changes
     NEG=false;						//	! - sign indicator of temperature changes
    // ! C2. Iterative solution of surface energy balance
     SnowSensF_Old=SnowSensF;
     SnowLatentF_Old=SnowLatentF;
	 double CHANGE;
     while (abs(SnowEbal)>1.E5&&Irep<40) {
       // ! C2.1 Estimate aerodynamic resistance
        //RaSnowSurf=NewRaHeatFunctionSnow(TempSnowSurf);
		RaSnowSurf = f_ResAirSnow(WindSpeed, RefHeight, TAir - TempSnowSurf, TAir);
        //! C2.2 Estimate sensible and latent heat fluxes
        SnowSensF=Sens_AirF(TempSnowSurf-TAir,RaSnowSurf);
        VapourPSnowSurf=SVP(TempSnowSurf);
		SnowLatentF=Latent_AirF(VapourPSnowSurf-VapourPAir,RaSnowSurf)*LateVapSnow/Fix::LATE;
        //! C2.3 Estimate snow surface heat flux
        if(INDICATOR==0){
           Tbelow=SteadyStateSol(HSnow, KSNOW, p_Soil_WaterF->ThicknessLayers.front()/2.,KSOIL, TempSnowSurf,((Soil_HeatF*)p_Soil_HeatF)->Temp.front());
           TempSnowSoil=Tbelow;
        }
        SnowSurfHeatF=m_pModelInfo->SecPerDay*(TempSnowSurf-Tbelow)/Rsnow;
        if(SoilVapour_Sw(m_pModelStructure->SoilVapour)>= SoilVapour_Sw::Soil_and_SnowVapourflow) {
           ConcSurf=Conc_Vap(VapourPSnowSurf,TempSnowSurf);
           VapourPSoilU=SVP_David(Tbelow);// !*RHPsi(Psi.front(), Tbelow,1.)
           ConcSurf1=Conc_Vap(VapourPSoilU,Tbelow);
           AirP=(1000.-DENSS)/1000.;
           DatMU=D_vap(p_Soil_WaterF->DVapTortuosity,AirP,Tbelow);
           VapGra=(ConcSurf1-ConcSurf)/ZSnow;
           p_Soil_WaterF->VapourFlowSurf=-86.400*DatMU*VapGra;  // Vapour goes directly into the soil !!!!!!
           SnowSurfHeatF=SnowSurfHeatF+LateVapSnow*p_Soil_WaterF->VapourFlowSurf;
        }
      //  ! C2.4 Estimate net radiation and closure of surface energy balance
       // ! if net radiation not is measured. DG 22/9
        if(NetLongRad_Sw(NetLongRad)== NetLongRad_Sw::Two_Formulas) {
           if(LongRadCanopy_Sw(LongRadCanopy)== LongRadCanopy_Sw::Implicit)
              RadNetSnowCover=(RadNetShort+RadInLong-f_radlongout(TempSnowSurf))*(1.0-CanopyFracRad); //!BeerLaw(LAI_Total)
           else {
              if(CanopyShade_Sw(CanopyShade)>= CanopyShade_Sw::Function_solar_angle){ //! DG 2008-09-02 new canopy transmissivity model
                 RadNetShortSnow = RadInShortGround * (1.0-SnowAlbedoVar/100.);
                 RadNetLongSnow = RadInLongGround-f_radlongout(TempSnowSurf);
                 RadNetSnowCover = RadNetShortSnow+RadNetLongSnow;
			  }
              else 
                 RadNetSnowCover=RadNetShort*(1.0-CanopyFracRad)+RadInLongGround-f_radlongout(TempSnowSurf);

           }
           SnowEbal=RadNetSnowCover-SnowSensF-SnowLatentF-SnowSurfHeatF; 
		}
        else {
           RadNetSnowCover=RadNetGround;
           SnowEbal=RadNetGround-SnowSensF-SnowLatentF-SnowSurfHeatF;
        }
        //! C2.5 Check energybalance closure and decide how to change 
        //!      surface temperatur if it is not sufficient

        //! C2.5.1 if first iteration set change of temperature=+/-1°C
        //!        if Closure>0 increase temp to reduce closure towards 0
        //!        if closure<0 decrease temp to increase closure towards 0
        if(Irep<=1) {
           if(SnowEbal>0) CHANGE=1.;
           if(SnowEbal<0) CHANGE=-1.;
        }
        //! C2.5.2 if Closure > +level of acceptance (i.e. TempSurf too low)	
        if(SnowEbal>5.E4) {
          // ! if closure < 0 in previous iteration:
           //! change direction and reduce temperature change to half
           if(NEG) CHANGE=-1.*CHANGE/2;
           //! Change temperature
           TempSnowSurf+=CHANGE;
           Irep++;
           //! Flag for positive closure
           NEG=false;
           POS=true;
           //! C2.5.3 if Closure < -level of acceptance (i.e. TempSurf too high)
           //!        As above but with opposite signs
		}
        else if(SnowEbal<-5.E4) {
           if(POS) CHANGE=-1.*CHANGE/2;
           TempSnowSurf=TempSnowSurf+CHANGE;
           Irep++;
           NEG=true;
           POS=false;
        }
	} //End do iterative_loop

     if(Irep>=40||abs(SnowEbal)>1.E6) {
        SnowSensF=SnowSensF_Old;
        SnowLatentF=SnowLatentF_Old;
     }

     if(TempSnowSurf>0.) {
        //! *** RECALCULATE FLUXES if TempSnowSurf > 0 C *****			
        TempSnowSurf=0.0;
        //! C2.1 Estimate aerodynamic resistance
      //  RaSnowSurf=NewRaHeatFunctionSnow(TempSnowSurf);
		RaSnowSurf=f_ResAirSnow(WindSpeed, RefHeight, TAir - TempSnowSurf, TAir);
        //! CALCULATE FLUXES OF SENSIBLE AND LATENT HEAT
        SnowSensF=Sens_AirF(TempSnowSurf-TAir,RaSnowSurf);
        VapourPSnowSurf=SVP_David(TempSnowSurf);
        SnowLatentF=Latent_AirF(VapourPSnowSurf-VapourPAir,RaSnowSurf)*LateVapSnow/Fix::LATE;
        //! CALCULATE SNOW SURFACE HEAT FLUX
        if(INDICATOR==0){
           Tbelow=SteadyStateSol(HSnow, KSNOW, p_Soil_WaterF->ThicknessLayers.front()/2., KSOIL, TempSnowSurf,((Soil_HeatF*)p_Soil_HeatF)->Temp.front());
           TempSnowSoil=Tbelow;
        }
        SnowSurfHeatF=m_pModelInfo->SecPerDay*(TempSnowSurf-Tbelow)/Rsnow;
        if(SoilVapour_Sw(m_pModelStructure->SoilVapour)>= SoilVapour_Sw::Soil_and_SnowVapourflow) {
		   ConcSurf=Conc_Vap(VapourPSnowSurf,TempSnowSurf);
           VapourPSoilU=SVP_David(Tbelow);// !*RHPsi(Psi.front(), Tbelow,1.)
           ConcSurf1=Conc_Vap(VapourPSoilU,Tbelow);
           AirP=(1000.-DENSS)/1000.;
           DatMU=D_vap(p_Soil_WaterF->DVapTortuosity,AirP,Tbelow);
           VapGra=(ConcSurf1-ConcSurf)/ZSnow;
           p_Soil_WaterF->VapourFlowSurf=-86.400*DatMU*VapGra;  // Vapour goes directly into the soil !!!!!!
           SnowSurfHeatF=SnowSurfHeatF+LateVapSnow*p_Soil_WaterF->VapourFlowSurf;
        }
//        ! if net radiation not is measured. DG 22/9
        if(NetLongRad_Sw(NetLongRad)== NetLongRad_Sw::Two_Formulas) {
           if(LongRadCanopy_Sw(LongRadCanopy)== LongRadCanopy_Sw::Implicit)
              RadNetSnowCover=(RadNetShort+RadInLong-f_radlongout(TempSnowSurf))*(1-CanopyFracRad);// !BeerLaw(LAI_Total)
           else {
              if(CanopyShade_Sw(CanopyShade)>= CanopyShade_Sw::Function_solar_angle){
                 RadNetShortSnow = RadInShortGround * (1.0-SnowAlbedoVar/100.);
                 RadNetLongSnow = RadInLongGround-f_radlongout(TempSnowSurf);
                 RadNetSnowCover = RadNetShortSnow+RadNetLongSnow;
			  }
              else
                 RadNetSnowCover=RadNetShort*(1-CanopyFracRad)+RadInLongGround-f_radlongout(TempSnowSurf);
 
           }
           SnowEbal=RadNetSnowCover-SnowSensF-SnowLatentF-SnowSurfHeatF ;
		}
        else {
           RadNetSnowCover=RadNetGround;
           SnowEbal=RadNetGround-SnowSensF-SnowLatentF-SnowSurfHeatF;
        }
        
     }
     NEG=false;

     switch(INDICATOR) {
	 case 0:  // ! steady state solution
        QSnowSoil=SnowSurfHeatF;
		TempSnowSoil = SteadyStateSol(HSnow*0.5, KSNOW, p_Soil_WaterF->ThicknessLayers.front() / 2., KSOIL, TempSnow, ((Soil_HeatF*)p_Soil_HeatF)->Temp.front());

		break;
	 case 1:	//! dynamic heat storage in snow pack
        TempSnowSoil=SteadyStateSol(HSnow*0.5, KSNOW, p_Soil_WaterF->ThicknessLayers.front()/2.,KSOIL, TempSnow,((Soil_HeatF*)p_Soil_HeatF)->Temp.front());
        QSnowSoil=m_pModelInfo->SecPerDay*(TempSnow-TempSnowSoil)/Rsnow;
		if(SoilVapour_Sw(m_pModelStructure->SoilVapour)>= SoilVapour_Sw::Soil_and_SnowVapourflow) {
		   ConcSurf=Conc_Vap(VapourPSnowSurf,TempSnow);
           VapourPSoilU=SVP_David(TempSnowSoil);// !*RHPsi(Psi.front(), Tbelow,1.)
           ConcSurf1=Conc_Vap(VapourPSoilU,TempSnowSoil);
           AirP=(1000.-DENSS)/1000.;
           DatMU=D_vap(p_Soil_WaterF->DVapTortuosity,AirP,TempSnowSoil);
           VapGra=(ConcSurf1-ConcSurf)/ZSnow;
           p_Soil_WaterF->VapourFlowSurf=-86.400*DatMU*VapGra;  // Vapour goes directly into the soil !!!!!!
           QSnowSoil=QSnowSoil+LateVapSnow*p_Soil_WaterF->VapourFlowSurf;
        }
		break;
	 }

  }

  if(SWATS>=Temp0Li)
     TempSnowSoil=0.0001;
  
  return SnowEbal;
}


double SnowPack::Actual_Phys_PrecipMelting(double HeatContentSnow) {
	// return the amount of metling because of in mm of water
	//! Assume instant equilibrium between liquid and frozen water
		POTMELTPREC=HeatContentSnow/Fix::LATMELT;
		if(POTMELTPREC>0.)
		  return min(POTMELTPREC,WICE);
		else
		  return-min(-POTMELTPREC,SWATS) ;
}
double SnowPack::Actual_Phys_SurfaceMelting(double QMeltSurface) {
	// Return the amount of water melting at the surface
	POTMELTSURF=TISTEP*QMeltSurface*(1.-FracBare)/Fix::LATMELT;
	return min(WICE, POTMELTSURF);

}
double SnowPack::Actual_Phys_SubSurfaceMelting(double HeatChangeSnow) {			
	POTMELT=HeatChangeSnow/Fix::LATMELT;
	return -min(-POTMELT,SWATS); 
}

bool SnowPack::SnowSurfaceMassBalance() {
// return true to show to All calculation done - No Snow remains
// return false to show that Snow storage is updated and more processing of Snow conditions are needed


//real condensation, sublimation, HSNOW_NEW, DENS_RIME, PotSublimation
	bool flag=false;
//! 2B.3 Surface melting and sublimation
   //! ------------------------------------
   //! a) sublimation/condesation at snow surface
   if(SnowLatentF>0){
		//  ! - sublimation -
		  PotSublimation=TISTEP*SnowLatentF*(1.-FracBare)/LateVapSnow;
		  if(PotSublimation>=WICE){
			 Sublimation=WICE;
			 SnowEvaporation=Sublimation/TISTEP;
			 SnowOutFlow=(WTotSnow-Sublimation)/TISTEP;
			 Reset_SnowPack();
			 return true;
		  }
		  else {
			Sublimation=PotSublimation;
			if(WICE>0.)
				HSnow=HSnow-Sublimation*HSnow/WICE;
			WICE=WICE-Sublimation;
      		WTotSnow=WICE+SWATS;
			SnowEvaporation=Sublimation/TISTEP;
		  }
   }
	else {
		  Condensation=-TISTEP*SnowLatentF*(1.-FracBare)/LateVapSnow;
		  WICE=WICE+Condensation;
		  WTotSnow=WTotSnow+Condensation;
		  //! assume density of rime equals denisty of snow precipitation
		  DENS_RIME=f_NewSnowDensity(1.,min(0.,TAir));
		  HSNOW_NEW=Condensation/DENS_RIME; 
		  HSnow=HSnow+HSNOW_NEW;
		  SnowEvaporation=-Condensation/TISTEP;
	}
	return flag;

} // End subroutine SnowSurfaceMassBalance

double SnowPack::LateSnowFunc() {
	double LIQFRAC;
	if(WTotSnow>0.){
		LIQFRAC=SWATS/WTotSnow;
		if(LIQFRAC>0.02)
			return Fix::LATE;
		else
			return Fix::LATESUB;
	}
	else
		return Fix::LATE;
}





