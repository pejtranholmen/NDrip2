#include "Soil_HeatF.h"
#include "../../Simulator/Simulator.h"
#include "../Structure/Structure.h"
#include "../PLANT/Salt.h"

						   
Soil_HeatF::Soil_HeatF(void)
{
	ModuleName="Soil heat flows";
	ModuleNo=16;
	ModuleConditions="HeatEq > 0";
}

bool Soil_HeatF::Ini()	{


	SoilOrgModel* pSoilOrg = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	p_NC_Soil = &pSoilOrg->m_NC_SoilOrg;





//	m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	function<void(vector<double>*, vector<double>*, vector<double>*)> f_ThCond;
	f_ThCond=GetVectorPointer_D3_Functor(MyFunc::THERMALCOND_KERSTEN);
	f_ThCond(&ThCondV, &p_Soil_WaterF->Theta, &ThQual);

	if (ThCondFunction_Sw(ThCondFunction) == ThCondFunction_Sw::Balland_Arp)
		f_ThermalCond = Get_D2_TableFunctor(MyFunc::THERMALCOND_MODEL_BALLAND_ARP);
	else
		f_ThermalCond = Get_D2_TableFunctor(MyFunc::THERMALCOND_KERSTEN);
	
	Thermal_ReCalc();
	DAMPDEPTH=0.1;
	T_Step=*pT_Step;
	m_SurfHeatFlowSnow=0.;



	 
//!C*************** DAMPING DEPTH FOR ANALYTICAL SOLUTION ****************

	if(p_Atm->TairCycle==0) {
		DAMPDEPTH=DampDepthDay(p_Soil_WaterF->Theta.front());
		Ycycle=1.;
	}
	else {
		DAMPDEPTH=DampDepthYear(p_Soil_WaterF->Theta.front());
		Ycycle=365.25;
	}
    
	if(!m_pModelInfo->StartStateValue) {


		for(size_t i=0; i<SoilHeat.size();i++) {
          if(InitialHeat==3)
			SoilHeat[i]=SoilHeatInitial[i];
		  else {
			double Itemps;
			if(InitialHeat==0)
				Itemps=SoilInitTemp;
			else if(InitialHeat==1) 
                Itemps=SoilInitTempVar[i]; 
			else if(InitialHeat==2) 
				Itemps=SinTemp(p_Atm->TAirMean, p_Atm->TAirAmpl, p_Atm->TAirPhase, Ycycle, DAMPDEPTH, m_pModelInfo->T_Current, m_pModelInfo->JDayNum, p_Soil_WaterF->DepthMidpLayer[i]); 

            
            Temp[i]=Itemps;
			SoilHeat[i]=SoilHeat_TempF(i, Itemps, p_Soil_WaterF->WaterStorage[i]);

		  }
          
          ThCondV[i]=f_ThermalCond(i,p_Soil_WaterF->Theta[i],0.);
		}

		if(p_Soil_WaterF->ConvectiveGasFlow!=0) {
			for(size_t i=0; i<p_Soil_WaterF->MassAir.size();i++)
				p_Soil_WaterF->MassAir[i]=max(0.,(p_Soil_WaterF->M_Saturation[i]-p_Soil_WaterF->Theta[i])/100.*p_Soil_WaterF->ThicknessLayers[i]*AirDensityTempFunc(Temp[i]));
		
		}

	}

    AgeofSourceAtStart=m_pModelInfo->T_Current;
    if(HeatSource==5)     
        AgeofSourceAtStart=m_pModelInfo->T_Current+HeatCableDelay*365.25;
    
    if(InsulatedWaterPipe>0) {
    
        WaterPipeVolume=.02*0.02*Fix::PI*PipeSegments.front();
        HeatCapacityPipeWater=WaterPipeVolume*Fix::DENSW*Fix::SPECW;
            
        for(size_t i=0; i<WaterPipeLayer.size();i++) {
            auto IntegerWaterPipeLayer=size_t(WaterPipeLayer[i]);   
            HeatBoxStorage[i]=(HeatCapSolidDist[IntegerWaterPipeLayer]*0.5*0.1+.015*4.2E6)*Temp[IntegerWaterPipeLayer];
		}

		double Itemps=0.;
        for(size_t i=0; i<WaterPipeLayer.size();i++) {
            auto ISEG=size_t(float(i)/5.);
            IntegerWaterPipeLayer=size_t(WaterPipeLayer[ISEG]);   
            Itemps=Temp[IntegerWaterPipeLayer];
            ColdPipeHeat[i]=HeatCapacityPipeWater*Itemps;
            OutPipeHeat[i]=HeatCapacityPipeWater*Itemps;
		}

        SewageWaterStore=10.;
        SewageHeatStore=SewageWaterStore*Fix::SPECW*Itemps;
        TravelDistance=0.0;
        EmptyContainer=false;
        HeatBoxEnhancer=1.;

	}

	return true;
}
bool Soil_HeatF::IniFunctors() {

	if(ThCondFunction_Sw(ThCondFunction)==ThCondFunction_Sw::Balland_Arp)
		f_ThermalCond = Get_D2_TableFunctor(MyFunc::THERMALCOND_MODEL_BALLAND_ARP);
	else
		f_ThermalCond = Get_D2_TableFunctor(MyFunc::THERMALCOND_KERSTEN);
	return true;
}
bool Soil_HeatF::Dyn()
{
	if (m_pModelStructure->HeatEq!=0) {
		Flux();
		Integ();
	}
	return true;
}

