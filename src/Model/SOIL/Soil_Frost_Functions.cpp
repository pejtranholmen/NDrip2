#include "Soil_Frost_Functions.h"
#include "Soil_HeatPump.h"
	
Soil_Frost_Functions::Soil_Frost_Functions(void)
{
}
Soil_Frost_Functions::~Soil_Frost_Functions(void)
{
}
bool Soil_Frost_Functions::Def()
{
	Soil_WaterF_Functions::Def();
	ModuleNo=15;
	ModuleConditions="";
	Sw* pSw;
	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "X";
	plotpar.Y_Label = "Y";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	Func* pFunc;
	string funcname;

	FunctorProp functorprop;
	P* pP;
	vector<P*> p_vec;
	vector<Ps*> ps_vec;
	ps_vec.clear();


 	pSw= new Sw(&FrostInteract   ,SIMPROFILE,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pSw,  "FrostInteract",1,"No|InfluencingWater","WaterEq > 0|HeatEq > 0");
 	pSw= new Sw(&LoadPotential,SIMPROFILE,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pSw,  "LoadPotential",1,"Off|On", "WaterEq > 0|HeatEq > 0");
 	pSw= new Sw(&FrostInfil,SIMPROFILE,WATER  , INFILTRATION, NOBPROC, NORMAL);
		Define(pSw,     "Infiltration"   ,1,"No reduction|In Low FlowDomain|Low+High FlowD", "WaterEq > 0|HeatEq > 0");
 	pSw= new Sw(&FrostKUpward,SIMPROFILE,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pSw,"k-estimate" ,1,"CentralDifference|MinimiumValues", "WaterEq > 0|HeatEq > 0|FrostInteract > 0");
 	pSw= new Sw(&FrostSwelling,  SIMPROFILE,WATER  , SHRINKING, NOBPROC, NORMAL);
		Define(pSw,  "FrostSwelling",1,"Off|On", "WaterEq > 0|HeatEq > 0|FrostInteract > 0");
 	pSw= new Sw(&FrFlowDomain   ,SIMPROFILE,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pSw,   "FlowDomains" ,1,"Low Domain|Low + High Domain","WaterEq > 0|HeatEq > 0|FrostInteract > 0");
	pSw = new Sw(&FreezingPointOptions, SIMPROFILE, WATER, SOIL_FREEZING, NOBPROC, NORMAL);
		Define(pSw, "Freezing Point Options", 0, "Original Version|By Extra Parameter|By Osmotic Potential", "WaterEq > 0|HeatEq > 0|FrostInteract > 0");

		
		Ps *pPs;
	pPs= new Ps(&HighFlowDampC  ,ALL_SOIL_LAYERS,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs  , "HighFlowDampC", 5.,"vol %","HeatEq > 0|FrostInteract > 0|WaterEq > 0",0.1,80.);
	pPs= new Ps(&FrCondImped  ,ALL_SOIL_LAYERS,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs       ,"LowFlowCondImped", 4.,"-","HeatEq > 0|FrostInteract > 0|WaterEq > 0",0., 10.);

	functorprop.FuncName = MyFunc::HEAT_TEMP_THQUAL; functorprop.Arg = MyArgType::Pair_T_D2;
		funcname = "Temperature Heat Functions"; functorprop.FunctorInitOption = 0;
		functorprop.pHyd = this;
		functorprop.Set_PlotPar = true;
		double theta = 20.;
		plotpar.X_Min = -Fix::LATMELT*theta*100.;
		plotpar.X_Max = 1.E6;

		plotpar.X_ScalingCoef = -6;
		plotpar.X_Label = "Change of Heat Storage ";  plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MEGA_HEATSTORE_VOL_UNIT, true);
		plotpar.Y_Label = "Temperature "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
		plotpar.X2_Vec = { 20. };

	
	pPs = new Ps(&FreezingStartTemp, ALL_SOIL_LAYERS, WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs, "FreezingStartTemp", 0., "-", "HeatEq > 0|WaterEq > 0|Freezing Point Options=1", -3., 0.); ps_vec.push_back(pPs);
	pPs = new Ps(&OsmoticScaleSens, ALL_SOIL_LAYERS, WATER, SOIL_FREEZING, NOBPROC, NORMAL);
		Define(pPs, "OsmoticScaleSens", 0., "-", "HeatEq > 0|WaterEq > 0|Freezing Point Options=2", -2., 2.); ps_vec.push_back(pPs);
	pPs = new Ps(&FpointFWi, ALL_SOIL_LAYERS, WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs, "FreezepointFWi", .5, "-", "HeatEq > 0|WaterEq > 0", 0.01, 2., 0.1, 1.);	ps_vec.push_back(pPs);
	pPs= new Ps(&FpointF0  ,ALL_SOIL_LAYERS,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs  		 ,"FreezepointF0"  , 10.,"-","HeatEq > 0|WaterEq > 0",0., 100.); ps_vec.push_back(pPs);
	pPs= new Ps(&FpointF1  ,ALL_SOIL_LAYERS,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs  		 ,"FreezepointF1"  , 0.,"-","HeatEq > 0|WaterEq > 0",0., 200.); ps_vec.push_back(pPs);
	
	pFunc = new Func(MPROFILE, HEAT, STORAGE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::HEAT_TEMP_THQUAL_OSMOTIC; functorprop.Arg = MyArgType::Pair_T_D3;
	funcname = "Temperature Heat Functions with Salinity "; functorprop.FunctorInitOption = 2;
	plotpar.Num_Of_Curves = 3;
	plotpar.X2_Vec = { 20.f, 20.f, 20.f };
	plotpar.X3_Vec = { 0.5E4, 1.E4, 1.5E4 };
	plotpar.LegendsVec = { "0.5 MPa","1 MPa", "1.5 MPa" };



	pFunc = new Func(MPROFILE, HEAT, STORAGE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);



	pPs= new Ps(&AlphaHT  ,ALL_SOIL_LAYERS,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs  		 ,"AlphaHeatCoef" , 1000.,"W/mC","HeatEq > 0|FrFlowDomain > 0|WaterEq > 0",0.01, 20000.,0.1, 100000.);

	pPs= new Ps(&MaxSwell  ,ALL_SOIL_LAYERS,WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs 	,"MaxSwell", 0.05,"-","HeatEq > 0|FrostSwelling = 1|WaterEq > 0",0.,1.);
	pPs= new Ps(&ShrinkRateFraction  ,ALL_SOIL_LAYERS,WATER  , REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pPs  ,"ShrinkRateFraction", 0.05,"1/day","HeatEq > 0|FrostSwelling = 1|WaterEq > 0",0., 0.1);

	X* pX;
	NE* pNL;
	pNL= m_pModelMap->GetNEPointer("Number of Layers");
	WaterHFD.resize(NL);
	pX= new X(&WaterHFD,pNL, SIMPROFILE,WATER, STORAGE, NOBPROC, NORMAL);
	vector<double> initv;
	initv.assign(NL,0.);
	Define(pX,pNL,"WaterHFD", initv,"mm","HeatEq > 0|FrFlowDomain = 1|FrostInteract = 1|WaterEq > 0");

	T* pT;
	Infreeze.resize(NL);
	pT= new T(&Infreeze,pNL, SIMPROFILE,WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pT,pNL ,"InFreeze",initv,UNIT_TYPES::WATERFLOW_UNIT,"HeatEq > 0|FrFlowDomain = 1|FrostInteract = 1|WaterEq > 0");
	pT= new T(&WaterflowHFD,pNL, SIMPROFILE,WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pT,pNL  	,"WaterflowHFD",initv,UNIT_TYPES::WATERFLOW_UNIT,"HeatEq > 0|FrlowDomain = 1|FrostInteract =1|WaterEq > 0");
	pT= new T(&WaterflowHD_LD,pNL, SIMPROFILE,WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pT,pNL  ,"WaterflowHD_LD",initv,UNIT_TYPES::WATERFLOW_UNIT,"HeatEq > 0|FrlowDomain = 1|FrostInteract =1|WaterEq > 0");
	pT= new T(&WaterflowLD_HD,pNL, SIMPROFILE,WATER, REDISTRIBUTION, NOBPROC, NORMAL);
		Define(pT,pNL  ,"WaterflowLD_HD",initv,UNIT_TYPES::WATERFLOW_UNIT,"HeatEq > 0|FrlowDomain = 1|FrostInteract =1|WaterEq > 0");
	
//!	Call Pne(NFL,2,2,2)
	Gs *pGs= new Gs(&FrostUpperB_1  ,ALL_SOIL_LAYERS,HEAT, FROST_BOUNDARY, NOBPROC, NORMAL);
	Define(pGs, "FrostUpperBoundary1", 0., "m","HeatEq > 0");
	pGs= new Gs(&FrostLowerB_1  ,ALL_SOIL_LAYERS,HEAT, FROST_BOUNDARY, NOBPROC, NORMAL);
	Define(pGs, "FrostLowerBoundary1", 0., "m","HeatEq > 0");
	pGs= new Gs(&FrostUpperB_2  ,ALL_SOIL_LAYERS,HEAT, FROST_BOUNDARY, NOBPROC, NORMAL);
		Define(pGs, "FrostUpperBoundary2", 0., "m","HeatEq > 0");
	pGs= new Gs(&FrostLowerB_2  ,ALL_SOIL_LAYERS,HEAT, FROST_BOUNDARY, NOBPROC, NORMAL);
		Define(pGs,"FrostLowerBoundary2", 0., "m","HeatEq > 0");
	pGs= new Gs(&Swelling  ,ALL_SOIL_LAYERS,HEAT, FROST_BOUNDARY, NOBPROC, NORMAL);
		Define(pGs, "Swelling", 0., "m", "HeatEq > 0|FrostSwelling =1|WaterEq > 0");
	
	NL_Swell=0;
	OBOUNDF=false	;
	OldFrost=false;
	Shift=false;
	Frozen=false;


return FunctionDef();
}
bool Soil_Frost_Functions::FunctionDef() {
	Func *pFunc;


	return true;
}




