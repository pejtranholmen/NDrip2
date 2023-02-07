#include "NC_Mineral.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../Atmosphere/AtmModel.h"
#include "../Atmosphere/Irrigation.h"
#include "./Functors/NC_Soil_Functors.h"

						   
NC_Mineral::NC_Mineral(void)
{
	ModuleName="Soil mineral N functions";
	ModuleNo=26;
	ModuleConditions="";
}

bool NC_Mineral::Ini()	{

	P_Mineral::Ini();
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;

//Use	Profile, Only		:	NL, AThicknessLayers, DepthDist
//Use NC_Drivings, Only	:	NDrivTheta
	size_t  Init;

//!	Initial Conditions
	if(!m_pModelInfo->StartStateValue) {
		if(InitialMineral_N==0) {
			for(size_t i=0; i<SoilNitrate.size();i++) SoilNitrate[i]=InitNitrateConc*pSoil->AThicknessLayers[i]*p_NC_Plant->NDrivTheta[i]/100.;
			if(Ammonium_Mobility==0) 
			   for(size_t i=0; i<SoilNitrate.size();i++) SoilAmmonium[i]=InitAmmoniumConc*pSoil->AThicknessLayers[i]*p_NC_Plant->NDrivTheta[i]/100.;			
			else
			   for(size_t i=0; i<SoilNitrate.size();i++)  SoilAmmonium[i]=1./(1.-AmmoniumAdsFrac[i])*InitAmmoniumConc*pSoil->AThicknessLayers[i]*p_NC_Plant->NDrivTheta[i]/100.;
		}    
		else { 
			for(size_t i=0; i<SoilNitrate.size();i++) SoilNitrate[i]=InitNitrateConcZ[i]*pSoil->AThicknessLayers[i]*p_NC_Plant->NDrivTheta[i]/100.;
			if(Ammonium_Mobility==0) 
                for(size_t i=0; i<SoilNitrate.size();i++) SoilAmmonium[i]=InitAmmoniumConcZ[i]*pSoil->AThicknessLayers[i]*p_NC_Plant->NDrivTheta[i]/100.;
			else		
			    for(size_t i=0; i<SoilNitrate.size();i++) SoilAmmonium[i]=1./(1.-AmmoniumAdsFrac[i])*InitAmmoniumConcZ[i]*pSoil->AThicknessLayers[i]*p_NC_Plant->NDrivTheta[i]/100.;

		}
	}

//	Adjustment of depth distribution for denitrification
	//Allocate (DenitDistribution.back())
	DenitDistribution.resize(DenitDistributionTab.size());

	DEPTH_type out;
	if(Denit_Depth_Distribution==0) 
		out.Dist=DenitDistributionTab;
	else {
		out=pSoil->DepthDist(Denit_Depth_Distribution,   1.,DenitDepth ,DenitFracExpTail);	
	}
	DenitDistribution=out.Dist;
//   Allocate for root distribution
	UptNitrate.resize(pSoil->NL);
	UptAmmonium.resize(pSoil->NL);
	
	if(p_NC_Plant->FungalGrowth>0) {
   // !   Allocate for Fungi distribution
	    FungiUptNitrate.resize(pSoil->NL);
	    FungiUptAmmonium.resize(pSoil->NL);
	}
	
    N_Tot_AnarobicGasStorage=N_Tot_DenitMicrobeStorage=N_Tot_NitriMicrobeStorage=0.0;

	if(Nitrification_Sw(Nitrification )> Nitrification_Sw::Old_SOILN&&!m_pModelInfo->StartStateValue) { //! Microbial based Nitrification
		Init=Initial_Nitrifier+1;
		out=pSoil->DepthDist(Init, InitNitrifier, NitrifierDepth, InitNitriFracExpTail);
		Nitrifier_Biomass=out.Dist;
        N_Tot_NitriMicrobeStorage=sum(Nitrifier_Biomass);
	}

	NTotNitrate=sum(SoilNitrate);
	NTotAmmonium=sum(SoilAmmonium);
	NTotMinN_Soil=NTotNitrate+NTotAmmonium;

	
	if(Denitrification_Sw(Denitrification)== Denitrification_Sw::Microbial_based) {

		DMic_Growth_NO3.resize(pSoil->NL); DMic_Growth_NO2.resize(pSoil->NL); DMic_Growth_NO.resize(pSoil->NL); DMic_Growth_N2O.resize(pSoil->NL);
		DMic_RespGrowth_NO3.resize(pSoil->NL); DMic_RespGrowth_NO2.resize(pSoil->NL); DMic_RespGrowth_NO.resize(pSoil->NL); DMic_RespGrowth_N2O.resize(pSoil->NL);
		DMic_RespMaint_NO3.resize(pSoil->NL); DMic_RespMaint_NO2.resize(pSoil->NL); DMic_RespMaint_NO.resize(pSoil->NL); DMic_RespMaint_N2O.resize(pSoil->NL);
		An_NXOY.resize(pSoil->NL);
		DMic_Growth_NXOY.resize(pSoil->NL);
		if(!m_pModelInfo->StartStateValue) {
			An_NO3=SoilNitrate;
			An_NO2.assign(pSoil->NL,0.);
			An_NO.assign(pSoil->NL,0.);
			An_N2O.assign(pSoil->NL,0.);
			An_N2.assign(pSoil->NL,0.);
			Denitrifier_Activity.assign(pSoil->NL,0.2);
			 for(size_t i=0; i<Denitrifier_Biomass.size();i++) Denitrifier_Biomass[i]=DenitDistributionTab[i]*InitDenitBiomass;
			 if(Denit_Depth_Distribution>0) {
				out=pSoil->DepthDist(Denit_Depth_Distribution, InitDenitBiomass, DenitDepth, DenitFracExpTail);
				Denitrifier_Biomass=out.Dist;
			}
		}
        N_Tot_AnarobicGasStorage=0.0;
        N_Tot_DenitMicrobeStorage=sum(Denitrifier_Biomass);
        
        RatioNO3.resize(pSoil->NL);
        RatioNO2.resize(pSoil->NL);
        RatioNO.resize(pSoil->NL);
        RatioN2O.resize(pSoil->NL);
        
	}
	IniGasMicrobeStorage=N_Tot_DenitMicrobeStorage+N_Tot_NitriMicrobeStorage;

	return true;
}
bool NC_Mineral::IniFunctors() {

	P_Mineral::IniFunctors();


	if (Denit_Temp_Response_Sw(Denit_Temp_Response) == Denit_Temp_Response_Sw::Common) {
		f_DenitTempResponse = f_CommonTempResponse;
	}
	else if (Denit_Temp_Response_Sw(Denit_Temp_Response) == Denit_Temp_Response_Sw::Q10_whole_range)
		f_DenitTempResponse = Get_D1_Functor(MyFunc::DENIT_TEMP_Q10_RESPONSE);
	else if (Denit_Temp_Response_Sw(Denit_Temp_Response) == Denit_Temp_Response_Sw::Q10_above_threshold)
		f_DenitTempResponse = Get_D1_Functor(MyFunc::DENIT_TEMP_Q10THRESHOLD_RESPONSE);
	else if (Denit_Temp_Response_Sw(Denit_Temp_Response) == Denit_Temp_Response_Sw::Ratkowsky)
		f_DenitTempResponse = Get_D1_Functor(MyFunc::DENIT_TEMP_RATKOWSKY_RESPONSE);

	if (Nitrific_Temp_Response_Sw(Nitrific_Temp_Response) == Nitrific_Temp_Response_Sw::Common) {
		f_NitriTempResponse = f_CommonTempResponse;
	}
	else if (Nitrific_Temp_Response_Sw(Nitrific_Temp_Response) == Nitrific_Temp_Response_Sw::Q10_whole_range)
		f_NitriTempResponse = Get_D1_Functor(MyFunc::NITRI_TEMP_Q10_RESPONSE);
	else if (Nitrific_Temp_Response_Sw(Nitrific_Temp_Response) == Nitrific_Temp_Response_Sw::Q10_above_threshold)
		f_NitriTempResponse = Get_D1_Functor(MyFunc::NITRI_TEMP_Q10THRESHOLD_RESPONSE);
	else if (Nitrific_Temp_Response_Sw(Nitrific_Temp_Response) == Nitrific_Temp_Response_Sw::Ratkowsky)
		f_NitriTempResponse = Get_D1_Functor(MyFunc::NITRI_TEMP_RATKOWSKY_RESPONSE);


	f_DenitThetaResponse = Get_D1_TableFunctor(MyFunc::DENIT_THETA_RESPONSE);

	if(!f_Theta_Response)	f_Theta_Response = Get_D1_TableFunctor(MyFunc::ABIOTIC_THETA_RESPONSE);

	return true;
}
bool NC_Mineral::Dyn()
{
	P_Mineral::Dyn();
	Flux();
	//	Integ();  Moved to NC_GasFlux to avoid problem with integration
	return true;
}

