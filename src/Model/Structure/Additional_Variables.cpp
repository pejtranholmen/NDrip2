#include "Additional_Variables.h"
#include "../../Simulator/Simulator.h"

						   
Additional_Variables::Additional_Variables(void)
{
	ModuleName="Additional Variables";
	ModuleNo=23;
	ModuleConditions="";
}

bool Additional_Variables::Ini()	{

	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	InitialTotalStorage=GetTotalWaterStorage();
	T_Step=*pT_Step;
	if(!m_pModelInfo->StartStateValue) {
		AccSoilEvap=AccTotalEvap=AccSoilEvapAndTransp=0.;
		AccTotalRunoff=AccPrecipitation=AccThroughfall=AccSurfaceRunoff=
		AccPrecandIrrig=AccThroughfall=AccPrecandIrrig=AccPrecAdjustSnow=
		AccLateralInflowWater=AccSpoolRunOn=AccTotalWaterInput=0;
	}

	f_ZadokShifts = ZadokShift2(p_NC_Plant->ZadokIndex, ZadokDays);
	if(Soil_TempSensors>0) {
		TempSond.resize(NTempSensors);
		for(size_t i=0; i<TempSond.size(); i++) {
			if(TempSensorHigh[i]<=p_Soil_WaterF->DepthMidpLayer.front()) {
					TempSond[i].UpperLayer1=0;
					TempSond[i].UpperLayer2=0;
					TempSond[i].UpperWeight2=0.;
					TempSond[i].UpperWeight1=1.;
			}
			else if(TempSensorHigh[i]>=p_Soil_WaterF->DepthMidpLayer.back()) {
					TempSond[i].UpperLayer1=p_Soil_WaterF->DepthMidpLayer.size()-1;
					TempSond[i].UpperLayer2=p_Soil_WaterF->DepthMidpLayer.size()-1;
					TempSond[i].UpperWeight2=0.;
					TempSond[i].UpperWeight1=1.;
			}
			else {
				for(size_t ii=0; ii<p_Soil_WaterF->DepthMidpLayer.size()-1;ii++) {
					if(TempSensorHigh[i]>=p_Soil_WaterF->DepthMidpLayer[ii]&&TempSensorHigh[i]<=p_Soil_WaterF->DepthMidpLayer[ii+1]) {
						TempSond[i].UpperLayer1=ii;
						TempSond[i].UpperLayer2=ii+1;
						TempSond[i].UpperWeight2=(TempSensorHigh[i]-p_Soil_WaterF->DepthMidpLayer[ii])
													/(p_Soil_WaterF->DepthMidpLayer[ii+1]-p_Soil_WaterF->DepthMidpLayer[ii]);
						TempSond[i].UpperWeight1=1-TempSond[i].UpperWeight2;
					}
				}
			}
		
			if(TempSensorLow[i]<=p_Soil_WaterF->DepthMidpLayer.front()) {
					TempSond[i].LowerLayer1=0;
					TempSond[i].LowerLayer2=0;
					TempSond[i].LowerWeight2=0.;
					TempSond[i].LowerWeight1=1.;
			}
			else if(TempSensorLow[i]>=p_Soil_WaterF->DepthMidpLayer.back()) {
					TempSond[i].LowerLayer1=p_Soil_WaterF->DepthMidpLayer.size()-1;
					TempSond[i].LowerLayer2=p_Soil_WaterF->DepthMidpLayer.size()-1;
					TempSond[i].LowerWeight2=0.;
					TempSond[i].LowerWeight1=1.;
			}
			else {
				for(size_t ii=0; ii<p_Soil_WaterF->DepthMidpLayer.size()-1;ii++) {
					if(TempSensorLow[i]>=p_Soil_WaterF->DepthMidpLayer[ii]&&TempSensorLow[i]<=p_Soil_WaterF->DepthMidpLayer[ii+1]) {
						TempSond[i].LowerLayer1=ii;
						TempSond[i].LowerLayer2=ii+1;
						TempSond[i].LowerWeight2=(TempSensorLow[i]-p_Soil_WaterF->DepthMidpLayer[ii])
													/(p_Soil_WaterF->DepthMidpLayer[ii+1]-p_Soil_WaterF->DepthMidpLayer[ii]);
						TempSond[i].LowerWeight1=1-TempSond[i].LowerWeight2;
					}
				}
			}

		}



		TempSensors=GetSensorValues(p_Soil_HeatF->Temp, &TempSond);
	}
	if(Soil_ThetaSensors>0) {
		ThetaSond.resize(NThetaSensors);

		for(size_t i=0; i<ThetaSond.size(); i++) {
			if(ThetaSensorHigh[i]<=p_Soil_WaterF->DepthMidpLayer.front()) {
					ThetaSond[i].UpperLayer1=0;
					ThetaSond[i].UpperLayer2=0;
					ThetaSond[i].UpperWeight2=0.;
					ThetaSond[i].UpperWeight1=1.;
			}
			else if(ThetaSensorHigh[i]>=p_Soil_WaterF->DepthMidpLayer.back()) {
					ThetaSond[i].UpperLayer1=p_Soil_WaterF->DepthMidpLayer.size()-1;
					ThetaSond[i].UpperLayer2=p_Soil_WaterF->DepthMidpLayer.size()-1;
					ThetaSond[i].UpperWeight2=0.;
					ThetaSond[i].UpperWeight1=1.;
			}
			else {
				for(size_t ii=0; ii<p_Soil_WaterF->DepthMidpLayer.size()-1;ii++) {
					if(ThetaSensorHigh[i]>=p_Soil_WaterF->DepthMidpLayer[ii]&&ThetaSensorHigh[i]<=p_Soil_WaterF->DepthMidpLayer[ii+1]) {
						ThetaSond[i].UpperLayer1=ii;
						ThetaSond[i].UpperLayer2=ii+1;
						ThetaSond[i].UpperWeight2=(ThetaSensorHigh[i]-p_Soil_WaterF->DepthMidpLayer[ii])
													/(p_Soil_WaterF->DepthMidpLayer[ii+1]-p_Soil_WaterF->DepthMidpLayer[ii]);
						ThetaSond[i].UpperWeight1=1-ThetaSond[i].UpperWeight2;
					}
				}
			}
			if(ThetaSensorLow[i]<=p_Soil_WaterF->DepthMidpLayer.front()) {
					ThetaSond[i].LowerLayer1=0;
					ThetaSond[i].LowerLayer2=0;
					ThetaSond[i].LowerWeight2=0.;
					ThetaSond[i].LowerWeight1=1.;
			}
			else if(ThetaSensorLow[i]>=p_Soil_WaterF->DepthMidpLayer.back()) {
					ThetaSond[i].LowerLayer1=p_Soil_WaterF->DepthMidpLayer.size()-1;
					ThetaSond[i].LowerLayer2=p_Soil_WaterF->DepthMidpLayer.size()-1;
					ThetaSond[i].LowerWeight2=0.;
					ThetaSond[i].LowerWeight1=1.;
			}
			else {
				for(size_t ii=0; ii<p_Soil_WaterF->DepthMidpLayer.size()-1;ii++) {
					if(ThetaSensorLow[i]>=p_Soil_WaterF->DepthMidpLayer[ii]&&ThetaSensorLow[i]<=p_Soil_WaterF->DepthMidpLayer[ii+1]) {
						ThetaSond[i].LowerLayer1=ii;
						ThetaSond[i].LowerLayer2=ii+1;
						ThetaSond[i].LowerWeight2=(ThetaSensorLow[i]-p_Soil_WaterF->DepthMidpLayer[ii])
													/(p_Soil_WaterF->DepthMidpLayer[ii+1]-p_Soil_WaterF->DepthMidpLayer[ii]);
						ThetaSond[i].LowerWeight1=1-ThetaSond[i].LowerWeight2;
					}
				}
			}



		}
		ThetaSensors=GetSensorValues(p_Soil_WaterF->Theta, &ThetaSond);
	}
	TDiffC = 0;

	return true;
}
bool Additional_Variables::Dyn()
{
	Integ();

	auto DaysAfterSowing = m_pModelInfo->JDayNum - p_NC_Plant->m_LatestSowingDay;
	if (DaysAfterSowing < 0) DaysAfterSowing += 365;

	f_ZadokShifts(DaysAfterSowing, ZadokDayValues);
	TotalDiffStorage=GetTotalWaterStorage()-InitialTotalStorage;
	if(abs(WaterBalanceCheck)>2.) {
		TotalDiffStorage=GetTotalWaterStorage()-InitialTotalStorage;
	}
	WaterBalanceCheck=GetWaterBalanceCheck();
	
	if(Soil_TempSensors>0) TempSensors=GetSensorValues(p_Soil_HeatF->Temp, &TempSond);
	if(Soil_ThetaSensors>0) ThetaSensors=GetSensorValues(p_Soil_WaterF->Theta, &ThetaSond);
	for(size_t i=0;i<AccWStorage.size(); i++) {
		AccWStorage[i]=GetSondStorage(i, &p_Soil_WaterF->WaterStorage);
		AccSStorage[i]=GetSondStorage(i,&p_Plant->SaltStorage);
		AccNO3Storage[i]=GetSondStorage(i, &m_pNCSoil->SoilNitrate);
		AccNH4Storage[i]=GetSondStorage(i, &m_pNCSoil->SoilAmmonium);
		AccMinNStorage[i] = AccNO3Storage[i] + AccNH4Storage[i];
		ORG_PROBES a=Get_Org_SondStorage(i);
		AccCSoilStorage[i]=a.Carbon;
		AccNSoilStorage[i]=a.Nitrogen;
	}
	PlantWaterStorage=0;
	for(size_t i=0; i<p_Soil_WaterF->Theta.size();i++) {
		if(p_Soil_WaterF->ModelUpperDepth[i]<=-p_Plant->RootDepth[0])
			PlantWaterStorage+=max(0.,(p_Soil_WaterF->Theta[i]-p_Soil_WaterF->M_Wilting[i]))*p_Soil_WaterF->ThicknessLayers[i];

	}
	PlantWaterStorage*=10.;

	if(m_pModelStructure->Evaporation>0)	AtmCanopyHeatFlux();

	return true;
}

