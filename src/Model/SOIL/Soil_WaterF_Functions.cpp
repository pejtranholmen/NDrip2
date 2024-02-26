#include "Soil_WaterF_Functions.h"
#include "../PLANT/WaterUptake.h"
#include "../Atmosphere/Irrigation.h"
#include "Soil_HeatPump.h"
#include "../Structure/Structure.h"

Soil_WaterF_Functions::Soil_WaterF_Functions(void)
{
}
Soil_WaterF_Functions::~Soil_WaterF_Functions(void)
{
}
bool Soil_WaterF_Functions::Def()
{
	Soil_Drainage::Def();
	ModuleNo=18;
	Sw* pSw;
	pSw = new Sw(&Crack,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,  "Crack",0,"No Bypass|Bypass Flow")  ;
	pSw = new Sw(&InitialWater,SINGLE, WATER, INITF, NOBPROC, NORMAL);
	Define(pSw,"Initial water conditions",0,"Uniform Pressure Head|Unifom Water Content|Uniform flow|Pressure Head(z)|Water Contents(z)","Nitrogen and Carbon < 2") ;
	pSw = new Sw(&Hysteresis,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"Hysteresis",0,"Off|On");
	pSw = new Sw(&ConvectiveGasFlow,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"ConvectiveGasFlow",0,"Off|On","HeatEq >0");
	pSw = new Sw(&TransitTime,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"TransitTime Estimation",0,"Off|On") ;
	pSw = new Sw(&SoilWaterBarrier,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"Soil Water Barrier",0,"Off|On");

	X* pX;
	NE* pNL;
	pNL= m_pModelMap->GetNEPointer("Number of Layers");
	WaterStorage.resize(NL);
	pX= new X(&WaterStorage,pNL, SIMPROFILE,WATER, STORAGE, NOBPROC, NORMAL);
	vector<double> initv;
	initv.resize(NL);
	
	Define(pX,pNL,"WaterStorage", initv,UNIT_TYPES::WATERMASS_UNIT,"");
	T* pT;
	pT= new T(&WaterFlow,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pT, pNL, "Waterflow", initv,UNIT_TYPES::WATERFLOW_UNIT,"");
	G* pG;
	pG= new G(&WBypassFlow,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pG, pNL,"WBypassflow", initv,UNIT_TYPES::WATERFLOW_UNIT,"Crack >= 1");
	pT =new T(&VapourFlow,pNL, SIMPROFILE, WATER, TRANSPORT,NOBPROC, NORMAL);
	Define(pT, pNL ,"Vapourflow", initv ,UNIT_TYPES::WATERFLOW_UNIT, "SoilVapour >= 1");
	Ts *pTs;
	pTs= new Ts(&SurfaceOutFlow, SINGLE, WATER, TRANSPORT,NOBPROC, NORMAL);
	Define(pTs, "SurfaceOutFlow", 0.,UNIT_TYPES::WATERFLOW_UNIT, "");
	pTs= new Ts(&VapourFlowSurf, SINGLE, WATER, TRANSPORT,NOBPROC, NORMAL);
	Define(pTs,"VapourflowSurf",0.,UNIT_TYPES::WATERFLOW_UNIT, "SoilVapour >= 1");


	
//	CALL Ts(Group_18, SoilInfil,"SoilInfil",  DBLE(0.), "mm/day","")
	pG= new G(&Theta,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNL,"WaterContent", initv,"vol %","");
	pG= new G(&ThetaB,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);	
	Define(pG, pNL, "ThetaB",initv,"vol %","WaterEq = 1|Kalle = 1");
 	pG= new G(&Psi,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNL,"PressureHead",initv,"cm water","");
 	pG= new G(&ThetaTotal,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);

	Define(pG, pNL,"TotalWaterContent", initv, "vol %","");
 	pG= new G(&HysEffect, pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);

	Define(pG, pNL, "HysEffect", initv, "-","Hysteresis = 1");
 	pG= new G(&MeanTransitTime,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);

	Define(pG, pNL, "MeanTransitTime",initv,"Days", "TransitTime>0");
	Gs* pGs = new Gs(&TotMeanTransitTime, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
	Define(pGs,"TotMeanTransitTime", 0.,"Days", "TransitTime>0");

	Ps *pPs=new Ps(&DVapTortuosity,  SINGLE, VAPOUR, TRANSPORT, NOBPROC, DETAILED);
	Define(pPs, "DVapTortuosity", 0.66, "-","SoilVapour >= 1", 0.01,4.);
	pPs=new Ps(&IniFlowRate,  SINGLE, VAPOUR, STORAGE, NOBPROC, DETAILED);

	Define(pPs ,"InitialFlowRate", 0.1,"mm/day","Initial water conditions = 2", 1.E-10,10.);
	pPs=new Ps(&IniPsiValue,  SINGLE, WATER, STORAGE, NOBPROC, DETAILED);

	Define(pPs, "InitialPressuredHead", 60.,"cm water","Initial water conditions = 0|Nitrogen and Carbon < 2", 1.,1.E5);
	pPs=new Ps(&IniThetaValue,  SINGLE, WATER, STORAGE, NOBPROC, DETAILED);

	Define(pPs,"InitialWaterContent", 20.,"vol %","Initial water conditions = 1",1.,100.);
	pPs=new Ps(&IniGWLev,  SINGLE, WATER, STORAGE, NOBPROC, DETAILED);

	Define(pPs, "InitialGroundWater", -1.,"m","Instate = 0|GroundWaterFlow > 0",-100.,0.);
	pPs=new Ps(&AScaleSorp,  SINGLE, VAPOUR, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs, "AScaleSorption", 0.5, "-","Crack = 1",0.001,1000.);
	pPs=new Ps(&HysThetaD,  SINGLE, WATER, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs,"HysThetaD", 0.2,"-","Hysteresis = 1",0.01,10.);
	pPs=new Ps(&HysThetaMax,  SINGLE, WATER, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs,"HysThetamax", 10.,"Vol %","Hysteresis = 1",1.,50.);
	pPs=new Ps(&HysPF1,  SINGLE, WATER, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs, "HysPF1", 1.5,"pF-value","Hysteresis = 1", 0.5,4.);
	pPs=new Ps(&HysPF2,  SINGLE, WATER, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs	,"HysPF2", 4.0,"pF-value","Hysteresis = 1",0.5,5.);
	pPs=new Ps(&HysKExp,  SINGLE, WATER, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs,"HysKExp", 0.5,"-","Hysteresis = 1",0.01, 2.0);
					
	pPs=new Ps(&BarrierLevel,  SINGLE, WATER, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs,"BarrierLevel", -0.4,"m","Soil Water Barrier = 1",-100.,-0.01);
	pPs=new Ps(&BarrierEfficiency,  SINGLE, VAPOUR, TRANSPORT, NOBPROC, DETAILED);

	Define(pPs ,"BarrierEfficiency", 0.9,"-","Soil Water Barrier = 1",0.001,1.0);
// vector Parameters	
	P *pP=new P(&IniPsiValues, pNL,SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
	for(auto i=initv.begin(); i!=initv.end();++i) *i=60.;

	Define(pP,pNL,"IniPressureHeads",initv , "cm water","Initial water conditions =3",0.,10000.);
	mTab_InitP.push_back(pP);
	pP=new P(&IniThetaValues, pNL,SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
	for(auto i=initv.begin(); i!=initv.end();++i) *i=10.;
	Define(pP, pNL,"IniWaterContents",initv,"vol %","Initial water conditions =4", 0., 100.);
	mTab_InitTheta.push_back(pP);
	Tab *pTab;
	pTab=new Tab(&mTab_InitP, SIMPROFILE, WATER, INITF, NOBPROC, NORMAL);
	Define(pTab, "InitialWaterPotentials","IniPressureHeads","Initial water conditions = 3");
	pTab=new Tab(&mTab_InitTheta, SIMPROFILE, WATERPOTENTIAL, INITF, NOBPROC, NORMAL);
	Define(pTab, "InitialWaterContents", "IniWaterContents","Initial water conditions = 4");

	pP=new P(&HysMaxEffRet, pNL,SIMPROFILE, WATER, HYSTERESIS, NOBPROC, NORMAL);
 		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;
		Define(pP, pNL,"HysMaxEffRet"	,initv,"-","Hysteresis =1",0,5);
		mTab_Hyster.push_back(pP);
	pP=new P(&HysMaxEffCond, pNL,SIMPROFILE, WATER, HYSTERESIS, NOBPROC, NORMAL);
		Define(pP, pNL,"HysMaxEffCond"	,initv,"-","Hysteresis =1",0,5);
		mTab_Hyster.push_back(pP);
	pTab=new Tab(&mTab_Hyster, SIMPROFILE, WATER, HYSTERESIS, NOBPROC, NORMAL);

	Define(pTab, "Hysteresis Effects", "HysMaxEffRet|HysMaxEffCond","Hysteresis = 1");




//	EPsiFunc(1);
//	double (Hydraulic_Functions::*pPsiFunc)(int, double);
//	pPsiFunc=&Hydraulic_Functions::PsiFunc;
	
	return true;
}

void Soil_WaterF_Functions::ThetaMMM() {

      Theta.front()=WaterStorage.front()/ThicknessLayers.front()/10.;
      for(size_t i=1; i<Theta.size();i++) {
        Theta[i]=WaterStorage[i]/ThicknessLayers[i]/10.;
        ThetaB[i-1]=(ThicknessLayers[i]*Theta[i-1]+ThicknessLayers[i-1]*Theta[i]) 
				/(ThicknessLayers[i]+ThicknessLayers[i-1]);
	  }

}
double Soil_WaterF_Functions::MassAirFuncL(size_t index, double temp) {
	 return max((M_Saturation[index]-ThetaTotal[index])/100.,0.)*ThicknessLayers[index]
					*AirDensityTempFunc(temp);

}
void Soil_WaterF_Functions::Soil_WaterVerticalSaturated_Dyn() {
/*
Use Commonstruc, Only:LayerOfSatLevel, LateralInput, WUptakeRate, Dripper
Use Plant, Only		: NNUP
Use Drainage, Only	: LayerOfDrain, &
					  PhysicalDrainEq, LBoundSaturated, &
					  DeepPerc, TotalDrainage
Use Drivings, Only	: WSource
Use TimeInfo*/
	Irrigation* p_Atm=(Irrigation*)pAtm;
	WaterUptake* p_Plant=(WaterUptake*)pPlant;



double AccDrainFlow, AccInFlow, /*AccWaterUptake,*/ PreviousWFlow;

	//! VERTICAL WATER FLOWS BELOW GROUND WATER LEVEL *******
 
    AccDrainFlow=0.;
	if(LayerOfDrain<WaterDrainFlow.size()) {
		for(size_t i=LayerOfDrain-1; i>=LayerOfSatLevel&&i<WaterDrainFlow.size();i--) {
          AccDrainFlow+=WaterDrainFlow[1+i]+WaterFlow[i];
		  AccDrainFlow+=p_Plant->WUptakeRate[1+i];
	      WaterFlow[i]=AccDrainFlow;
		}
	}

/*!    AccWaterUptake=0.
!	DO  i=NNUP-1, LayerOfSatLevel,-1
!          AccWaterUptake=AccWaterUptake+WUptakeRate(1+i)+WaterFlow[i]
!	      WaterFlow[i]=AccWaterUptake
!	EndDo
*/




//!      Add deep percolation flow to all saturated horizons
    if(LBoundSaturated==1||LBoundSaturated>=2) {
            //DO  i=NL-1,LayerOfSatLevel,-1
			for(size_t i=WaterFlow.size()-2;i>LayerOfSatLevel;i--)
	            WaterFlow[i]+=DeepPerc;
			//EndDo
    }
//!      Add additional contribution from source inflow
	if(m_pModelStructure->LateralInput>=1||p_Atm->Dripper>0) {
			AccInFlow=0;
			//DO i=NL-1, LayerOfSatLevel, -1
			for(size_t i=WaterFlow.size()-1; i>LayerOfSatLevel;i--) {
				AccInFlow+=p_Atm->Wsource[i];
				WaterFlow[i-1]-=AccInFlow;
			}
	}

//! Outflow upward within profile
 	//Do i=2,NL,1
	for(size_t i=1; i<WaterFlow.size(); i++) {
 	   if(Theta[i]-M_Saturation[i]>0.) 
 			 WaterFlow[i-1]-=(Theta[i]-M_Saturation[i])*ThicknessLayers[i]*10.;
 	   
	}

//! Water Barrier converts from vertical to horizontal flow at specific horizon

	if(SoilWaterBarrier>0) {
//VLayers:		Do  i=1, NL
		for(size_t i=0; i<WaterFlow.size()-1; i++) {
				if(abs(BarrierLevel)>=DepthMidpLayer[i]&&abs(BarrierLevel)<DepthMidpLayer[i+1]) {
					PreviousWFlow=WaterFlow[i];
					if(PreviousWFlow>0) {
						WaterFlow[i]=(1-BarrierEfficiency)*PreviousWFlow;
						WaterDrainFlow[i]+=BarrierEfficiency*PreviousWFlow;
					}
					else {
  						WaterFlow[i]=(1-BarrierEfficiency)*PreviousWFlow;
						WaterDrainFlow[i+1]+=BarrierEfficiency*abs(PreviousWFlow);

					}
					TotalDrainage+=BarrierEfficiency*abs(PreviousWFlow);
					break;//exit VLayers
				}
		}
	//EndDo VLayers

	}


	if(Theta.front()-M_Saturation.front()>0) 
		SurfaceOutFlow=(Theta.front()-M_Saturation.front())*ThicknessLayers.front()*10.;
	else
		SurfaceOutFlow=0.;
	

	if(TransitTime) {
		SumTransit=0.;
		SumWaterFlow=0.;
		double SumStorage=0;
		//Do i=1,NL
		for(size_t i=0; i<WaterStorage.size(); i++) {
			SumStorage+=WaterStorage[i];
			AccWaterStorage[i]+=SumStorage*T_Step;
			AccWaterFlow[i]+=(WaterFlow[i]+WaterDrainFlow[i])*T_Step;
			if(i==NL-1) AccWaterFlow[i]+=DeepPerc*T_Step;
			if(AccWaterFlow[i]>0) {
				MeanTransitTime[i]=AccWaterStorage[i]/AccWaterFlow[i];
				SumTransit+=MeanTransitTime[i]*AccWaterFlow[i];
				SumWaterFlow+=AccWaterFlow[i];
			}
		}
		//! David, 2005-11-18, to avoid division by zeero with new compiler
		if(SumWaterFlow>0)
		    TotMeanTransitTime=SumTransit/SumWaterFlow;
		else
		    TotMeanTransitTime=0.0;
		
	}

}
//End Subroutine Soil_WaterVerticalSaturated_Dyn


double Soil_WaterF_Functions::Hys_Eff_Func(size_t i) {


double Shift, AccDerMin, Tpos;
//*************** Hysteresis loop ***************************************
//
       ThetaDer[i]=(Theta[i]-THETAO[i])/T_Step;
       THETAO[i]=Theta[i];
//      Increase of water content above theshold rate (sorption)           
       if(ThetaDer[i]>HysThetaD) {
         Icount[i]++;
///        The first occurence (first indication of sorption)
         if(Icount[i]==1) {
            IcountN[i]=0;
            if(Psi[i]>0) {
//            Shift position function with account for no jumb back
//            if Shift point indicate less hysteresis than age of main
//            loop
             A_PsiSh[i]=max(A_Age[i], min( 1. ,	(log10(Psi[i])-HysPF1)/(HysPF2-HysPF1) ));
              Shift=max(0. , min( 1. ,	(log10(Psi[i])-HysPF1)/(HysPF2-HysPF1) ));
              if(Shift>A_Age[i]) 
                TposShift[i]=m_pModelInfo->T_Current-T_Step;
            }
            else
              A_PsiSh[i]=0.0001;
            
            AccDerMin=HysEffect[i]*HysThetaMax/(A_PsiSh[i]*A_Age[i]+0.000001);
         }
//!C        Accumulation of rate changes and estimation of rate
//!C        change function
         if(Icount[i]>=1) {
           AccDer[i]=max(AccDerMin, AccDer[i]+ThetaDer[i]*T_Step);
           A_Der[i]=min(1., max(0.,  AccDer[i]/HysThetaMax));
         }
	   }
//!C      Indication of desorption 
       else if(ThetaDer[i]<0) {
         IcountN[i]++;//        Indication of stabil Shift to desorption
         if(IcountN[i]==3) {
//!          Reset sortpion counter and and sorption rate change variable 
           Icount[i]=0;
           AccDer[i]=0;
         }
       }
//!      Time since last main loop Shift from desoption to sorption
       Tpos=m_pModelInfo->T_Current-TposShift[i];
       A_Age[i]=exp(-1.*HysKExp*Tpos);
       return A_Age[i]*A_Der[i]*A_PsiSh[i];
}//END Function Hys_Eff_Func
 
 


void Soil_WaterF_Functions::WaterVapourF_Dyn() {

//Use GenFunctions, Only:	RHpsi, Conc_Vap, D_vap, SVP, AirDensityTempFunc
//Use TimeInfo, Only	:T_Step
Soil_HeatPump* p_Heat=(Soil_HeatPump*)pHeat;
vector<double> CONCVAP,DATM;
CONCVAP.resize(NL); DATM.resize(NL);


double VPS, VPSS, AirP, VapGra, Dmean;//	! Local variables
//! Convective Gas Flow
	  if(ConvectiveGasFlow>0) {
		   FluxAir.back()=0.;
		   //DO i= NL-1,1,-1
		   for(size_t i=NL-2; i>0; i--) 	
			   FluxAir[i]=FluxAir[i+1]+(MassAir[i+1]-MassAirFuncL(i+1, p_Heat->Temp[i]))/T_Step;
//		   EndDo
		   FluxAirInfil=FluxAir.front()+(MassAir.front()-MassAirFuncL(1, p_Heat->Temp.front()))/T_Step;
		   //Do i=1, NL	//! Resetting Mass of Air to 
			 for(size_t i=0; i<MassAir.size(); i++)  MassAir[i]=MassAirFuncL(i, p_Heat->Temp[i]);
		   //EndDo
	  }

//Loop_over_layers:  DO i=1,NL
	for(size_t i=0; i<VapourFlow.size(); i++) {
		  VPSS=ModelFunctions::SVP(p_Heat->Temp[i]);
		  VPS=VPSS*RHPsi(Psi[i], p_Heat->Temp[i],1.);//	! 1. by DG 27/7
		  CONCVAP[i]=Conc_Vap(VPS, p_Heat->Temp[i]);//		!   kg/m3
		  AirP=(M_Saturation[i]-ThetaTotal[i])/100.;
		  DATM[i]=D_vap(DVapTortuosity,AirP,p_Heat->Temp[i]);

		  if(i>0) {   // loop_over_layers
				VapGra=(CONCVAP[i]-CONCVAP[i-1])/DistanceMidpLayers[i];// !g/m4
				Dmean=(DATM[i]+DATM[i-1])/2.;
				VapourFlow[i-1]=-86.400*Dmean*VapGra;//   ! Kg/m2/day
				if(ConvectiveGasFlow>0) {
					if(FluxAir[i-1]>0.) 
						 VapourFlow[i-1]=VapourFlow[i-1]+FluxAir[i-1]*CONCVAP[i-1]/(AirDensityTempFunc(p_Heat->Temp[i-1])*1000.);
					else
						 VapourFlow[i-1]=VapourFlow[i-1]+FluxAir[i-1]*CONCVAP[i]/(AirDensityTempFunc(p_Heat->Temp[i])*1000.);
					
				}
				WaterFlow[i-1]=WaterFlow[i-1]+VapourFlow[i-1];
		  }
	}

}

void Soil_WaterF_Functions::ConvPot() {
	WaterUptake* p_Plant=(WaterUptake*)pPlant;
	for(size_t i=0; i<Psi.size(); i++) {
		Psi[i]=-Psi[i];
		if(m_pModelStructure->SaltTracer>0) p_Plant->OsmoticPressure[i]=-p_Plant->OsmoticPressure[i];
		if(p_Plant->PlantWaterStorage>0) p_Plant->PlantWaterPotential[i]=-p_Plant->PlantWaterPotential[i];
	}
}