void Soil_HeatF::Flux()
{
	LowerBoundary();
	ProfileFlux();
	if(SoilHeatConv>0)	HeatWaterFlow();
	if(HeatVapourF>0) HeatVapourFlow();
	UpperBoundary();
}
void Soil_HeatF::Integ()
{
	//double *pDouble=
	InFlow(SoilHeat[0], SurfHeatFlow);    
    IntFlow(SoilHeat, SoilHeatFlow);
    if(HeatLowerB==1) 
        OutFlow(SoilHeat.back(), SoilHeatFlow.back());
	if(HeatSource>0) 
            InFlow(SoilHeat, SoilHeatSource);
	if(SoilHeatConv==1&&m_pModelStructure->GroundWaterFlow)
            OutFlow(SoilHeat, SoilHeatSink);
	else if(p_Atm->Dripper>0)
			OutFlow(SoilHeat, SoilHeatSink);

	if(InsulatedWaterPipe>0) {
		InFlow(HeatBoxStorage, HeatBoxBoundaryFlux);
	    auto Ilayer=size_t(WaterPipeLayer.front());
	    OutFlow(SoilHeat[Ilayer], HeatBoxReturnFlow);
	    
	    if(InsulatedWaterPipe>=2) 	    
	            InFlow(HeatBoxStorage, HeatBoxHeaterFlux);
	    
			for(size_t ii=0; ii<HeatBoxStorage.size(); ii++) {
				for(size_t iseg=0;iseg<ColdPipeSinkFlux.size();iseg++) {
				   InFlow(HeatBoxStorage[ii], ColdPipeSinkFlux[iseg]);
				   InFlow(HeatBoxStorage[ii], OutPipeSinkFlux[iseg]);
				}
			}
	    
	    if(WaterPipeBox>0) {
	        InFlow(SewageHeatStore, SewageInflow);
	        OutFlow(SewageHeatStore, SewageOutflow);
	        
	        OutFlow(SewageHeatStore, SewageSinkFlux);
	        
	        InFlow(SewageWaterStore, SewageWaterIn);
	        OutFlow(SewageWaterStore, SewageWaterOut);
	        
	      //  ! Conductive losses along incoming and outgoing pipes
	        OutFlow(ColdPipeHeat, ColdPipeSinkFlux);
	        OutFlow(OutPipeHeat, OutPipeSinkFlux);
	        
	    //  ! Fluxes along incomming pipe
	        InFlow(ColdPipeHeat.front(), ColdPipeInFlux);
	        IntFlow(ColdPipeHeat, ColdPipeFlux);
	        OutFlow(ColdPipeHeat.back(), ColdPipeFlux.back());
		}
	}
}