void Additional_Variables::Flux()
{
	
}
void Additional_Variables::Integ()
{
	if(m_pModelStructure->Evaporation>0) {
		InFlow(AccSoilEvap,p_Irrigation->SoilEvaporationFlux);
		double SumTransAndEvap=p_Irrigation->SoilEvaporationFlux+p_Plant->Transpiration_All;
		if(m_pModelStructure->PlantType>0) 
			InFlow(AccSoilEvapAndTransp,SumTransAndEvap);
		Evapotranspiration=p_Plant->Transpiration_All+p_Plant->EvaActInt+p_Irrigation->SoilEvaporationFlux+p_Irrigation->SnowEvaporation;
		InFlow(AccTotalEvap, Evapotranspiration);

	}
	InFlow(AccSurfaceRunoff,p_Irrigation->SpoolRunoff);
	InFlow(AccTotalRunoff,p_Soil_WaterF->TotalRunoff);
	//InFlow(&AccTotalRunoff,&p_Irrigation->SpoolRunoff);

	InFlow(AccPrecipitation,p_Irrigation->PrecCorr);
	InFlow(AccThroughfall,p_Plant->ThroughFall);

	if(m_pModelStructure->Irrigation>0) {
		double SumInputWater=p_Irrigation->PrecCorr+p_Irrigation->IrrigationRate+p_Irrigation->DripOutlet;
		InFlow(AccPrecandIrrig,SumInputWater);
	}
	if(p_Irrigation->SnowAdjustment>0) 
		InFlow(AccPrecAdjustSnow,p_Irrigation->PrecAdjustSnow);

	if(p_Soil_WaterF->GWSoFlow>0)
		InFlow(AccLateralInflowWater, p_Soil_WaterF->GWSoFlow);

	if(m_pModelStructure->LateralInput>0) {
		LateralInflowWater=sum(p_Irrigation->Wsource)+p_Irrigation->SpoolRunOn+p_Irrigation->SnowImport;
		InFlow(AccLateralInflowWater,LateralInflowWater);
		InFlow(AccSpoolRunOn, p_Irrigation->SpoolRunOn);
		double TotalSumInputWater=p_Irrigation->PrecCorr+p_Irrigation->IrrigationRate+p_Irrigation->DripOutlet+p_Irrigation->PrecAdjustSnow+LateralInflowWater;
		InFlow(AccTotalWaterInput,TotalSumInputWater);

	}

}