//SUBROUTINE SWELLS()
double Soil_Frost_Functions::Swells() {
/*Use CommonStruc, Only : ThQual, Nfrost
USE Hydraulic
Use Timeinfo
Use soil_Waterf*/
	Soil_HeatPump* p_Heat=(Soil_HeatPump*)pHeat;  


	double TotThi, ThLiQ, ThIce, ThSolid, ThTot, ACH, ChThick;
	size_t	NLoop;
 
    TotThi=0;
    NLoop=max(NL_Frost, NL_Swell);

 //DO i=1,NLOOP
	NL_Swell=0;

	 for(size_t i=0; i<NLoop; i++) {
        ThLiQ=WaterStorage[i]*(1-p_Heat->ThQual[i])*1.E-3;
        ThIce=WaterStorage[i]*p_Heat->ThQual[i]*1.1E-3;
        if(i>=1) 
			ThSolid=(100-M_Saturation[i])/100.*(DepthLayers[i]-DepthLayers[i-1]);
		else
		     ThSolid=(100-M_Saturation[i])/100.*DepthLayers[i];
		
        ThTot=ThLiQ+ThIce+ThSolid;
        ChThick=ThTot-ThicknessLayers[i];

        if(ChThick>0.) 
          ThicknessLayers[i]=min(ThTot, AThicknessLayers[i]*(MaxSwell+1.));
        else {
          ACH=ShrinkRateFraction*(ThicknessLayers[i]-AThicknessLayers[i]);
	      ThicknessLayers[i]=max(ThicknessLayers[i]-ACH,ThTot);
		  ThicknessLayers[i]=max(ThicknessLayers[i],AThicknessLayers[i]);
        }

        if(ThicknessLayers[i]>ThicknessLayers[i]*1.0002) 
           NL_Swell=i;
        

        if(i>=1) DistanceMidpLayers[i]=(ThicknessLayers[i-1]+ThicknessLayers[i])/2.;
        TotThi+=ThicknessLayers[i];
	 }
 
      if(NLoop>=1) 
        return TotThi-DepthLayers[NLoop-1];
      else
        return 0;
      
}