bool Soil_HeatF::End()
{
return true;
}
Soil_HeatF::~Soil_HeatF(void)
{
}
void Soil_HeatF::LowerBoundary()
{
	double	Tbound;
	if(HeatLowerB==0) {
        Tbound=SinTemp(p_Atm->TAirMean, p_Atm->TAirAmpl, p_Atm->TAirPhase, Ycycle, DAMPDEPTH, m_pModelInfo->T_Current, m_pModelInfo->JDayNum, p_Soil_WaterF->DepthLayers.back());
		SoilHeat.back()=SoilHeat_TempF(SoilHeat.size()-1, Tbound, p_Soil_WaterF->WaterStorage.back());
        SoilHeatFlow.back()=0.;
	}
	else
		SoilHeatFlow.back()=GeothermalF;
	
	if(Analytical_Solution==1){
	   for(size_t i=0; i<ExactTemperature.size();i++)
       ExactTemperature[i]=SinTemp(p_Atm->TAirMean, p_Atm->TAirAmpl, p_Atm->TAirPhase, Ycycle, DAMPDEPTH, m_pModelInfo->T_Current, m_pModelInfo->JDayNum,
			p_Soil_WaterF->DepthMidpLayer[i]);  
	}

}
void Soil_HeatF::UpperBoundary()
{
	double AdjustNetRadGround;
	if(p_Atm->TSurfInput!=1) {
		//if(p_Atm->SurEbal>1)

		if(p_Atm->SurEbal>1)
			if(abs(p_Atm->TempBareSoil)<70.)
				TempSoilSurf=p_Atm->TempBareSoil;
			else {
				TempSoilSurf=p_Atm->TAir;
				if(abs(p_Atm->TempSnowSoil)<50) 
				;
				else
					p_Atm->TempSnowSoil=p_Atm->TAir;
				
				//TempSoilSurf=p_Atm->TempBareSoil;

			}
		else { //! Without iterative energy balance
			double TCONDU=f_ThermalCond(0,p_Soil_WaterF->Theta.front(),ThQual.front());
			if(m_pModelStructure->WaterEq==0) 
				TempSoilSurf=p_Atm->TAir;
			else
				TempSoilSurf=p_Atm->TempBareSoil;
		
			SurfHeatFlow=86400.*TCONDU*(TempSoilSurf-Temp.front())/(p_Soil_WaterF->ThicknessLayers.front()/2.);
			if(HeatVapourF>=1) {
				p_Soil_WaterF->VapourFlowSurf=SurfaceVapourFlow(TempSoilSurf);	//	!C Vapour flow
		   		SurfHeatFlow+=Fix::LATE*p_Soil_WaterF->VapourFlowSurf;		//!C Energy flow
			}
		}
		if(p_Atm->SurEbal==1) {
			if(p_Atm->RadNetGround<=0)
					SurfHeatFlow=max(p_Atm->RadNetGround, SurfHeatFlow);
			else 
					SurfHeatFlow=min(p_Atm->RadNetGround, SurfHeatFlow);	     
		}

//! 2. SurfHeatFlux from snow covered area
		if(p_Atm->FracBare<0.99) { //  !Snow is present
			double SurfHeatFlowSnow;
			double VapourFlowSnowSurf=0.;
			if(m_pModelStructure->TimeResolution==1) {

				if (p_Atm->TempSnowSoil > Temp.front() && p_Atm->TempSnowSoil >p_Atm->TempSnowSurf&&p_Atm->SWATS<p_Atm->Temp0Li) {

					p_Atm->TempSnowSoil = p_Atm->SnowSoilTemp_Function();// max(Temp.front(), p_Atm->TempSnowSurf);

				}
				double Rsoil=p_Atm->ThCond_R20(p_Soil_WaterF->Theta.front(), ThQual.front());
				SurfHeatFlowSnow=86400.*(p_Atm->TempSnowSoil-Temp.front())/Rsoil;
				// Removed damping of SurfheatFlowbeneath Snow
			/*	double Frac = 0.01, ActualEst;
				if (p_Atm->WTotSnow > 50.&&SurfHeatFlowSnow > p_Atm->SnowSurfHeatF&&p_Atm->SnowSurfHeatF<0.) {
					if (p_Atm->TAir < Temp.front())
						ActualEst = min(0., min(SurfHeatFlowSnow, SoilHeatFlow[1]));
					else
						ActualEst = min(SurfHeatFlowSnow, SoilHeatFlow[1]);


					m_SurfHeatFlowSnow = m_SurfHeatFlowSnow*(1 - Frac) + ActualEst*Frac;
					SurfHeatFlowSnow = m_SurfHeatFlowSnow;
				}
				else if (p_Atm->WTotSnow > 50.&&SurfHeatFlowSnow > 0.) {
					if (p_Atm->TAir < Temp.front())
						ActualEst = min(min(0., p_Atm->SnowSurfHeatF), min(SurfHeatFlowSnow, SoilHeatFlow[1]));
					else
						ActualEst = min(p_Atm->SnowSurfHeatF, min(SoilHeatFlow[1], SurfHeatFlowSnow));

					m_SurfHeatFlowSnow = m_SurfHeatFlowSnow*(1 - Frac) + ActualEst*Frac;
					SurfHeatFlowSnow = m_SurfHeatFlowSnow;
				}
				else
					m_SurfHeatFlowSnow = SurfHeatFlowSnow;*/

			}
			else {
				double TCONDU=f_ThermalCond(1,p_Soil_WaterF->Theta.front(),ThQual.front());
		 		SurfHeatFlowSnow=86400.*TCONDU*(p_Atm->TempSnowSoil-Temp.front())/(p_Soil_WaterF->ThicknessLayers.front()/2.);
			}
			if(HeatVapourF>=1) {
				VapourFlowSnowSurf=SurfaceVapourFlow(p_Atm->TempSnowSoil);
		   		SurfHeatFlowSnow+=Fix::LATE*VapourFlowSnowSurf;//
			}
			SurfHeatFlow=(1.-p_Atm->FracBare)*SurfHeatFlowSnow+p_Atm->FracBare*SurfHeatFlow;
			TempSoilSurf=(1.-p_Atm->FracBare)*p_Atm->TempSnowSoil+p_Atm->FracBare*TempSoilSurf;

			if (SurfHeatFlow > 1.E6 && p_Atm->TAir < -10) {

				int koll = 0;

			}
			p_Soil_WaterF->VapourFlowSurf=(1.-p_Atm->FracBare)*VapourFlowSnowSurf+p_Soil_WaterF->VapourFlowSurf*p_Atm->FracBare;
	//! 3. Summary surface fluxes
			if(p_Atm->SnowSurEbal==1&&p_Atm->SurEbal==2) {
				TotalGroundSensFlow=   p_Atm->FracBare*p_Atm->SoilSensF+(1-p_Atm->FracBare)*p_Atm->SnowSensF;//	    ! +sensibel flow from veg?
				TotalGroundLatFlow=  p_Atm->FracBare*p_Atm->SoilLatentF+(1-p_Atm->FracBare)*p_Atm->SnowLatentF;//	  ! +Latent flow from veg?
				if(p_Atm->MISSRNT)	
					AdjustNetRadGround=p_Atm->RadNetGround-(p_Atm->FracBare*p_Atm->RadNetBareSoil+(1-p_Atm->FracBare)*p_Atm->RadNetSnowCover);
			
			}
			else
				AdjustNetRadGround=0.;
	
		
		}
		else { //  ! No snow present
			if(p_Atm->SurEbal==2) {
				TotalGroundSensFlow=  p_Atm->SoilSensF;
				TotalGroundLatFlow=p_Atm->SoilLatentF;
				if(p_Atm->MISSRNT)
				AdjustNetRadGround=p_Atm->RadNetGround-p_Atm->RadNetBareSoil;

			}
			else
				AdjustNetRadGround=0.;
		}
		//!   Adjusting NetRadiation because of surface temperature of ground
		//!   When net radiation is estimated

		if(p_Atm->MISSRNT)	{
			p_Atm->RadNetGround=p_Atm->RadNetGround-AdjustNetRadGround;
			p_Atm->RadNetTot=p_Atm->RadNetTot-AdjustNetRadGround;
		}
		
	}
	else { //! 4. SurfHeatFlux from input TempSoilSurf
		double TCONDU=f_ThermalCond(0,p_Soil_WaterF->Theta.front(),ThQual.front());
		SurfHeatFlow=86400.*TCONDU*(TempSoilSurf-Temp.front())/(p_Soil_WaterF->ThicknessLayers.front()/2.);
		if(HeatVapourF>=1) {
			p_Soil_WaterF->VapourFlowSurf=SurfaceVapourFlow(TempSoilSurf);	//	!C Vapour flow
		   	SurfHeatFlow+=Fix::LATE*p_Soil_WaterF->VapourFlowSurf;		//!C Energy flow
		}

		if(p_Atm->SurEbal==1) {
			if(p_Atm->RadNetGround<=0) 
				SurfHeatFlow=max(p_Atm->RadNetGround, SurfHeatFlow);
			else
				SurfHeatFlow=min(p_Atm->RadNetGround, SurfHeatFlow);
		}

	}
}
void Soil_HeatF::ProfileFlux()
{

	p_Soil_WaterF->Frost=false;
	p_Soil_WaterF->NL_Frost=-1;
//LOOP_OVER_LaYERS:  DO I=1,NCALC
	for(size_t i=0; i<SoilHeat.size()&&i<p_Soil_WaterF->NCalc; i++) {

		if(SoilHeat[i]<-1.E-20) {
		  p_Soil_WaterF->Frost=true;
		  p_Soil_WaterF->NL_Frost=i;
		}

		FrozenCond cond=Temp_SoilHeatF(i, SoilHeat[i], p_Soil_WaterF->WaterStorage[i]);
		Temp[i] = cond.Temp;
		ThQual[i] = cond.ThQual;
    
		if (HeatSource == 6) {


			SoilHeatSource[i] = (p_NC_Soil->O_RespRate[_H][i] + p_NC_Soil->O_RespRate[_L1][i] + p_NC_Soil->O_RespRate[_L2][i])*HeatConversionCoef;
		}
    
 		else if(HeatSource==5&&size_t(HeatCableLayer)==i) {
			if(m_pModelInfo->T_Current>AgeofSourceAtStart&&SingleCableControl==0)
				SoilHeatSource[i]=SoilHeatCableFunction();
			else if(SingleCableControl==1)
				SoilHeatSource[i]=RegulatedHeatCableAirTempFunction(Temp.front(), p_Atm->TAir );
			else if(SingleCableControl==2)
				SoilHeatSource[i]=RegulatedHeatCableDewPointTempFunction(Temp.front(), p_Atm->VapourPAir);
		}
		else if(HeatSource==5) {
			SoilHeatSource[i]=0.;

		}
 		else if(HeatSource>0) {
 	 		SoilHeatSource[i]=SoilHeatSourceFuncL(i, Temp[i]);
	 		SoilHeatSource[i]=SoilHeatSource[i]+SoilHeatSourceFuncL2(i, Temp[i]);
		}


		if(i>0) {
		//!PROPERTIES AT BOUNDARY BETWEEN SOIL LAYERS ************
 
		  double TQUALM=(p_Soil_WaterF->ThicknessLayers[i]*ThQual[i-1]+p_Soil_WaterF->ThicknessLayers[i-1]*ThQual[i])/ 
			(p_Soil_WaterF->ThicknessLayers[i]+p_Soil_WaterF->ThicknessLayers[i-1]);

		  double TCONDM=f_ThermalCond(i,p_Soil_WaterF->ThetaB[i-1],TQUALM);

		  if(p_Soil_WaterF->Frost) p_Soil_WaterF->ThetaB[i-1]=p_Soil_WaterF->ThetaB[i-1]*(1-TQUALM);
 
		//! SOIL HEAT CONDUCTION **********************************

 
		  SoilHeatFlow[i-1]=86400.*TCONDM*(Temp[i-1]-Temp[i])/p_Soil_WaterF->DistanceMidpLayers[i];

		  if(ThCondOutput==1)  
			 ThCondV[i-1]=TCONDM;
       
		}

	} //	Enddo Loop_over_layers

	p_Soil_WaterF->Infreeze=p_Soil_WaterF->Refreezing();
 
 
    if(InsulatedWaterPipe>0)
          WaterPipeModule();   
    
       
    if(p_Soil_WaterF->NCalc!=SoilHeat.size()) return;
       
//!C***** ********** FROST BOUNDARIES **************************************
       
      if(p_Soil_WaterF->Frost||p_Soil_WaterF->OBOUNDF) 
        p_Soil_WaterF->FrostBoundaries(SoilHeat,TempSoilSurf*HeatCapSolid.front());

}
void Soil_HeatF::HeatWaterFlow() 
{

	
	for(size_t i=0; i<SoilHeat.size()-2&&i<p_Soil_WaterF->NCalc; i++) {
		if(m_pModelStructure->GroundWaterFlow>0) {
			SoilHeatSink[i]=p_Soil_WaterF->WaterDrainFlow[i]*Temp[i]*Fix::SPECW;
			if(m_pModelStructure->LateralInput>=1) SoilHeatSink[i]-=p_Atm->Wsource[i]*Temp[i]*Fix::SPECW;
		}
		else if (p_Atm->Dripper > 0) {
			SoilHeatSink[i] = -p_Atm->Wsource[i] * Temp[i] * Fix::SPECW;
			SoilHeatSink[i] += p_Plant->SingleUptake[i]* Temp[i] * Fix::SPECW;
		}
		if(p_Soil_WaterF->WaterFlow[i]<0.)
			SoilHeatFlow[i]+=p_Soil_WaterF->WaterFlow[i]*Temp[i+1]*Fix::SPECW;
		else
			SoilHeatFlow[i]+=p_Soil_WaterF->WaterFlow[i]*Temp[i]*Fix::SPECW;
	}
	size_t i=SoilHeat.size()-2;
	
	if(i<p_Soil_WaterF->NCalc) {
		if(p_Soil_WaterF->WaterFlow[i]<0.)
			SoilHeatFlow[i]+=p_Soil_WaterF->WaterFlow[i]*Temp.back()*Fix::SPECW;
		else 
			SoilHeatFlow[i]+=p_Soil_WaterF->WaterFlow[i]*Temp[i]*Fix::SPECW;
	}

	double ConvInfil;
	if(PrecTemperature==0)
		ConvInfil=p_Atm->SoilInfil*TempSoilSurf*Fix::SPECW;
	else
		ConvInfil=p_Atm->SoilInfil*(p_Atm->TAir+TempDiffPrec_Air)*Fix::SPECW;

	SurfHeatFlow+=ConvInfil;

	if(m_pModelStructure->GroundWaterFlow==0) 
		SoilHeatFlow.back()+=p_Soil_WaterF->WaterFlow.back()*Temp.back()*Fix::SPECW;


}
void Soil_HeatF::HeatVapourFlow() 
{
	for(size_t i=0; i<SoilHeat.size()-2&&i<p_Soil_WaterF->NCalc; i++) {
		SoilHeatFlow[i]+=p_Soil_WaterF->VapourFlow[i]*Fix::LATE;
		if(ThCondOutput == 2) {
			if(abs(Temp[i]-Temp[i+1])>0.01) {
				double testv=SoilHeatFlow[i]/(Temp[i]-Temp[i+1])*p_Soil_WaterF->DistanceMidpLayers[i];
				if(testv>0.05) ThCondV[i]=testv;
			}
		}
	}

}
void Soil_HeatF::WaterPipeModule()
{

double PartOfDay=0.1;
double FrozenLatentHeat;


int Nshift, ISEG;


	IntegerWaterPipeLayer=size_t(WaterPipeLayer.front());

        
	double HeatBoxCapacity=HeatCapSolidDist[IntegerWaterPipeLayer]*0.5*0.1+.015*4.2E6;
	double InsulatorThickness=0.05;
	FrozenLatentHeat=-15.*Fix::LATMELT;


        for(size_t i=0; i<HeatBoxStorage.size(); i++) HeatBoxTemp[i]=HeatBoxStorage[i]/HeatBoxCapacity;
        

		for(size_t i=0; i<HeatBoxStorage.size(); i++) {
            if(HeatBoxStorage[i]<0.&&HeatBoxStorage[i]>FrozenLatentHeat) 
                HeatBoxTemp[i]=0.0;            
            else if(HeatBoxStorage[i]<FrozenLatentHeat) 
                HeatBoxTemp[i]=(HeatBoxStorage[i]-FrozenLatentHeat)/HeatBoxCapacity;
            IntegerWaterPipeLayer=size_t(abs(WaterPipeLayer[i]));
            HeatBoxBoundaryFlux[i]=(Temp[IntegerWaterPipeLayer]-HeatBoxTemp[i])/InsulatorThickness*ThCondInsulator*m_pModelInfo->SecPerDay;
		}
        
        double PartofDay=m_pModelInfo->T_Current-int(m_pModelInfo->T_Current);       
        
        if(InsulatedWaterPipe==2) {
               if(m_pModelInfo->JDayNum>345||m_pModelInfo->JDayNum<90.) { 
                    if(PartofDay<HeaterDuration) 
                        HeatBoxHeaterFlux.assign(HeatBoxHeaterFlux.size(), HeaterFluxRate);
                    else
                        HeatBoxHeaterFlux.assign(HeatBoxHeaterFlux.size(),0.);
                   
			   }
                HeatBoxHeaterFlux.assign(HeatBoxHeaterFlux.size(),0.);
        }  
        else if(InsulatedWaterPipe==3) {
            
			for(size_t i=0; i<HeatBoxStorage.size(); i++) {
                HeatBoxStorage[i]<0. ?                
                    HeatBoxHeaterFlux[i]=-HeatBoxStorage[i]/T_Step
					:
                    HeatBoxHeaterFlux[i]=0.;
 
			}
		}        
        else if(InsulatedWaterPipe==4) {
        	for(size_t i=0; i<HeatBoxStorage.size(); i++) {
                if(HeatBoxStorage[8]<0.)       
                        HeatBoxEnhancer=min(1000.,HeatBoxEnhancer*1.1);
                else if(HeatBoxTemp[8]>.5)                         
                        HeatBoxEnhancer=max(1., HeatBoxEnhancer/1.1);                
       
			}
		}
        
        double HeatBoxReturnAcc=0;
        for(size_t i=0; i<HeatBoxStorage.size(); i++)  HeatBoxReturnAcc+=HeatBoxBoundaryFlux[i];
       
        HeatBoxReturnFlow=HeatBoxReturnAcc/HeatBoxStorage.size();
        
        
        
        
        if(WaterPipeBox>0) {
				for(size_t i=0; i<ColdPipeTemp.size(); i++) {
					ColdPipeTemp[i]=max(0., ColdPipeHeat[i]/HeatCapacityPipeWater);
					OutPipeTemp[i]=max(0., OutPipeHeat[i]/HeatCapacityPipeWater);
				}
        		if(SewageWaterStore>WaterSewageBuffer*0.5) 
					SewageTemp=max(0.,SewageHeatStore/(Fix::SPECW*SewageWaterStore));
        
               // ! Losses by conduction from pipe to heatbox soil

				for(size_t i=0; i<ColdPipeSinkFlux.size(); i++) {
                    int ISEG=int(i/5);
                    ColdPipeSinkFlux[i]=(ColdPipeHeat[i]/HeatCapacityPipeWater-HeatBoxTemp[ISEG])/HeatPipeResistance;
                    OutPipeSinkFlux[i]=(OutPipeHeat[i]/HeatCapacityPipeWater-HeatBoxTemp[ISEG])/HeatPipeResistance;
				}
                
              //  ! Convective flux along the incoming water pipe
                double DynamicFlux=WaterPipeFlux*WaterPipeFluxFluc*sin(PartOfDay*4*Fix::PI-7*Fix::PI/8)+WaterPipeFlux;
                
                if(HeatBoxEnhancer>1.) 
                    DynamicFlux=min(1000.,DynamicFlux+100.*HeatBoxEnhancer);
  
                    
                ColdPipeInFlux=DynamicFlux*WaterPipeInTemp*Fix::SPECW;

				for(size_t i=0; i<ColdPipeSinkFlux.size(); i++) 
                    ColdPipeFlux[i]=max(0.,ColdPipeTemp[i]*DynamicFlux*Fix::SPECW);
                
                
                SewageInflow=DynamicFlux*(WaterSewageInputAdd+ColdPipeTemp.front())*Fix::SPECW;               
                SewageSinkFlux=(SewageTemp-max(0.,Temp[IntegerWaterPipeLayer+3]))/HeatSewageResistance;
                
                
                SewageWaterIn=DynamicFlux;
                SewageWaterOut=0.;

                if(SewageWaterStore>WaterSewageBuffer*1.4&&SewageWaterStore>0.&&EmptyContainer==false) {
                        SewageWaterOut=WaterSewageBuffer*0.2/T_Step;  
                        EmptyContainer=true;
				}
                else if(SewageWaterStore<WaterSewageBuffer*0.4&&EmptyContainer) {
                        EmptyContainer=false;
                        SewageWaterOut=0.;
				}
                else if(EmptyContainer) 
                        SewageWaterOut=WaterSewageBuffer*0.2/T_Step ;
                                
                    
                if(EmptyContainer) 
                    SewageOutflow=SewageWaterOut*SewageTemp*Fix::SPECW;
                else
                    SewageOutflow=0.;
          
                
                
                
                TravelDistance=(SewageWaterOut*T_Step)/(WaterPipeVolume*1000);
               
                if(TravelDistance>=1) {
                    size_t NShift=size_t(TravelDistance);    
                    TravelDistance=TravelDistance-NShift;
                    auto NPS=OutPipeHeat.size();
                    NShift=min(NPS, NShift);
                    for(size_t i=0; i<NPS-NShift-1; i++)
						OutPipeHeat[i]=OutPipeHeat[i+NShift];

					for(size_t i=NPS-NShift;i<NPS;i++)
                        OutPipeHeat[i]=HeatCapacityPipeWater*SewageTemp;

				}
                else {
                    SewageWaterOut=0.;
                    SewageOutflow=0.;
                }
        }
}