void NC_Mineral::Flux()
{

/*	Use NC_Drivings, Only	:	NDrivTheta, NDrivWaterFlow, NDrivDrainage, NDrivDeepPercolation
Use NC_Drivings, Only	:	DeepPercolationInput, SoilDrainageInput
Use	Profile, Only		:	NL, AThicknessLayers
Use AbioticResponses
Use TimeInfo, Only		:	TTSTEP_NC
Use NC_Organic, Only    : NDissolved*/


    double NitriMaxRate;
    
    double SumDrainage;
	int koll = 0;

//	Nitrate concentrations
	for(size_t i=0; i<SoilNitrateConc.size(); i++) {
		SoilNitrateConc[i]=100.*SoilNitrate[i]/p_NC_Plant->NDrivTheta[i]/pSoil->AThicknessLayers[i];
		if (SoilNitrate[i] < 0.&&SoilNitrate[i]>-100.)  SoilNitrateConc[i] = 1.E-7;
		else if (SoilNitrate[i] > 0.) {
		 }
		else 
		    koll = 3;
	}
//	Vertical nitrate flow
	
	for(size_t i=0; i<SoilNitrateConc.size(); i++) {
		if(p_NC_Plant->NDrivWaterFlow[i]>0.) {
            if(SoilNitrateConc[i]>=0&&SoilNitrateConc[i]<100000.) 
                ;
            else
                SoilNitrateConc[i]=0.000001;

			SoilNitrateFlow[i]=p_NC_Plant->NDrivWaterFlow[i]*SoilNitrateConc[i]/1000.;
		}
		else if(i<SoilNitrateConc.size()-1) {
            if(SoilNitrateConc[i+1]>=0&&SoilNitrateConc[i+1]<100000.) 
				;
            else
                SoilNitrateConc[i+1]=0.000001;
        
			SoilNitrateFlow[i]=p_NC_Plant->NDrivWaterFlow[i]*SoilNitrateConc[i+1]/1000.;
		}
		else
			SoilNitrateFlow[i]=0.;
		
	}
//   Deep percolation
	if(p_NC_Plant->DeepPercolationInput>0) 
	   SoilNitrateFlow.back()=SoilNitrateFlow.back()+p_NC_Plant->NDrivDeepPercolation*SoilNitrateConc.back()/1000.;

//	Horizontal nitrate flow
	if(p_NC_Plant->SoilDrainageInput>0) 
		for(size_t i=0; i<SoilNitrateDrainage.size(); i++) SoilNitrateDrainage[i]=p_NC_Plant->NDrivDrainage[i]*SoilNitrateConc[i]/1000.;
	

// Ammonium concentration
	if(Ammonium_Mobility==0) 
		for(size_t i=0; i<SoilAmmoniumConc.size(); i++) SoilAmmoniumConc[i]=100.*SoilAmmonium[i]/p_NC_Plant->NDrivTheta[i]/pSoil->AThicknessLayers[i];
	else {
		for(size_t i=0; i<SoilAmmoniumConc.size(); i++) SoilAmmoniumConc[i]=100.*SoilAmmonium[i]*(1.-AmmoniumAdsFrac[i])/p_NC_Plant->NDrivTheta[i]/pSoil->AThicknessLayers[i];
//	Vertical ammonium flow
		//Do i=1, NL
		for(size_t i=0; i<SoilAmmoniumFlow.size(); i++) {
			if(p_NC_Plant->NDrivWaterFlow[i]>0.) 
				SoilAmmoniumFlow[i]=p_NC_Plant->NDrivWaterFlow[i]*SoilAmmoniumConc[i]/1000.;
			else if(i<SoilAmmoniumFlow.size()-1) 
				SoilAmmoniumFlow[i]=p_NC_Plant->NDrivWaterFlow[i]*SoilAmmoniumConc[i+1]/1000.;
			else
				SoilAmmoniumFlow[i]=0.;
		}
//   Deep percolation ammonium
		if(p_NC_Plant->DeepPercolationInput>0) 
		   SoilAmmoniumFlow.back()=SoilAmmoniumFlow.back()+p_NC_Plant->NDrivDeepPercolation*SoilAmmoniumConc.back()/1000.;


//	Horizontal ammonium flow
		if(p_NC_Plant->SoilDrainageInput>0) 
			for(size_t i=0; i<SoilAmmoniumDrainage.size(); i++) SoilAmmoniumDrainage[i]=p_NC_Plant->NDrivDrainage[i]*SoilAmmoniumConc[i]/1000.;
	}


//	Denitrification Process
	if(Denitrification_Sw(Denitrification)== Denitrification_Sw::Old_SOILN) { // Old SOILN


		for(size_t i=0; i<DenitResponse.size(); i++) {
			DenitResponse[i]	=f_DenitTempResponse(p_NC_Plant->NDrivTemperature[i])*f_DenitThetaResponse(i, p_NC_Plant->NDrivTheta[i])*DenitNitrateFunction(SoilNitrateConc[i]);
			DenitrificationRate[i]=DenitResponse[i]*DenitDistribution[i]*DenitPotentialRate;
		}
	}
	else if(Denitrification_Sw(Denitrification)== Denitrification_Sw::Microbial_based) { // New Microbial based

			//An_NO3=SoilNitrate
            

		for(size_t i=0; i<DenitResponse.size(); i++) {

		  //  if(An_NO3[i]<0) An_NO3[i]=0.
		  //  if(An_NO2[i]<0) An_NO2[i]=0.
		  //  if(An_NO[i]<0) An_NO[i]=0.
		  //  if(An_N2O[i]<0) An_N2O[i]=0.
		  An_NO3[i]=SoilNitrate[i];

			An_NO3_Conc[i]	=LiquidConcL(i, An_NO3[i]);
			An_NO2_Conc[i]	=LiquidConcAnFractionL(i, An_NO2[i]);
			An_NO_Conc[i]	=LiquidConcAnFractionL(i, An_NO[i]);
			An_N2O_Conc[i]	=LiquidConcAnFractionL(i, An_N2O[i]);


		    DMic_Growth_NO3[i]	=DMic_GrowthCoef_NO3*GrowthResponseDOC_L(i)*GrowthResponseNO3_L(i) ;   
		    DMic_Growth_NO2[i]	=DMic_GrowthCoef_NO2*GrowthResponseDOC_L(i)*GrowthResponseNO2_L(i)	;
		    DMic_Growth_NO[i]	=DMic_GrowthCoef_NO*GrowthResponseDOC_L(i)*GrowthResponseNO_L(i);	
		    DMic_Growth_N2O[i]	=DMic_GrowthCoef_N2O*GrowthResponseDOC_L(i)*GrowthResponseN2O_L(i);	

			DMic_RespGrowth_NO3[i]	=DMic_Growth_NO3[i]/DMic_EffCoef_NO3-DMic_Growth_NO3[i];
			DMic_RespGrowth_NO2[i]	=DMic_Growth_NO2[i]/DMic_EffCoef_NO2-DMic_Growth_NO2[i];
			DMic_RespGrowth_NO[i]	=DMic_Growth_NO[i]/DMic_EffCoef_NO-DMic_Growth_NO[i];
			DMic_RespGrowth_N2O[i]	=DMic_Growth_N2O[i]/DMic_EffCoef_N2O-DMic_Growth_N2O[i];

//// Changed RespGrowth Calculations so that the total sum of Growth and RespGrowth is consistent
//// In previous version the Resp Growth (prior September 28)  the Growth Respiration was not substracted 
//// so the RespGrowth was taken as the ratio of Growth and the efficiency only	/PEJ
	    

			An_NXOY[i]=max(0.0001, An_NO3[i]+An_NO2[i]+An_NO[i]+An_N2O[i]);
            if(An_NXOY[i]>1.E-6) {
			    if(An_NO3[i]>0.) 
			        DMic_RespMaint_NO3[i]	=DMic_RespCoef_NO3*An_NO3[i]/An_NXOY[i];
			    else
			        DMic_RespMaint_NO3[i]=0.;
			       
			    if(An_NO2[i]>0.)      
			        DMic_RespMaint_NO2[i]	=DMic_RespCoef_NO2*An_NO2[i]/An_NXOY[i];
			    else
			        DMic_RespMaint_NO2[i]=0.;
			    
			    if(An_NO[i]>0.)        
			        DMic_RespMaint_NO[i]	=DMic_RespCoef_NO* An_NO[i]/An_NXOY[i];
			    else
			        DMic_RespMaint_NO[i]=0.;
			    
			    if(An_N2O[i]>0.) 
			        DMic_RespMaint_N2O[i]	=DMic_RespCoef_N2O*An_N2O[i]/An_NXOY[i];
			    else
			        DMic_RespMaint_N2O[i]=0.;
			    

               An_Cons_NO3[i]			= (DMic_RespGrowth_NO3[i]+DMic_RespMaint_NO3[i])*Denitrifier_Biomass[i]*Denitrifier_Activity[i];
		       An_Cons_NO2[i]			= (DMic_RespGrowth_NO2[i]+DMic_RespMaint_NO2[i])*Denitrifier_Biomass[i]*Denitrifier_Activity[i];
 		       An_Cons_NO[i]			= (DMic_RespGrowth_NO[i] +DMic_RespMaint_NO[i]) *Denitrifier_Biomass[i]*Denitrifier_Activity[i];
               An_Cons_N2O[i]			= (DMic_RespGrowth_N2O[i]+DMic_RespMaint_N2O[i])*DenitriInhibitNO3L(i)*Denitrifier_Biomass[i]*Denitrifier_Activity[i];
			}
            else {
                DMic_RespMaint_NO3[i]=DMic_RespMaint_NO2[i]=DMic_RespMaint_NO[i]=DMic_RespMaint_N2O[i]=0.;
                An_Cons_NO3[i]	= An_Cons_NO2[i]= An_Cons_NO[i]= An_Cons_N2O[i]	= 0.;
            } 
			DenitrificationRate[i]=An_Prod_N2[i]+An_Prod_N2O[i]+An_Prod_NO[i];
		}		

			
	}

//   Nitrification Process

	if(Nitrification==0 ) { // Old SOILN model
		//Do i=1,NL
		for(size_t i=0; i<NitrificationRate.size(); i++) 
			NitrificationRate[i]=f_NitriTempResponse(p_NC_Plant->NDrivTemperature[i])*f_Theta_Response(i, p_NC_Plant->NDrivTheta[i])*Nitri_pH_Response[i]*NitrificationFunction(SoilAmmonium[i],SoilNitrate[i]);
	
        N_Tot_NitriMicrobeStorage=0.0;
	}
	else if(Nitrification==1) {  // Microbial based Nitrification process
	
		//Do i=1, NL
		for(size_t i=0; i<Nitrifier_GrowthRate.size(); i++) {
		     if(NitrifierGrowth==0)    
		            NitriMaxRate=max(0.,SoilAmmonium[i]/T_Step);
		     else
		            NitriMaxRate=max(0.,O_State[_DIS][_N_][i]/T_Step);
		     
			Nitrifier_GrowthRate[i]=min(NitriMaxRate, f_NitriTempResponse(p_NC_Plant->NDrivTemperature[i])*f_Theta_Response(i, p_NC_Plant->NDrivTheta[i])*Nitri_pH_Response[i]*NitrifierGrowthFunctionL(i)*Nitrifier_Biomass[i]);
			// Nitrifier_DeathRate[i]=max(0., min(Nitrifier_Biomass[i]/TTSTEP_NC,NitriTempFunctionL(i)*CommonThetaFunctionL(i)*Nitri_pH_Response[i]*NitrifierDeathFunctionL(i)*Nitrifier_Biomass[i]**2))
			Nitrifier_DeathRate[i]=max(0., min(Nitrifier_Biomass[i]/T_Step,NitrifierDeathFunctionL(i)*Nitrifier_Biomass[i]*Nitrifier_Biomass[i]));
            Nitrifier_RespRate[i]= f_NitriTempResponse(p_NC_Plant->NDrivTemperature[i])*f_Theta_Response(i, p_NC_Plant->NDrivTheta[i])*Nitri_pH_Response[i]*NitrifierRespFunctionL(i)*Nitrifier_Biomass[i];
			NitrificationRate[i]=NitrificationRateCoef*NitrificationNH4FunctionL(i)*f_NitriTempResponse(p_NC_Plant->NDrivTemperature[i])*f_Theta_Response(i, p_NC_Plant->NDrivTheta[i])*Nitri_pH_Response[i]*Nitrifier_Biomass[i];
            NitrificationRate[i]=max(0., min(NitrificationRate[i],SoilAmmonium[i]/T_Step*0.1));					  
		}
		N_Tot_NitriMicrobeStorage=sum(Nitrifier_Biomass);
		
	    N_Tot_NitriDeathRate=sum(Nitrifier_DeathRate);
	   	N_Tot_NitriGrowthRate=sum(Nitrifier_GrowthRate);
	}

	NTotNO3_Drainage=sum(SoilNitrateDrainage);
	NTotNH4_Drainage=sum(SoilAmmoniumDrainage);
	NTotMinN_Drainage=NTotNO3_Drainage+NTotNH4_Drainage;
	SumDrainage=sum(p_NC_Plant->NDrivDrainage);
	if(SumDrainage>0) {
	    NTotNO3Conc_Drainage=1000.*NTotNO3_Drainage/SumDrainage;
	    NTotNH4Conc_Drainage=1000.*NTotNH4_Drainage/SumDrainage;
	    NTotMinNConc_Drainage=1000.*NTotMinN_Drainage/SumDrainage;
	}
	
	NTotDenitrification=sum(DenitrificationRate);
	NTotNitrification=sum(NitrificationRate);
	
	NTotNitrate=sum(SoilNitrate);
	NTotAmmonium=sum(SoilAmmonium);
	NTotMinN_Soil=NTotNitrate+NTotAmmonium;


}
void NC_Mineral::Integ()
{

	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	Irrigation *pMet=(Irrigation*)&pAtm->m_Irrigation;
/*	Use Profile, Only	: NL
	Use NC_Drivings, Only	:	DeepPercolationInput, SoilDrainageInput
	Use Timeinfo
	Use General, Only	: InflowScalar, OutFlowScalar, OutFlow,InFlow, Intflow, Transflow
	Use NC_Organic, Only    : NDissolved, O_State[_L1][_N_]
    Use Irrigate, Only  : Dripper*/
	



	
//	Nitrate and Ammonium Flow with water
	if(p_NC_Plant->SoilDrainageInput>0) {
		OutFlow(SoilNitrate, SoilNitrateDrainage);
		OutFlow(SoilAmmonium, SoilAmmoniumDrainage);
	}

    if(pMet->Dripper==1) {
		InFlow(SoilNitrate, SoilNitrateInflow);
		InFlow(SoilAmmonium, SoilAmmoniumInflow);

    }

	IntFlow(SoilNitrate, SoilNitrateFlow);
	IntFlow(SoilAmmonium, SoilAmmoniumFlow);
//   Deep Percolation
	OutFlow(SoilNitrate.back(), SoilNitrateFlow.back());
	OutFlow(SoilAmmonium.back(), SoilAmmoniumFlow.back());
//	Denitrification losses
	if(Denitrification==1)  // Old SOILN model
		OutFlow(SoilNitrate, DenitrificationRate);
	else if(Denitrification==2) { // New Microbial based model
		InFlow (Denitrifier_Biomass, Denitrifier_Growth);
		OutFlow(Denitrifier_Biomass, Denitrifier_Death);
		InFlow(O_State[_L1][_N_], Denitrifier_Death);
		
		if(DenitrifierGrowth==0) {
		    if(Denitrifier_Growth[0]>=0.) 
		        OutFlow(SoilAmmonium, Denitrifier_Growth);
            else
                Denitrifier_Growth[0]=0.;
		}

		

//		OutFlow(An_NO3, An_Cons_NO3);	  // Nitrate is consumed from entire NO3 Pool
		OutFlow(SoilNitrate, An_Cons_NO3);
		
		OutFlow(An_NO2, An_Cons_NO2);
		OutFlow(An_NO, An_Cons_NO);
		OutFlow(An_N2O, An_Cons_N2O);

		InFlow(An_NO2, An_Prod_NO2);
		InFlow(An_NO, An_Prod_NO);
		InFlow(An_N2O, An_Prod_N2O);
		InFlow(An_N2, An_Prod_N2);

		InFlow(Denitrifier_Activity, Denitrifier_ActivityRate);

	}
//   Nitrification
    if(NitrificationRate[0]>=0.) {
	    OutFlow(SoilAmmonium, NitrificationRate);
	    InFlow(SoilNitrate, NitrificationRate);
	}
    else
        NitrificationRate[0]=0.;
    
	
	//TransFlow(SoilAmmonium, SoilNitrate, NitrificationRate);
	if(Nitrification>0) {
		 InFlow(Nitrifier_Biomass, Nitrifier_GrowthRate);
		 OutFlow(Nitrifier_Biomass, Nitrifier_DeathRate);
		 InFlow(O_State[_L1][_N_], Nitrifier_DeathRate);
		// OutFlow(Nitrifier_Biomass, Nitrifier_RespRate);
        // Removed Nitrifier_RespRate from N Mass balance 20090301
		if(NitrifierGrowth==0) 
		    OutFlow(SoilAmmonium, Nitrifier_GrowthRate);
		    
		else if(NitrifierGrowth==1) 
		    OutFlow(O_State[_DIS][_N_], Nitrifier_GrowthRate);
		
		
	}
//	Mineral N Uptake by roots and mycorrhiza
	OutFlow(SoilAmmonium	,SoilAmmoniumUptake);
	OutFlow(SoilNitrate	,SoilNitrateUptake);





}

bool NC_Mineral::End()
{
return true;
}
NC_Mineral::~NC_Mineral(void)
{
}