//Subroutine Refreezing
/*	Use Timeinfo
	Use PhysConst
	Use Profile
	Use CommonStruc, Only	: Temp, Nfrost*/
	
vector<double> Soil_Frost_Functions::Refreezing() {
	Soil_HeatPump* p_Heat=(Soil_HeatPump*)pHeat; 
	vector<double> out;
	out.assign(NL,0.);
	double Heattrans;

	if (FrFlowDomain>=1) {
//!C     Old equation substituted 951222 by PEJ Unit of alphaht (J/dayøC)
//!C         heattrans = alphaht*(-temp[i])   
//!C     New equation below Unit of alphaht (W/møC)
		//Do i=1, Ncalc
		for(size_t i=0; i<NCalc; i++) {
		 if(p_Heat->Temp[i]<0.) {
			 NL_Frost=i;
		     Heattrans=-Fix::SECPERDAY*ThicknessLayers[i]*HeatTransFunction(p_Heat->Temp[i]);
			 out[i] = min(Heattrans/Fix::LATMELT, WaterHFD[i]/T_Step);
		 }
		}
	}
	return out;
}


void Soil_Frost_Functions::WaterFlowFrost() {
/*Use Timeinfo
Use Profile
Use CommonStruc, Only	: Theta, Temp, ThQual, Psi 
Use Soil_Waterf, Only	: Thetatotal
Use Hydraulic, Only		: M_Saturation, WcondMat, WCondB
Use Numerical, Only		: XFMAX
 
!C Routine for calculating the flow in the high-flow domain
!c LCL 1992-10-02 13.47                 
!C PEJ/MF revised version 1993-07-15*/
	Soil_HeatPump* p_Heat=(Soil_HeatPump*)pHeat; 

   double WCondMp;	//!hydraulic conductivity corresponding to a water content of thetap
	double WCondAverage;	//!d:o averaged over a layer with preferential flow and a normal layer
	double thetaF, Frac,  WCondM,  IceContent, WcondMpLiquid;		//! Local

    WaterDrainFlow.assign(NL,0.);


	if(Frost&&FrostInteract) {
//Loop_over_layers1: Do i=1, NCALC
	for(size_t i=0; i<NCalc; i++) {
//! 
//!C*************** LIQUID WATER CONTENT **********************************
//! 
        thetaF=Theta[i]*p_Heat->ThQual[i]*1.1;
        Theta[i]=(1-p_Heat->ThQual[i])*Theta[i];
        ThetaTotal[i]=thetaF+Theta[i];
 
//!C*************** WATER POTENTIAL (PSI) **********
//!        if(Hysteresis>=1) CALL HYSTER(T_Step, T_Current, i)
        if(ThetaTotal[i]>M_Saturation[i])
          Psi[i]=0.-0.1*(ThetaTotal[i]-M_Saturation[i]);
        else
          Psi[i]= f_PsiTheta_M(i,Theta[i]);
        
//!
        if(LoadPotential==1) {
          Frac=min(1.,thetaF/max(0.01,M_Saturation[i]-Theta[i]));
          Psi[i]=Psi[i]-Frac*(DepthLayers[i]-ThicknessLayers[i]*.5)*100*2.;
//!C     / 100 CONVERSION TO CM, 2 WET BULK DENSITY
        }

		//EndDo Loop_over_layers1
		}
	}

	if(FrFlowDomain==0) return;

      Frozen = false;
//!c    Put all high-flow related transfers to zero initially      
      for(size_t  i = 0; i< NCalc-1; i++) {
        WaterflowHFD[i] = 0. ;
        WaterflowHD_LD[i] = 0.;
        WaterflowLD_HD[i] = 0.;
	  }

      if(Frost&&OldFrost) 
         Shift=false;
      else if(!Frost&& !OldFrost) 
         Shift=false;
      else {
         Shift=true;
         OldFrost=Frost;
	  }

      if(!Frost) {
         if(Shift) 
             for(size_t i=0; i<Infreeze.size(); i++) {
				Infreeze[i]=WaterHFD[i]/T_Step;
				ThetaTotal[i] =  Theta[i];
			 }
         return;
      }

//Loop_over_layers:    DO  i=1,NCALC
//!    Return when calculations passed the freezing front        
	for(size_t i=0; i<NCalc; i++) {
        if (! Frozen && p_Heat->ThQual[i] > 0.001) Frozen = true;
/*! c    Calculate water content of both domains. Thetap is the total          
!c    water content of the soil (incl ice and contributions from both
!c    domains).
!c9505 Added the ice swelling effect when calculating ThetaTotal*/

		ThetaTotal[i]+=WaterHFD[i]/ThicknessLayers[i]/10.;


//!c    Skip calculation for layers that are i) totally unfrozen and         
//!c                                      ii) totally filled with ice and water 

       if(p_Heat->ThQual[i]<= 0.01) 
             Infreeze[i]=WaterHFD[i];

       if(ThetaTotal[i]>M_Saturation[i]) {
        Infreeze[i]=max(0.,min((ThetaTotal[i]-M_Saturation[i])*ThicknessLayers[i]*10, 
                                   WaterHFD[i]/T_Step));
        WaterDrainFlow[i]=Infreeze[i];
        continue;//Cycle Loop_over_layers
       }


       if(!Frozen || i==0) {
			continue;
       }


	   if (Frozen && p_Heat->ThQual[i] < 0.001 &&  p_Heat->ThQual[i-1] < 0.001) {
            Frozen=false;
            continue;//Cycle Loop_over_layers
       }

//!

 
/*!C*************** SOIL WATER FLOW ***************************************
!c     Case 1: Upper layer is thawed; there is no high-flow domain
!c            in the upper compartment. Calculate gradient between 
!c            upper compartment and high-flow domain in lower compartment.*/
      
         if (p_Heat->ThQual[i-1] < 0.001&&p_Heat->ThQual[i] > 0.001) {
			WCondM=f_CondThetaMatric(true, i-1, Theta[i-1], p_Heat->Temp[i - 1]);
			WCondMp=f_CondTheta(i, ThetaTotal[i], p_Heat->Temp[i]);
			WcondMpLiquid=f_CondThetaMatric(true, i, Theta[i], p_Heat->Temp[i - 1]);
           	IceContent=ThetaTotal[i]*p_Heat->ThQual[i];
			if(WCondMp>WcondMpLiquid)  WCondMp=f_CondHighFlowDampFactor(IceContent)*(WCondMp-WcondMpLiquid);
	        WCondAverage = sqrt(WCondMp*WCondM);
            WaterflowLD_HD[i-1]=min(WCondAverage, WaterStorage[i-1]/T_Step-Infreeze[i-1]);
			WaterflowLD_HD[i-1]=min(WaterflowLD_HD[i-1], m_pModelInfo->XFMAX);


//!c     Case 2: Lower layer is thawed; there is no high-flow domain
//!c            in the lower compartment. Calculate gradient between 
//!            high-flow domain in upper compartment and lower compartment.
		 }
         else if (p_Heat->ThQual[i-1]>0.001&&p_Heat->ThQual[i] < 0.001) {
            WCondMp=f_CondTheta(i-1, ThetaTotal[i-1],p_Heat->Temp[i-1]);
//!c            call wcondc (i, Theta[i], p_Heat->Temp[i], wcondm)
//!c            WCondAverage = sqrt(wcondmp*wcondm)
            WaterflowHD_LD[i-1]=min(WCondMp, WaterHFD[i-1]/T_Step-Infreeze[i-1]);
            WaterflowHD_LD[i-1]=min(WaterflowHD_LD[i-1], m_pModelInfo->XFMAX);
//!C      write (*,*) 'Entering waterp2', WCondAverage, WaterHFD[i-1]/T_Step, xfmax
		 }
//!c     Case 3: Flow from one frozen compartment to another.
          else {
/*!c     Contribution of air-filled pores to infiltration. 
!c     hydraulic conductivity of high-flow domain is 
!c     wcond = wcond(thetap)-wcond(Theta)
!            THETAMT=(ThicknessLayers[i]*ThetaTotal[i-1]+ThicknessLayers[i-1]*ThetaTotal[i])/   &
!                   (ThicknessLayers[i]+ThicknessLayers[i-1])*/
            WCondMp=f_CondTheta(i-1, ThetaTotal[i-1] ,p_Heat->Temp[i-1]);
            WCondM=f_CondThetaMatric(true, i-1, Theta[i-1], p_Heat->Temp[i-1]);
			IceContent=ThetaTotal[i-1]*p_Heat->ThQual[i-1];
			WCondAverage =f_CondHighFlowDampFactor(IceContent)*(WCondMp - WCondM);
            WaterflowHFD[i-1]=min(WCondAverage, m_pModelInfo->XFMAX);
			WaterflowHFD[i-1]=min(WaterflowHFD[i-1], WaterHFD[i-1]/T_Step-Infreeze[i-1]);
			WaterflowHFD[i-1]=min(WaterflowHFD[i-1],(M_Saturation[i]-ThetaTotal[i]/1.08)*ThicknessLayers[i]*10./T_Step);
			if(WaterflowHFD[i-1]<0) WaterflowHFD[i-1]=0;
/*!C940926 Introduced last line to avoid oversaturation in receiving layer
!c       and the 1:st line to avoid negative storage in layer [i]
!c       high flow domain
!C      write (*,*) 'Entering waterp3', WCondAverage, WaterHFD[i-1]/T_Step, xfmax*/
          }
	}
//Enddo Loop_over_layers
 
	
	
	return;
}
double Soil_Frost_Functions::HeatTransFunction(double Temp) {
	
	return AlphaHT*Temp;
}

