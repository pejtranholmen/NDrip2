
#include "SoilEvaporation.h"
#include "../SOIL/Soil_HeatF.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../PLANT_NC/NC_Plant.h"
#include "../../Simulator/Simulator.h"

#include "Functors/SoilEvaporation_Functors.h"
#include "Functors/Radiation_Functors.h"
						   
SoilEvaporation::SoilEvaporation(void)
{
	ModuleName="Soil evaporation";
	ModuleNo=13;
	ModuleConditions="SoilEvaporation>0";
}

bool SoilEvaporation::Ini()	{
//	m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();

	SurfaceCoverSwitch=SurfaceCover;	
	SoilCoverEvapPar=SoilCoverEvap;
	SoilCoverRunoffPar=SoilCoverRunOff;
	TempBareSoil=10.;
    TDiffC=0.;
	VapourPAir=0;;
    Surfmos=0.;
    SurfMos1=0.;
    SurfMos2=0.;
    SoilEbal=0.;
    SoilLatentF=0.;
	SoilEvaporationFlux=0;
    ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow=0.;
	if (!SnowPack::Ini()) return false;

	return true;
}
bool SoilEvaporation::IniFunctors() {

	if (!SnowPack::IniFunctors()) return false;

	if (StabilitySoil_Sw(StabilitySoil) == StabilitySoil_Sw::Richardson_Number)	f_ResAirSoilSurface = Get_D5_Functor(MyFunc::RA_HEATSOIL_ANALYTICAL);
	else f_ResAirSoilSurface = Get_D5_Functor(MyFunc::RA_HEATSOIL_NUMERICAL);
	f_ResNeutralAirSoilSurface = Get_D2_Functor(MyFunc::RA_HEATSOIL_NEUTRAL);
	if (SurEbal_Sw(SurEbal) == SurEbal_Sw::E_balance_Solution) f_RelHumSoilSurface = Get_D3_Functor(MyFunc::REL_HUMIDITY_GAS_EQUILIBRIUM);
	f_TempDiffFromFlux = Get_D2_Functor(MyFunc::TEMPDIFF_FROMSENSIBLEFLOW);
	if (SurEbal_Sw(SurEbal) == SurEbal_Sw::PM_equation || SoilEvap_Sw(SoilEvap) == SoilEvap_Sw::PM_EQ_1PAR || SoilEvap_Sw(SoilEvap) == SoilEvap_Sw::PM_EQ_3PAR) {
		f_PM_EquationSoil = Get_D5_PhysFunctor(FUNC_CHOICE::PM);
		f_RaSoilFromVegCover = Get_D1_Functor(MyFunc::RA_BELOWCANOPYLAI);
	}

	if (SoilEvap_Sw(SoilEvap) == SoilEvap_Sw::PM_EQ_1PAR) f_SoilSurfaceResistance = Get_D2_Functor(MyFunc::SOIL_SURFACE_RES_1P);
	else if (SoilEvap_Sw(SoilEvap) == SoilEvap_Sw::PM_EQ_3PAR) f_SoilSurfaceResistance = Get_D2_Functor(MyFunc::SOIL_SURFACE_RES_3P);
	return true;
}
bool SoilEvaporation::Dyn()
{

	Flux();
	Integ();
	return true;
}