bool Additional_Variables::End()
{
return true;
}
Additional_Variables::~Additional_Variables(void)
{
}
void Additional_Variables::AtmCanopyHeatFlux() {
	double EAvailCanopy, CanopySensF;

	TotalLatentFlow=(Evapotranspiration-p_Irrigation->SnowEvaporation)*Fix::LATE+p_Irrigation->SnowEvaporation*Fix::LATESUB;
	if(m_pModelStructure->PlantType<3) {
		EAvailCanopy=p_Irrigation->RadNetTot-p_Irrigation->RadNetGround;
		CanopySensF=EAvailCanopy-(p_Plant->Transpiration_All+p_Plant->EvaActInt)*Fix::LATE;
		TDiff=p_Irrigation->f_TempDiffFromFlux(CanopySensF, p_Plant->ResAir);
		TDiffC=0.8*TDiffC+0.2*TDiff;
		if(p_Irrigation->SnowSurEbal==0||m_pModelStructure->SnowPack_Option==0)
			TotalSensibleFlow=p_Irrigation->SoilSensF+CanopySensF;
		else
			TotalSensibleFlow=p_Soil_HeatF->TotalGroundSensFlow;
		p_Plant->TempCanopy=p_Irrigation->TAir+TDiffC;

	}
	else {
		if(p_Irrigation->SnowSurEbal==0||m_pModelStructure->SnowPack_Option==0)
			TotalSensibleFlow=p_Irrigation->SoilSensF;
		else
			TotalSensibleFlow=p_Soil_HeatF->TotalGroundSensFlow;

		if(MC_TDiff.size()!=p_Irrigation->MC_RntDist.size()) {
			MC_TDiff.resize(p_Irrigation->MC_RntDist.size());
			MC_TDiffC.resize(p_Irrigation->MC_RntDist.size());

		}

		for(size_t i=0; i<p_Irrigation->MC_RntDist.size(); i++) {
			CanopySensF=p_Irrigation->MC_RntDist[i]-(p_Plant->Transpiration[i]+p_Plant->MC_EvaActInt[i])*Fix::LATE;
			MC_TDiff[i]=p_Irrigation->f_TempDiffFromFlux(CanopySensF,p_Plant->MC_ResAir[i]);
			MC_TDiffC[i]=0.8*MC_TDiffC[i]+0.2*MC_TDiff[i];
			p_Irrigation->MC_TempCanopy[i]=p_Irrigation->TAir+MC_TDiffC[i];
			TotalSensibleFlow+=CanopySensF;
		}
		if(sum(p_Plant->LAI)>0) {
			double acc1=0., acc2=0.;
			for(size_t i=0; i<p_Plant->LAI.size(); i++) {
				acc1+=p_Irrigation->MC_TempCanopy[i]*p_Plant->LAI[i];
				acc2+=p_Plant->LAI[i];
			}
			p_Plant->TempCanopy=acc1/acc2;
		}
		else
			p_Plant->TempCanopy=sum(p_Irrigation->MC_TempCanopy)/p_Irrigation->MC_TempCanopy.size();

	}
}