double Soil_Frost_Functions::ShrinkStage(double Days) {
	
	return exp(-ShrinkRateFraction*Days);
}
void Soil_Frost_Functions::FrostBoundaries(vector<double> Heat, double HeatSurf)
{
	int IL, IU;
	double CISO=0.01;
      
     IL=0;
     IU=0;
     if(HeatSurf<=CISO&&Heat.front()>CISO) {
         IL=IL+1;
		 if(IL==1) 
		     FrostLowerB_1=-(CISO-HeatSurf)/(Heat.front()-HeatSurf)*ThicknessLayers.front()/2.;
		 else
	         FrostLowerB_2=-(CISO-HeatSurf)/(Heat.front()-HeatSurf)*ThicknessLayers.front()/2.;
		 
	  }
      else if(HeatSurf>=CISO&&Heat.front()<CISO) {
         IU=IU+1;
		 if(IU<=1) 
            FrostUpperB_1=-(CISO-HeatSurf)/(Heat.front()-HeatSurf)*ThicknessLayers.front()/2.;
		 else
            FrostUpperB_2=-(CISO-HeatSurf)/(Heat.front()-HeatSurf)*ThicknessLayers.front()/2.;
	  }
 
//Loop_over_layers:     DO  I=2,NL,1
  for(size_t i=1;i<Heat.size();i++) {	
      if(Heat[i-1]<=CISO&&Heat[i]>CISO) {
          IL=IL+1;
		  if(IL<=1)
				FrostLowerB_1=-(DepthMidpLayer[i-1]+(CISO-Heat[i-1])/(Heat[i]-Heat[i-1])*DistanceMidpLayers[i-1]);
		  else if(IL==2) 
				FrostLowerB_2=-(DepthMidpLayer[i-1]+(CISO-Heat[i-1])/(Heat[i]-Heat[i-1])*DistanceMidpLayers[i]);
	  }
      else if(Heat[i-1]>=CISO&&Heat[i]<CISO) {
          IU=IU+1;
      	  if(IU<=1) 
			FrostUpperB_1=-(DepthMidpLayer[i-1]+(CISO-Heat[i-1])/(Heat[i]-Heat[i-1])*DistanceMidpLayers[i-1]);// ! Replaced ThickM with DistanceMidpLayers
		  else
			FrostUpperB_2=-(DepthMidpLayer[i-1]+(CISO-Heat[i-1])/(Heat[i]-Heat[i-1])*DistanceMidpLayers[i-1]);// ! Replaced ThickM with DistanceMidpLayers
		 
	  }
	}// End Do Loop_over_layers
  if (Heat.back() < 0. && IL < 2) {
	  IL++;
	  if (IL <= 1)
		  FrostLowerB_1 = -DepthLayers.back();
	  else
		  FrostLowerB_2 = -DepthLayers.back();
  }
	  if(IU==0) FrostUpperB_1=0.;
	  if(IU<=1) FrostUpperB_2=0.;
	  if(IL==0) FrostLowerB_1=0.;
	  if(IL<=1) FrostLowerB_2=0.;
	  if(IL==2) 
			swap(FrostLowerB_1, FrostLowerB_2);
	  if(IU==2) 
			swap(FrostUpperB_1, FrostUpperB_2);

      if(IU==0&&IL==0) 
        OBOUNDF=false;
	  else
        OBOUNDF=true;
      
}