void SoilEvaporation::Flux()
{
	PlantOrgModel* pNCPlant=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	NC_Plant *pNC_Plant=&pNCPlant->m_NC_Plant;
 
  if(FracBare<.001) {
     SoilEvaporationFlux=0.;
     SoilLatentF=0.;
     SoilSensF=0. ;
     PotEvapGround=0.;
  }

  if(SurfaceCover_Sw(SurfaceCover)== SurfaceCover_Sw::PERMANENT_PLASTIC_SHEET)
     MulchInUse=true;
  else	{
     MulchInUse=false;
     for(size_t i=0; i<pNC_Plant->Mulch.size();i++)
       if(pNC_Plant->Mulch[i]>0&&pNC_Plant->GrowthStageIndex[i]>=0.) MulchInUse=true;   
  }

  if(MulchInUse) {
     SoilCoverEvap=SoilCoverEvapPar;
     SoilCoverRunOff=SoilCoverRunoffPar;
  }
  else {
     SoilCoverEvap=0;
     SoilCoverRunOff=0 ;
  }

  if(p_Soil_WaterF->WaterStorage.front()>0.&&p_Soil_WaterF->WaterStorage.front()<1000.) 
    Thetau=p_Soil_WaterF->WaterStorage.front()/p_Soil_WaterF->ThicknessLayers.front()/10.;
  else
    Thetau=p_Soil_WaterF->M_Saturation.front();


  double PsiUpper=p_Soil_WaterF->f_PsiTheta_M(0,Thetau);

  RadNetBareSoil=RadNetGround;

  if(SurEbal_Sw(SurEbal)==SurEbal_Sw::AirTemperature) 
     TempBareSoil=TAir;

  double DiffHeightTemp; 	
  if(CommonRefHeight==1) 
     DiffHeightTemp=RefHeight;
  else
     DiffHeightTemp=RefHeightTemp;

  double RaSoilUncorrected = f_ResNeutralAirSoilSurface(WindSpeed, DiffHeightTemp);

  if(SurEbal_Sw(SurEbal)<=SurEbal_Sw::PM_equation&&SoilEvap_Sw(SoilEvap)>=SoilEvap_Sw::PM_EQ_1PAR) {
     // Aerodynamic resistance above soil surface
     //RaSoilSurf=RaSoilUncorrected*StabilityCorr(RefHeight, Tair-TempBareSoil, Tair, WindSpeed) &
     //	+RaBound(LAI_Total)
     RaSoilSurf=RaSoilUncorrected*f_StabCorrAnalytical(RefHeight-p_Plant->DisplVar, TAir-TempBareSoil, TAir,WindSpeed)
          + f_RaSoilFromVegCover(p_Plant->LAI_Total);
     // Avoid negative energy available for evaporation
     if(RadNetGround<0&&((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow<0) 
         ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow=max(RadNetGround,  ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow);
     else if(((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow>0&&RadNetGround>0)
         ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow=min(RadNetGround,  ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow);
	 else
		 ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow=0.;

     // Soil surface resistance according to 'PM-eq (1par)' or 'PM-eq (3par)' 
     RsSoilSurf=f_SoilSurfaceResistance(PsiUpper, Surfmos);
     // Potential soil evaporation (Rs=0)
     PotEvapGround=PMEquation(TAir, RadNetGround-((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow, VapourPDef, RaSoilSurf, 0.,1.);
	 PotEvapGround = PMEquation(TAir, RadNetGround - ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow, VapourPDef, RaSoilSurf, 0.);
     // Calculate empirical soil evaporation as latent heat flux
     if (SoilEvap==4) 
        // 'K-function' taking the minimum value of Potential soil evaporation and available supply
        SoilLatentF=min(PotEvapGround, p_Soil_WaterF->EvaRateMax(PsiUpper))*Fix::LATE;
	 else  
        // 'PM-Eq (1Par)' or 'PM-Eq (3Par)' formulations
        SoilLatentF=PMEquation(TAir, RadNetGround-((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow, VapourPDef, RaSoilSurf, RsSoilSurf, 1.)
             *Fix::LATE ;
		SoilLatentF = PMEquation(TAir, RadNetGround - ((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow, VapourPDef, RaSoilSurf, RsSoilSurf)*Fix::LATE;

     // Reset Latent heat flux to potential rate if condensation is predicted 
     if(PotEvapGround<0.) SoilLatentF=PotEvapGround*Fix::LATE;
     // Next statement seems to be meaningless??
     // Calculate surface temperature - taking the sensible heat flux 
     // as the residual of the surface energy balance
     if(SurEbal==1) {
        EAvailSurf=RadNetGround-((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow;
        SoilSensF=RadNetGround-SoilLatentF-((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow;
        double TDiff= f_TempDiffFromFlux( SoilSensF, RaSoilSurf);
        // Filter the temperature difference with temperature difference
        // from last time-step
        TDiffC=0.8*TDiffC+0.2*TDiff;
        TempBareSoil=TAir+TDiffC;
        SoilEbal=RadNetGround-SoilLatentF-((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow-SoilSensF;
     }
  }
  else if(SurEbal==2) {	   // Iterative heat balance solution
	 SOILEBAL_OUT ebal_out, ebal_out1, ebal_out2;
	 SOILEBAL_IN ebal_in;
	 ebal_in.SoilEbal=SoilEbal;
     if(SoilPartitioningArea==0) {
		ebal_in.TotalCoverDegree=0; ebal_in.AreaFraction=1.;
		ebal_in.SurfaceMoistureDiff = Surfmos;
        if(CanopyShade>=1)
			ebal_out=SurfEvalSolution(ebal_in, ONE);	
        else 
			ebal_out=SurfEvalSolution(ebal_in, ONE);
	 } 
	 else {
		ebal_in.AreaFraction=DripIrrigCover;
		ebal_in.TotalCoverDegree=min(DripIrrigCover, SoilCoverEvap);
        // Area1 = drip irrigated area
        Fraction1 = DripIrrigCover;
      //  CoverDegree1=min(DripIrrigCover, SoilCoverEvap)
       // CoverDegree2=max(SoilCoverEvap-CoverDegree1,0.)

        // Explicitly distributed radiation?
        if(SoilPartitioningArea==2){

           // Identical INPUT for area 1 and area 2:
           //		RadNetShort, RadInLong
           //		PsiUpper
           // Different INPUT for area 1 and 2:
           //		RadInLongGround
           //		RadNetGround (may be and input or an output depending on MISSRNT)
           //		CanopyFracRad
           //       Fraction (fractional soil coverage of area 1 and 2)
           //       SurfMos

           // Different OUTPUT for area 1 and 2:
           //		SoilEbal
           //		RadNetBareSoil
           //		SoilSensF
           //		SoilLatentF
           //		SurfHeatFlow
           //		TempBareSoil
           //       RaSoilSurf
			ebal_in.SurfaceMoistureDiff = SurfMos1;
		   ebal_out1=SurfEvalSolution(ebal_in, SPLIT);	
		   ebal_in.TotalCoverDegree=SoilCoverEvap-ebal_in.TotalCoverDegree;
		   ebal_in.SurfaceMoistureDiff = SurfMos2;
		   ebal_out2=SurfEvalSolution(ebal_in, SPLIT);	
       /*    Call SurfaceHeatBalance(SoilEbal1, SurfMos1, PsiUpper, RaSoilSurf1, RadNetBareSoil1, & 
                SoilSensF1, SoilLatentF1,SurfHeatFlow1, Fraction1, &
                RadNetGround1, RadNetShort, RadInLong, CanopyFracRad1, RadInLongGround1, &
                TempbareSoil1, CoverDegree1)

           Call SurfaceHeatBalance(SoilEbal2, SurfMos2, PsiUpper, RaSoilSurf2, RadNetBareSoil2, & 
                SoilSensF2, SoilLatentF2,SurfHeatFlow2, 1-Fraction1, &
                RadNetGround2, RadNetShort, RadInLong, CanopyFracRad2, RadInLongGround2, &
                TempbareSoil2, CoverDegree2)*/
		}
        else {
           // "Unpartitioned" radiation for the two surfaces

           // Identical INPUT for area 1 and area 2:
           //		RadNetShort, RadInLong
           //		PsiUpper
           //		RadInLongGround
           //		RadNetGround (may be and input or an output depending on MISSRNT)
           //		CanopyFracRad
           // Different INPUT for area 1 and 2:
           //       Fraction (fractional soil coverage of area 1 and 2)
           //       SurfMos

           // Different OUTPUT for area 1 and 2:
           //		SoilEbal
           //		RadNetBareSoil (this will be different when MISSRNT
           //						since TempBareSoil is always different)
           //		SoilSensF
           //		SoilLatentF
           //		SurfHeatFlow
           //		TempBareSoil
           //       RaSoilSurf
			ebal_in.SurfaceMoistureDiff = SurfMos1;
		   ebal_out1=SurfEvalSolution(ebal_in, SPLIT);
		   ebal_in.TotalCoverDegree=SoilCoverEvap-ebal_in.TotalCoverDegree;
		   ebal_in.SurfaceMoistureDiff = SurfMos2;
		   ebal_out2=SurfEvalSolution(ebal_in, SPLIT);
     /*      Call SurfaceHeatBalance(SoilEbal1, SurfMos1, PsiUpper, RaSoilSurf1, RadNetBareSoil1, & 
                SoilSensF1, SoilLatentF1,SurfHeatFlow1, Fraction1, &
                RadNetGround, RadNetShort, RadInLong, CanopyFracRad, RadInLongGround, &
                TempbareSoil1, CoverDegree1)

           Call SurfaceHeatBalance(SoilEbal2, SurfMos2, PsiUpper, RaSoilSurf2, RadNetBareSoil2, &
                SoilSensF2, SoilLatentF2,SurfHeatFlow2, 1-Fraction1, &
                RadNetGround, RadNetShort, RadInLong, CanopyFracRad, RadInLongGround, &
                TempbareSoil2, CoverDegree2)*/
        }
        if(ebal_in.AreaFraction>=0. && Fraction1<=1.) {

			ebal_out.SoilEbal				=Fraction1*ebal_out1.SoilEbal+(1-Fraction1)*ebal_out2.SoilEbal;
			ebal_out.Surface_VapourPressure = Fraction1*ebal_out1.Surface_VapourPressure + (1 - Fraction1)*ebal_out2.Surface_VapourPressure;
            ebal_out.RaSoilSurf				=Fraction1*ebal_out1.RaSoilSurf+(1-Fraction1)*ebal_out2.RaSoilSurf;
			ebal_out.RadNet			=Fraction1*ebal_out1.RadNet+(1-Fraction1)*ebal_out2.RadNet;
            ebal_out.SensibleF				=Fraction1*ebal_out1.SensibleF+	(1-Fraction1)*ebal_out2.SensibleF;
            ebal_out.SoilSurfHeatFlow			=Fraction1*ebal_out1.SoilSurfHeatFlow+(1-Fraction1)*ebal_out2.SoilSurfHeatFlow;
            ebal_out.LatentF				=Fraction1*ebal_out1.LatentF+(1-Fraction1)*ebal_out2.LatentF;
            // To be precise for the longwave balance, TempBareSoil**4 should be averaged: 
            //deleted bug May 2022, using longwave radiation instead of temperature raised by 4.
			ebal_out.Surface_Temp			=pow(Fraction1*pow(ebal_out1.Surface_Temp+273.15,4.0) +(1-Fraction1)*pow(ebal_out2.Surface_Temp+273.15,4.), 0.25)-273.15;
		}
		else {
			ebal_out.SoilEbal = ebal_out1.SoilEbal;
			ebal_out.Surface_VapourPressure = ebal_out1.Surface_VapourPressure;
			ebal_out.RaSoilSurf = ebal_out1.RaSoilSurf ;
			ebal_out.RadNet = ebal_out1.RadNet ;
			ebal_out.SensibleF = ebal_out1.SensibleF ;
			ebal_out.SoilSurfHeatFlow = ebal_out1.SoilSurfHeatFlow ;
			ebal_out.LatentF = ebal_out1.LatentF;
			ebal_out.Surface_Temp = ebal_out1.Surface_Temp;
		}
        SoilEBal1 = ebal_out1.SoilEbal;
        SoilEBal2 = ebal_out2.SoilEbal;
        RadNetBareSoil1 = ebal_out1.RadNet;
        RadNetBareSoil2 = ebal_out2.RadNet;
         RadNetBareSoil1 = ebal_out1.RadNet;
         RadNetBareSoil2 = ebal_out2.RadNet;
         RaSoilSurf1 = ebal_out1.RaSoilSurf;
         RaSoilSurf2 = ebal_out2.RaSoilSurf;
		 SoilLatentF1=ebal_out1.LatentF;
		 SoilLatentF2=ebal_out2.LatentF;
         SoilSensF1 = ebal_out1.SensibleF;
         SoilSensF2 = ebal_out2.SensibleF;
		 SoilEvaporation1=SoilLatentF1/Fix::LATE;
         SoilEvaporation2 = SoilLatentF2 /Fix::LATE;
         TempBareSoil1 = ebal_out1.Surface_Temp;
         TempBareSoil2 = ebal_out2.Surface_Temp;
         SurfHeatFlow1 = ebal_out1.SoilSurfHeatFlow;
         SurfHeatFlow2 = ebal_out2.SoilSurfHeatFlow;
     }
	 if(ebal_out.LatentF>-5.E7&&ebal_out.LatentF<1.E8) 
		   SoilLatentF=ebal_out.LatentF;
	  else
		   SoilLatentF=0.;

	 	RaSoilSurf=ebal_out.RaSoilSurf;
		TempBareSoil=ebal_out.Surface_Temp;
		VapourPSurf = ebal_out.Surface_VapourPressure;
		SoilSensF=ebal_out.SensibleF;
		((Soil_HeatF*)p_Soil_HeatF)->SurfHeatFlow=ebal_out.SoilSurfHeatFlow;
		if(abs(ebal_out.SoilEbal)<1.E9)
			SoilEbal=ebal_out.SoilEbal;
		else
			SoilEbal=0.;

     }
	
	 SoilEvaporationFlux=max(-MaxSoilCondens, SoilLatentF/Fix::LATE)*FracBare;
	 PotEvapGround=max(-MaxSoilCondens, PotEvapGround)*FracBare;
	
  //c changed maximal evaporation rate limitation 931128 to be
  //c 50 % of storage  /PEJ

	if(SoilEvaporationFlux>-2.&&SoilEvaporationFlux<100.) 
		 SoilEvaporationFlux=min(max(0., 0.1*(p_Soil_WaterF->WaterStorage.front()-p_Plant->WUptakeRate.front()*T_Step)/T_Step), SoilEvaporationFlux);
	else
		SoilEvaporationFlux=0.;
}
void SoilEvaporation::Integ()
{

	 Surfmos=Surfmos-SoilEvaporationFlux*T_Step+SoilInfil*T_Step-p_Soil_WaterF->VapourFlowSurf*T_Step+SurfacePool+Wsource.front()*T_Step; //Added SurfacePool 8/10 2004 and Wsource 17/10
	 Surfmos=max(MaxSurfDeficit, min(MaxSurfExcess, Surfmos) );
	 if(SoilPartitioningArea>0) {
	
		  SurfMos1=SurfMos1-SoilEvaporation1*T_Step+SoilInfil*T_Step-p_Soil_WaterF->VapourFlowSurf*T_Step+SurfacePool+Wsource.front()/Fraction1*T_Step; // !Added SurfacePool 8/10 2004
		  SurfMos1=max(MaxSurfDeficit, min(MaxSurfExcess, SurfMos1) );
		  SurfMos2=SurfMos2-SoilEvaporation2*T_Step+SoilInfil*T_Step-p_Soil_WaterF->VapourFlowSurf*T_Step+SurfacePool; // !Added SurfacePool 8/10 2004
          SurfMos2=max(MaxSurfDeficit, min(MaxSurfExcess, SurfMos2) );
	  }


	 OutFlow(p_Soil_WaterF->WaterStorage.front(), SoilEvaporationFlux);
}

bool SoilEvaporation::End()
{
	SnowPack::End();

	return true;
}
SoilEvaporation::~SoilEvaporation(void)
{
}
SOILEBAL_OUT SoilEvaporation::SurfEvalSolution(SOILEBAL_IN Input, enum soil_surfacetype choice)
{
 SOILEBAL_OUT out;

 double Rsoil=ThCond_R20(p_Soil_WaterF->Theta.front(), ((Soil_HeatF*)p_Soil_HeatF)->ThQual.front());

 double CoverFraction=0;
 if(Input.AreaFraction>0) CoverFraction=Input.TotalCoverDegree/Input.AreaFraction;
 Input.SoilEbal=1.E6;
 int Irep=1;
 bool POS,NEG;
 POS=NEG=false;

 out.Surface_Temp=((Soil_HeatF*)p_Soil_HeatF)->Temp.front();

 double Change;
 while(	abs(Input.SoilEbal)>1.E5 && Irep<40) {
	 //1  L

	 out.RaSoilSurf = f_ResAirSoilSurface(RefHeight, TAir-out.Surface_Temp, TAir, WindSpeed, p_Plant->LAI_Total);

	 //      ! 2 Soil surface vapour pressure
	 out.Surface_VapourPressure = f_svp(out.Surface_Temp)*f_RelHumSoilSurface(p_Soil_WaterF->Psi.front(), out.Surface_Temp, Input.SurfaceMoistureDiff);
	 //! 3 Latent heat flux from soil surface
	 out.LatentF = f_LatentFlowByResistance(out.Surface_VapourPressure - VapourPAir, out.RaSoilSurf)*(1 - CoverFraction);
	 //! 4 Sensible heat flux from soil surface
	 out.SensibleF = f_SensibleFlowByResistance(out.Surface_Temp - TAir, out.RaSoilSurf);

	 //! 5 Soil surface heat flux

     if(m_pModelStructure->TimeResolution==1) 
        out.SoilSurfHeatFlow=Fix::SECPERDAY*(out.Surface_Temp-((Soil_HeatF*)p_Soil_HeatF)->Temp.front())/Rsoil;
     else
        out.SoilSurfHeatFlow=Fix::SECPERDAY*ThCond_Upper(Thetau,((Soil_HeatF*)p_Soil_HeatF)->ThQual.front())*(out.Surface_Temp-((Soil_HeatF*)p_Soil_HeatF)->Temp.front())/(p_Soil_WaterF->ThicknessLayers.front()/2.)*((Soil_HeatF*)p_Soil_HeatF)->SurfFlowDamp(out.Surface_Temp);
	 double VapourFlowSurf;
     if(m_pModelStructure->SoilVapour==1||m_pModelStructure->SoilVapour==2) {
	    double VapourFlowSurf=((Soil_HeatF*)p_Soil_HeatF)->SurfaceVapourFlow(out.Surface_Temp);
        out.SoilSurfHeatFlow=out.SoilSurfHeatFlow+Fix::LATE*VapourFlowSurf;//		!C Energy flow
	 }
	 else
        VapourFlowSurf=0.;


	 
//     // if net radiation not is measured. DG 22/9
     if(NetLongRad_Sw(NetLongRad)==NetLongRad_Sw::Two_Formulas) {
        if(LongRadCanopy_Sw(LongRadCanopy)==LongRadCanopy_Sw::Implicit)
           out.RadNet=(RadNetShort+RadInLong-f_radlongout(out.Surface_Temp))*(1-CanopyFracRad); 
        else {
           if(CanopyShade_Sw(CanopyShade)>=CanopyShade_Sw::Function_solar_angle){
              RadNetShortBareSoil = RadInShortGround*(1.0-SoilAlbedoVar/100.);
              RadNetLongBareSoil = RadInLongGround-f_radlongout(out.Surface_Temp);
              out.RadNet=RadNetShortBareSoil+RadNetLongBareSoil;
		   }
           else
              out.RadNet=RadNetShort*(1-CanopyFracRad)+RadInLongGround-f_radlongout(out.Surface_Temp);          
        }
        Input.SoilEbal=out.RadNet-out.SensibleF-out.LatentF-out.SoilSurfHeatFlow;
	 }
     else {
		 out.RadNet=RadNetGround;
		 Input.SoilEbal=out.RadNet-out.SensibleF-out.LatentF-out.SoilSurfHeatFlow;
     }

     if(Irep<=3) {
        if(Input.SoilEbal>0.) Change=0.5;
        if(Input.SoilEbal<0.) Change=-0.5;
     }
     if(Input.SoilEbal>5.E4) {
        if(NEG) Change=-1.*Change/2	;
        out.Surface_Temp=out.Surface_Temp+Change;
        Irep++;
        NEG=false;
        POS=true;
	  }
     else if(Input.SoilEbal<-5.E4) {
        if(POS) Change=-1.*Change/2;
        out.Surface_Temp=out.Surface_Temp+Change;
        Irep++;
        NEG=true;
        POS=false;
     }
}
 if(Irep==40) {
	 out.SoilEbal=0;
	 out.Surface_Temp=TAir;
	 out.SoilSurfHeatFlow=0;
	 out.SensibleF=0;
	 out.LatentF=0;


 }
 else {
	out.SoilEbal=Input.SoilEbal;
 }
 return out;

}

