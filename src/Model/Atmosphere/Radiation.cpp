#include "Radiation.h"
#include "../PhysConst.h"
#include "../../Simulator/Simulator.h"
#include "./Functors/Radiation_Functors.h"
						   
Radiation::Radiation(void)
{
	ModuleName="Radiation";
	ModuleNo=12;
	ModuleConditions="Radiation>0";
}

bool Radiation::Ini()	{
	if(!Meteorological_Data::Ini()) return false;

    RadFractions.resize(p_Plant->NumPlants);

    RadfractionsDirect.resize(p_Plant->NumPlants);
    DirectShortWaveSwitch = false;
    DistributedRadiationReset = true;

    RelSun=0;
    
	if(m_pModelInfo->ActualNoMulRun<1) {
		rand_init(size_t(m_pModelStructure->FRandomSeed));//   ! Setting seed to random number generator
	}
	Snow_On_RadImpact();
	return true;
}
bool Radiation::IniFunctors() {
	if (!Meteorological_Data::IniFunctors()) return false;
	f_radlongout_e = Physf(FUNC_CHOICE::RADLONG_OUT_DYN_E);
	f_radlongout = Physf(FUNC_CHOICE::RADLONG_OUT);
	f_radlongout = Get_D1_Functor(MyFunc::RADLONG_OUT_EM);
	f_AlbedoSoil = Get_D1_Functor(MyFunc::ALBEDO_SOIL);// AlbedoSoil(AlbedoDry, AlbedoKExp, AlbedoWet);
	f_AlbedoSnow = Get_D2_Functor(MyFunc::ALBEDO_SNOW_TAIR);// AlbedoSnow(AlbSnowCoef_1, AlbSnowCoef_2, AlbSnowCoef_3, AlbSnowMin, T_Step);
	f_BeerSingleLeaf = Get_D1_Functor(MyFunc::BEER_LAW_SINGLE_LEAF);
	f_BeerAbsorbedPerLayer = Get_D1_TableFunctor(MyFunc::BEER_LAW_PERLEAF);

	f_RadNetLong_Air = Get_D3_Functor(MyFunc::RADNETLONG_AIR); //RadNetLong(BruntAirCoef_1, BruntAirCoef_2, BruntAirCoef_3, BruntAirCoef_4);
	//f_RadNetLong_AirSoil = RadNetLong(BruntAirSoilCoef_1, BruntAirSoilCoef_2, BruntAirSoilCoef_3, BruntAirSoilCoef_4);

	f_RadLongIn_Konz = Get_D3_Functor(MyFunc::RADLONGIN_KONZELMANN); //RadLongIn(InLongRad, false, KonzelmannCoef_1, KonzelmannCoef_2, KonzelmannCoef_3);
	f_RadLongIn_Satt = Get_D3_Functor(MyFunc::RADLONGIN_SATTERLUND); //RadLongIn(InLongRad, false, SatterlundCoef_1, SatterlundCoef_2);
	f_RadLongIn_Brunt = Get_D3_Functor(MyFunc::RADLONGIN_BRUNT); //RadLongIn(InLongRad, false, BruntCoef_1, BruntCoef_2, BruntCoef_3);

	f_E_RadLongIn_Konz = Get_D3_Functor(MyFunc::RADLONGIN_KONZELMANN_EMISSIVITY); //RadLongIn(InLongRad, true, KonzelmannCoef_1, KonzelmannCoef_2, KonzelmannCoef_3);
	f_E_RadLongIn_Satt = Get_D3_Functor(MyFunc::RADLONGIN_SATTERLUND_EMISSIVITY); //RadLongIn(InLongRad, true, SatterlundCoef_1, SatterlundCoef_2);
	f_E_RadLongIn_Brunt = Get_D3_Functor(MyFunc::RADLONGIN_BRUNT_EMISSIVITY); //RadLongIn(InLongRad, true, BruntCoef_1, BruntCoef_2, BruntCoef_3);

/*	f_E_Cloud_RadLongIn_Konz = RadLongIn(InLongRad, true, KonzelmannCoef_1, KonzelmannCoef_2, KonzelmannCoef_3);
	f_E_Cloud_RadLongIn_Satt = RadLongIn(InLongRad, true, SatterlundCoef_1, SatterlundCoef_2);
	f_E_Cloud_RadLongIn_Brunt = RadLongIn(InLongRad, true, BruntCoef_1, BruntCoef_2, BruntCoef_3);*/

	f_DiffuseRadRatio = Get_D2_Functor(MyFunc::RAD_RATIO_DIFFUSE);
	f_Angstrom = Get_D1_Functor(MyFunc::ATM_TRANSPARENCY);// Angstrom(RadFracAng1, RadFracAng2);

	f_CanopyShadeCover = Get_D2_Functor(MyFunc::CANOPY_SHADE_COVER);// :CanopyShadeCover(CanopyDiameterHeightRatio_Common, CanopyChadeCoef);


	f_SlopeCorrDirectRad = Physf(FUNC_CHOICE::SLOPE_CORR, Latitude, AltSimPosition, RadFracAng1, RadFracAng2, Solar_Time_Adjust);

	f_ThetaAngleHour = Physf(FUNC_CHOICE::THETA_ANGLE, Latitude, AltSimPosition, RadFracAng1, RadFracAng2);
	f_potrisday = Physf(FUNC_CHOICE::POTRISDAILY, Latitude, AltSimPosition, RadFracAng1, RadFracAng2);
	f_potrishour = Physf(FUNC_CHOICE::POTRISHOUR, Latitude, AltSimPosition, RadFracAng1, RadFracAng2, Solar_Time_Adjust);
	f_atmcorrection = Physf(FUNC_CHOICE::ATM_CORR, Latitude, AltSimPosition, RadFracAng1, RadFracAng2);
	//f_RadGlobalTempAmplitude = RadGlobalTempAmplitude(ParB);

	f_daylength = Physf(FUNC_CHOICE::DAYLENGTH, Latitude, AltSimPosition, RadFracAng1, RadFracAng2);


	f_RadGlobalTempAmplitude = Get_D2_Functor(MyFunc::BRISTOW_SEASONALB);
	//	Fix::CaSun_Init(Latitude ,AltSimPosition, RadFracAng1, RadFracAng2);

	auto koll = f_AlbedoSnow(5., 3.);
	return true;
}
bool Radiation::Dyn()
{
	Meteorological_Data::Dyn();

	Flux();
	Integ();
	return true;
}

void Radiation::Flux()
{

   // !*************** POTENTIAL GLOBAL RADIATION (NO ATMOSPHERE) ************
    //! 
    double RadFracSoilDirect,/*RadFracSoilLong,*/ RadFracDirect ,RadFracDiffuse;
    double SlopeCorrFactor=1.;
	double PotRis, Hour=0.;
    double RadFracSoil,IntSnowAlbedoFrac;//, CanopyTotSoilCover;
    double /*AlbedoSoilPartition,*/ AlbedoVarVeg=12.;
	double RadNetBackup;

    if(TimeResolution_Sw(m_pModelStructure->TimeResolution)!=TimeResolution_Sw::DAILY_MEAN) {
       Hour=(m_pModelInfo->T_Current-int(m_pModelInfo->T_Current))*24.;

       PotRis=f_potrishour(Hour, m_pModelInfo->JDayNum);
	   if (PotRis < 0) {
		   PotRis = 0.;

	   }
       if(Turbidity==1)
          PotRis= f_atmcorrection(TAir, HumRel/100., Hour)*PotRis;
    }
    else 
        PotRis=f_potrisday(m_pModelInfo->JDayNum);
          
    

    //! 
    //!C*************** Relative duration of bright sunshine ******************
    //! 
    if(!MISSRIS&&MISSCLOUD) {
       //!     if cloudiness is missing and global radiation is available
      // !*************** from global radiation
       if(PotRis>=1.E6) {
          //! DG 2008-09-02: POTRIS might be SlopeCorrected, but this makes no sense for cloudiness estimation
          //!                therefor, always re-project the POTRIS to horizontal
          //! Moving back /PEJ 09-15
	      RelSun=max(0., min(1.,(RadInShort/(PotRis)-RadFracAng1)/RadFracAng2));
          CloudFrac=1-RelSun;
	   }

	}
    else if (MISSCLOUD&&DurBriSun>0&&PotRis>=2.E7&&Hour>4&&Hour<20) {
		if (TimeResolution_Sw(m_pModelStructure->TimeResolution) == TimeResolution_Sw::HOURLY) {
			RelSun = min(1., max(0., DurBriSun / 60.));
		}
		else if (TimeResolution_Sw(m_pModelStructure->TimeResolution) == TimeResolution_Sw::TEN_MINUTES) {
			RelSun = min(1., max(0., DurBriSun / 10.));
		}
		else if (TimeResolution_Sw(m_pModelStructure->TimeResolution) == TimeResolution_Sw::DAILY_MEAN)
			RelSun = min(1., max(0., DurBriSun / f_daylength(m_pModelInfo->JDayNum)));
		else
			RelSun = min(1., max(0., DurBriSun / 720.));
		CloudFrac = 1 - RelSun;

	}
    else if (MISSCLOUD&&CloudInput==4) {
    
        RadInShort=f_RadGlobalTempAmplitude(TempDayDifference, m_pModelInfo->JDayNum)*PotRis;  // ! This POTRIS is not projected onto slope
      
		RelSun=(RadInShort/(PotRis)-RadFracAng1)/RadFracAng2;         
        RelSun=max(0., min(1.,RelSun) );         
        CloudFrac=1-RelSun;    
	}
    else
       RelSun=(1-CloudFrac);
    


 //   !
  //  !C*************** Global radiation from sunshine ************************

    if(MISSRIS)  
       RadInShort=f_Angstrom(RelSun)*PotRis;   //! This POTRIS is not projected onto slope
    
    RadInDirect=0.0;
    RadInDiffuse=RadInShort;
    if(m_pModelStructure->TimeResolution!=1){  //! Removed 
             
       RadInDiffuse=RadInShort*f_DiffuseRadRatio(CloudFrac, f_ThetaAngleHour(Hour, m_pModelInfo->JDayNum));
       RadInDirect=(RadInShort-RadInDiffuse)*f_SlopeCorrDirectRad(Hour); // ! Only correcting direct radiation /PEJ
	  
       RadInShort=RadInDiffuse+RadInDirect ;//     ! Recalculating RadInShort to accout for extra direct radiation/PEJ
	   RadFracDirect = RadInDirect / RadInShort;
    }
   // ! END SNOWMIP2 ADDITION, DG 2008-09-02
	RADDIST radout;

    if(m_pModelStructure->PlantType>=3) {    	
   
      // ! START SNOWMIP2 ADDITION, DG 2008-09-02
       RadfractionsDirect.assign(p_Plant->NumPlants, 0.0);
	   RadFractions.assign(p_Plant->NumPlants,0.);
       RadFracSoil=RadFracSoilDirect=0.0;
       //! END SNOWMIP2 ADDITION, DG 2008-09-02

    //   ! --- Multiple canopies ---	
     //  !
      // ! Change by David 2001-05-31, to exclude CanopyFracRad>1
       //!
       //!	Call DistributeRad(NumPlants, LAI, PlantHeight, 1., RadFractions, MC_LAIAbove)
	   

       if(SoilPartitioningArea>1){
		  radout=RadInterceptFunc(1., ALL);

      //    Call DistributeRad(NumPlants, LAI, PlantHeight, 1., RadFractions, MC_LAIAbove,RadFracSoil,CanopyFracRad1,CanopyFracRad2)
		  CanopyFracRad1=1.0-radout.RadSoil1;
          CanopyFracRad2=1.0-radout.RadSoil2;
         // ! START SNOWMIP2 ADDITION, DG 2008-09-02
          CanopyFracRadLong = 1.0 - radout.RadSoil;
          //! END SNOWMIP2 ADDITION, DG 2008-09-02
	   }
       else {
          //! START SNOWMIP2 ADDITION, DG 2008-09-02
          //! DG: 2006-11-20, CanopyFracRad has to be calculated twice, once for diffuse, and once
          //!                 for direct if the canopyShade is made a function of solar angle
          if(CanopyShade>=1){
             if(RadInDirect>0.){
                //! Switch on the solar elevation function before the calculation of direct
                DirectShortWaveSwitch = true;
                DistributedRadiationReset = true;
             //  ! RadFractions for direct shortwave
              //  RadFracDirect = RadInDirect/(RadInDirect+RadInDiffuse);
				radout=RadInterceptFunc(RadInDirect/(RadInDirect+RadInDiffuse),DIRECT_SHORT);
				RadfractionsDirect=radout.DistFrac;
				RadFracSoilDirect=radout.RadSoil;
               // Call DistributeRad(NumPlants, LAI, PlantHeight, RadFracDirect, RadFractionsDirect, MC_LAIAbove, RadFracSoilDirect)
               // ! Canopy RadFractions for diffuse shortwave and longwave
                DirectShortWaveSwitch = false;
                DistributedRadiationReset = true;
                //! END SNOWMIP2 ADDITION, DG 2008-09-02
				radout=RadInterceptFunc(1.0,ALL );		
                //Call DistributeRad(NumPlants, LAI, PlantHeight, 1.0, RadFractions, MC_LAIAbove,RadFracSoil)
               // ! START SNOWMIP2 ADDITION, DG 2008-09-02
				CanopyFracRadLong = 1.0 - radout.RadSoil;

              //  ! RadFractions for diffuse shortwave has to be scaled with the fraction of diffuse
                RadFracDiffuse = RadInDiffuse/(RadInDirect+RadInDiffuse);
				for(size_t i=0; i<p_Plant->NumPlants; i++) RadFractions[i] =radout.DistFrac[i]*RadFracDiffuse;
				RadFracSoil = radout.RadSoil * RadFracDiffuse;

              //  ! Total RadFractions, and CanopyFracRad
                for(size_t i=0; i<p_Plant->NumPlants; i++) RadFractions[i] += RadfractionsDirect[i];
                RadFracSoil +=RadFracSoilDirect;
			 }
             else {
                RadFracDiffuse = 1.0;
                RadFracDirect = 0.0;
                DirectShortWaveSwitch = false;
                DistributedRadiationReset = true;
				radout=RadInterceptFunc(RadFracDiffuse,ALL );
              //  Call DistributeRad(NumPlants, LAI, PlantHeight, , RadFractions, MC_LAIAbove,RadFracSoil)
				CanopyFracRadLong = 1.0 - radout.RadSoil;
             }
             CanopyFracRadDirect =  RadFracDirect-RadFracSoilDirect;   
             CanopyFracRadDiffuse = RadFracDiffuse-(RadFracSoil-RadFracSoilDirect);
		  }
          else {
			 radout=RadInterceptFunc(1.,ALL );
             //Call DistributeRad(NumPlants, LAI, PlantHeight, 1., RadFractions, MC_LAIAbove,RadFracSoil)
			 CanopyFracRadLong = 1.0 - radout.RadSoil;
			 if(radout.DistFrac.size()==RadFractions.size()) RadFractions=radout.DistFrac;
	      }
        // ! END SNOWMIP2 ADDITION, DG 2008-09-02
       
	   }
	 //  radout.DistFrac;
       //! Fraction of radiation absorbed by plants (which might be bigger than 1 at this point!!) 
       CanopyFracRad=sum(radout.DistFrac);

	   AlbedoVarVeg=0.0;
       for(size_t i=0;i<p_Plant->NumPlants;i++) {
          if(CanopyFracRad>0.0) 
             AlbedoVarVeg+=radout.DistFrac[i]/CanopyFracRad*p_Plant->PlantAlbedo[i];
          else
             AlbedoVarVeg=0.0;
	   }

       //! Fraction of radiation absorbed by plants (now with regrad to unit area of soil, which is always <=1!!!)
	   CanopyFracRad=1.0-radout.RadSoil;

	}
    else if(p_Plant->AlbedoVeg==0) {	//  ! Single Canopy, albedo veg not explicit
      // ! --- Static vegetation albedo=AlbedoLeaf --- 
       AlbedoVarVeg=p_Plant->AlbedoLeaf;//				! Pej changed from albedovar to albedovarveg
       p_Plant->PlantAlbedo.front()=AlbedoVarVeg;  //  ! Pej 2 August -98 change Albedo to AlbedoLeaf
	}
    else if(p_Plant->AlbedoVeg==1) {
      // ! --- Vegetation albedo as a time-function of parameters
       if(m_pModelInfo->JDayNum<=p_Plant->DayNum.front()) 
          AlbedoVarVeg=p_Plant->AlbedoV.front();
       else if(m_pModelInfo->JDayNum>=p_Plant->DayNum.back()) 
          AlbedoVarVeg=p_Plant->AlbedoV.back();
	   else if(p_Plant->IvalInterp<p_Plant->AlbedoV.size()) {
		   AlbedoVarVeg = p_Plant->AlbedoV[p_Plant->IvalInterp - 1] + T_Step*(p_Plant->AlbedoV[p_Plant->IvalInterp]) - p_Plant->AlbedoV[p_Plant->IvalInterp - 1];
	   }
	   else
		   AlbedoVarVeg = p_Plant->AlbedoV.front();
       
       p_Plant->PlantAlbedo.front()=AlbedoVarVeg;
	}
    else if(p_Plant->AlbedoVeg==2) {
     //  ! --- Vegetation albedo given as driving variable
     //  AlbedoVarVeg=GetEcoDat(&m_PGPOS_, AlbedoVegIpos);
       p_Plant->PlantAlbedo.front()=AlbedoVarVeg; 
    }

    //! Fraction of radiation absorbed by plants when Multiple canopies is NOT used.
	if (m_pModelStructure->PlantType == 0)
		CanopyFracRad = 0;
    else if(m_pModelStructure->PlantType<3){
       CanopyFracRad=1.0-f_BeerSingleLeaf(p_Plant->LAI_Total);
       //! START SNOWMIP2 ADDITION, DG 2008-09-02
       CanopyFracRadDiffuse = CanopyFracRad;
       CanopyFracRadDirect = 0.0;
       //! END SNOWMIP2 ADDITION, DG 2008-09-02
       //! Make sure the SoilPartitioning for SoilEvaporation and Irrigation is still working
       if(SoilPartitioningArea>1){
          CanopyFracRad1=CanopyFracRad;
          CanopyFracRad2=CanopyFracRad;
       }
    }
    //! Snow interception influence on vegetation albedo
    if(p_Plant->IntThQual>0.&&p_Plant->IntStorage>0.) {
       IntSnowAlbedoFrac=AlbLeafSnowCoef*min(1.0,p_Plant->IntStorage/p_Plant->IntCapacity);
       AlbedoVarVeg=AlbedoVarVeg*(1.0-IntSnowAlbedoFrac)+IntSnowAlbedoFrac*f_AlbedoSnow(SnowAge,TAir);
      
    }

    if(m_pModelStructure->PlantType>1) {
       AlbedoVar=(1.0-CanopyFracRad)*(f_AlbedoSoil(p_Soil_WaterF->Psi.front())*FracBare +
            f_AlbedoSnow(SnowAge, TAir)*(1.0-FracBare)) + 
            CanopyFracRad*AlbedoVarVeg;  //  		! PEJ changed Albedovarveg

       //! START SNOWMIP2 ADDITION, DG 2008-09-02
       SoilAlbedoVar = f_AlbedoSoil(p_Soil_WaterF->Psi.front());
       //! END SNOWMIP2 ADDITION, DG 2008-09-02
	}
    else if(m_pModelStructure->PlantType==0) {
       AlbedoVar=f_AlbedoSoil(p_Soil_WaterF->Psi.front())*FracBare +
            f_AlbedoSnow(SnowAge, TAir)*(1.0-FracBare) ;

      // ! START SNOWMIP2 ADDITION, DG 2008-09-02
       SoilAlbedoVar = f_AlbedoSoil(p_Soil_WaterF->Psi.front());
       //! END SNOWMIP2 ADDITION, DG 2008-09-02
	 }
    else {
       AlbedoVar=Albedo*FracBare+(1.0-FracBare)*f_AlbedoSnow(SnowAge, TAir);

       //! START SNOWMIP2 ADDITION, DG 2008-09-02
       SoilAlbedoVar = Albedo;
      // ! END SNOWMIP2 ADDITION, DG 2008-09-02

    }

    //! START SNOWMIP2 ADDITION, DG 2008-09-02
    SnowAlbedoVar = f_AlbedoSnow(SnowAge, TAir);
    GroundAlbedoVar = SnowAlbedoVar*(1.0-FracBare) + SoilAlbedoVar * FracBare;
    //! END SNOWMIP2 ADDITION, DG 2008-09-02

    //!*************** Effective incoming shortwave radiation ****************

    RadNetShort=RadInShort*(1.0-AlbedoVar/100.0);

   // ! START SNOWMIP2 ADDITION, DG 2008-09-02
    RadShortOut = RadInShort-RadNetShort;
    //! END SNOWMIP2 ADDITION, DG 2008-09-02

  //  ! ************** Net Radiation *****************************************   
    if(MISSRNT) {

      // !C*************** EFFECTIVE OUTGOING LONGWAVE RADIATION *****************

       if(NetLongRad==1)	{
          if(RadInLongInput==0||MISSRIL) 
             RadInLong=f_RadLongIn_Konz(TAir,RelSun, VapourPAir);
          

/*          !DG 2001-03-14 CanopyFrac calculated above, since it may be smaller than 
          !               1-BeerLaw(LAI_Total) if Multiple Canopies is used	
          !CanopyFrac=1-BeerLaw(LAI_Total)

          ! DG corrected the weighting between snow and canopy 2003-01-28
          !RadNetTot=RadNetShort+ RadInLong-(FracBare*RadLongOut(TempBareSoil*(1-CanopyFracRaD)+CanopyFracRad*TempCanopy)+ &
          !			(1-FracBare)*RadLongOut(TempSnowSurf))


          ! DG 2003-08-18: it is more consistent to use transmissivity from Beers law both for longwave  
          !                and shortwave, therefor i use canopyFracRad insted of CanopyTotSoilCover. 
          !
          !                However, now this if statement is pointless since both cases gives the same result.
          !                Also, RadNetTot has to be corrected after longwave balance of multiple plants
          !                further down.
          !
          !			if(LongRadCanopy==0){

          ! START SNOWMIP2 ADDITION, DG 2008-09-02

          ! The canopy shade for DIRECT Shortwave is made a function of solar angle following Gryning, de Bruin et al,
          ! Thus, Global radiation has been splitted into direct and diffus. On top of that, the solar angle influence the 
          ! transmission of direct shortwave, whereas transmission of diffuse and longwave are treated as before.

          ! Modification to the SnowMIP2 addition, DG 2010-09-29:
          !    It has been revealed (by comparison to the Alptal radiation data and study with the Snowpack-model) that
          !    the solar angle influence on direct shortwave transmission is too strong - in reality the direct shortwave is
          !    probably transformed into diffuse quite early in the transmission through the canopy, thus the solar angle 
          !    influence on transmission factor (practically by increasing the LAI in beer's law) is overestimated: it is suggested to only 
          !    use the solar angle to modify the canopy shade, or in other words the sky view factor for direct shortwave, but that the
          !    extinction is treated as for diffuse. However, this is not influencing this part of the code...
		  */

          RadLongOutVar = f_radlongout(p_Plant->TempCanopy)*CanopyFracRadLong //&    ! out longwave canopy
               +((1.-FracBare)*f_radlongout(TempSnowSurf)				//&               ! out longwave snow
               +FracBare*f_radlongout(TempBareSoil))*					//	&                   ! out longwave bare soil 
               (1.-CanopyFracRadLong);	


          RadNetTot=RadNetShort+RadInLong- RadLongOutVar;            //                 ! net shortwave + in  longwave - out Longwave
		}
       else
     //     ! PEJ commented out Davids functions and now included them again
          RadNetTot=RadNetShort- f_RadNetLong_Air(TAir, VapourPAir, RelSun);
       

    }


   // !C*************** NET RADIATION AT SOIL SURFACE ************************

    if(m_pModelStructure->PlantType==3){ // ! Multiple canopies
       if(LongRadCanopy==0){
          if(SoilPartitioningArea>1) {
			  radout=RadInterceptFunc(RadNetTot,ALL,true );
			  MC_RntDist=radout.DistFrac;
			  RadNetGround=radout.RadSoil;
			  RadNetGround1=radout.RadSoil1;
			  RadNetGround2=radout.RadSoil2;


            // Call DistributeRad(NumPlants, LAI, PlantHeight, RadNetTot, MC_RntDist, MC_LAIAbove, RadNetGround, RadNetGround1, RadNetGround2)
		  }
          else {
            
            
            RadNetBackup=RadNetTot;
            radout=RadInterceptFunc(RadNetTot,ALL,false );
           //  Call DistributeRad(NumPlants, LAI, PlantHeight, RadNetTot, MC_RntDist, MC_LAIAbove, RadNetGround)
			
			if(radout.DistFrac.size()==MC_RntDist.size()) 	MC_RntDist=radout.DistFrac;
			
			RadNetGround=radout.RadSoil;
             

			 // to recover if unreasonable results
			/* bool safe=false;
             if(RadNetTot>-1.E10&&RadNetTot<1.E10&&RadNetGround>-1.E8&&RadNetGround<1.E8) 
				 safe=true;
             
                
             if(RadNetBackup>-1.E10&&RadNetBackup<1.E10) {
                    for(size_t i=0; i<p_Plant->NumPlants; i++) 
                         MC_RntDist[i]=RadNetBackup/(p_Plant->NumPlants+1);
					RadNetTot=RadNetBackup;
                    RadNetGround=RadNetBackup/(p_Plant->NumPlants+1);
			 }
             else  {
				 for(size_t i=0; i<p_Plant->NumPlants; i++) 
                        MC_RntDist[i]=0.;
					RadNetTot=0.;
		            RadNetGround=0.;
       
             }*/
         
             
             
		}
	   }
       
       else {
          //! START SNOWMIP2 ADDITION, DG 2008-09-02
          //! This branch is specifically difficult to implement the new radiation transmission model....
          //! ... It is omitted for the moment, incase there are more than 1 SoilPartitioningArea....

          if(SoilPartitioningArea>1){
           //  ! So this Branch is the original code here, DG 2008-09-02
			 radout=RadInterceptFunc(RadNetShort, ALL);
			 MC_RntDist=radout.DistFrac;
			 MC_LAIAbove=radout.LAI_Above;
			 RadNetGround=radout.RadSoil; RadNetGround1=radout.RadSoil1;RadNetGround1=radout.RadSoil1;
		  }
          else {
             //! And this is the "SNOWMIP2" code
             if(CanopyShade_Sw(CanopyShade)>= CanopyShade_Sw::Function_solar_angle){
               // ! Direct Shortwave
                DirectShortWaveSwitch = true;
                DistributedRadiationReset = true;
         		 radout=RadInterceptFunc(RadInDirect, DIRECT_SHORT);
				 for(size_t i=0; i<MC_RntDist.size(); i++) MC_RntDist[i]=radout.DistFrac[i];
				 MC_LAIAbove=radout.LAI_Above;
				 RadInShortGround=radout.RadSoil;
                //Call DistributeRad(NumPlants, LAI, PlantHeight, RadInDirect, MC_RntDist, MC_LAIAbove,RadInShortGround)

                //! Diffuse Shortwave
                DirectShortWaveSwitch = false;
                DistributedRadiationReset = false;
				radout=RadInterceptFunc(RadInDirect, ALL);
               // Call DistributeRad(NumPlants, LAI, PlantHeight, RadInDiffuse, MC_RntDist, MC_LAIAbove,RadInShortGround)
				for(size_t i=0; i<MC_RntDist.size(); i++) {
					MC_RntDist[i]+=radout.DistFrac[i];
					MC_RntDist[i]*=(1.0-AlbedoVarVeg/100.);
				}
				RadInShortGround+=radout.RadSoil;
                RadNetGround = RadInShortGround *(1.0-GroundAlbedoVar/100.);		

                //! Take care of albedo, both for canopy and ground
                //MC_RntDist(:) = MC_RntDist(:)*(1.0-AlbedoVarVeg/100.)


                //! SNOWMIP2Distri
                RadNetShortCanopy=sum(MC_RntDist);
                RadNetShortGround = RadNetGround;

                //! Longwave
                //Call DistributeLongRad(NumPlants, LAI, PlantHeight, RadInLong,(1.-FracBare)*RadLongOut(TempSnowSurf)	& ! out longwave snow
                 //    +FracBare*RadLongOut(TempBareSoil) &
                 //    , MC_TempCanopy, MC_RntDist, RadNetGround, RadInLongGround)



				radout=RadInterceptFunc(RadInLong, LONGVAWE);

                //! SNOWMIP2
                RadNetCanopy = sum(MC_RntDist);
                RadNetLongCanopy = RadNetCanopy-RadNetShortCanopy;
                RadNetLongGround = RadNetGround-RadNetShortGround;
			 }
             else {
                 //! All shortwave
                DirectShortWaveSwitch = false;
                DistributedRadiationReset = true;
				radout=RadInterceptFunc(RadNetShort, ALL);
				MC_RntDist=radout.DistFrac;
				MC_LAIAbove=radout.LAI_Above;
				RadNetGround=radout.RadSoil;
                //Call DistributeRad(NumPlants, LAI, PlantHeight, RadNetShort, MC_RntDist, MC_LAIAbove,RadNetGround)
                //! Longwave
				radout=RadInterceptFunc(RadInLong, LONGVAWE);
				MC_RntDist=radout.DistFrac;
				RadNetGround=radout.RadSoil;

				for(size_t i=0; i<MC_RntDist.size(); i++) {
					RadInLongGround+=radout.DistFrac[i];

				}
				RadInShortGround+=radout.RadSoil;
             }
		  }
          RadNetTot=sum(MC_RntDist)+RadNetGround;
       }
	}
    else if(SoilEvap>=1) //	  ! Single Leaf and Soil Evaporation excplicit
       RadNetGround=RadNetTot*(1.-CanopyFracRad);
    else //  ! No separation of soil evaporation
       RadNetGround=0;
}

RADDIST Radiation::RadInterceptFunc(double TotalAmount, enum radtypes ActRadType,  bool SoilZone) 
{	RADDIST out;

	vector<double> Hsort, Edges, RdistZone, RadAboveZone, LAI_Zone;
	vector<BORDERS> Xborders;
	vector<ZONES> RadLongZones;
	double RadOutLong;
	OUTDIST distout;

	vector<double> FracCoverSoilZone,XCenterSoilZone, RadSoilZone;
	vector<int>	   IndexArray;

	if(SoilZone) {
		if(SoilPartitioningArea>0) {
			FracCoverSoilZone.push_back(DripIrrigCover);
			FracCoverSoilZone.push_back(1-DripIrrigCover);
			XCenterSoilZone.push_back(DripIrrigXCenter);
			XCenterSoilZone.push_back(DripIrrigXCenter+0.5);
			if(XCenterSoilZone.back()>1.0) XCenterSoilZone.back()-=0.5;
		}
		else {
			for(size_t i=0; i<2;i++) {
				FracCoverSoilZone.push_back(0.5);
				XCenterSoilZone.push_back(0.5);
			}
		}
	}
    auto Hour = (m_pModelInfo->T_Current - int(m_pModelInfo->T_Current)) * 24.;

	// Update below is using variables from Plant module and also estimate new outputs in that module (MOVE ?)
	for(size_t i=0; i<p_Plant->SoilCoverFraction.size(); i++) {
		p_Plant->SoilCoverFraction[i]=p_Plant->f_CanopyCover(i, p_Plant->LAI[i]);
		if(p_Plant->SoilCoverFraction[i]<=0||p_Plant->SoilCoverFraction[i]>3)
			p_Plant->SoilCoverFraction[i]=0.01;
		if(p_Plant->SoilCoverFraction[i]<1.&&DirectShortWaveSwitch) 
			p_Plant->SoilCoverFraction[i]=max(1., p_Plant->SoilCoverFraction[i]/f_CanopyShadeCover(f_ThetaAngleHour(Hour , m_pModelInfo->JDayNum), p_Plant->PlantHeight[i]));
	}
	p_Plant->TotSoilCovFrac=sum(p_Plant->SoilCoverFraction);

	// Estimate borders (2.2) Using options from SpatialPlantDist in the Plant
	Xborders=DefineBorders();
	if(SoilZone) {
		BORDERS soilb;
		for(size_t i=0; i<2; i++) {
			soilb.Left=XCenterSoilZone[i]-FracCoverSoilZone[i]*0.5;
			soilb.Right=XCenterSoilZone[i]+FracCoverSoilZone[i]*0.5;
			Xborders.push_back(soilb);
		}
	}
	// 3.1

	Edges=DefineEdges(&Xborders);
	Hsort=DefineSortedHeight();
	
	out.RadSoil=0;
	if(ActRadType!=LONGVAWE) {
		RadAboveZone=DefineRadAbove(&Edges, TotalAmount);
		distout=DistributeRad(&RadAboveZone,&Hsort, &Xborders,&Edges);
		out.DistFrac=distout.can_distribution;
		if(p_Plant->SpatialPlantDist==2) {
			for(size_t i=0; i<RadAboveZone.size()-1; i++)
				if(Edges[i]<=1&&Edges[i+1]>=0)
					out.RadSoil+=RadAboveZone[i];
		}
		else 
		  out.RadSoil=sum(RadAboveZone);
	}
	else {
		RadOutLong=(1.-FracBare)*f_radlongout(TempSnowSurf)+FracBare*f_radlongout(TempBareSoil);	
		RadLongZones=DefineRadLongZones(&Edges, TotalAmount,RadOutLong); 
		out.RadSoil=sum(RadAboveZone);
	}

	if(SoilZone) {
		SOILSURFACEZONE Surf;
		Surf=DistributeRad_At_Surface(&RadAboveZone, &Xborders,&Edges, &FracCoverSoilZone);
		out.TotalSoilCover=Surf.TotalSoilCover;
		out.RadSoil1=Surf.RadSoil1;
		out.RadSoil2=Surf.RadSoil2;
	}

	return out;
}
vector<BORDERS> Radiation::DefineBorders() {
	vector<BORDERS> out;
	BORDERS x;
		if(p_Plant->SpatialPlantDist==2) {
			for(size_t i=0; i<p_Plant->NumPlants; i++) {
				x.Left=p_Plant->Xcenter[i]-p_Plant->SoilCoverFraction[i]*0.5;
				x.Right=p_Plant->Xcenter[i]+p_Plant->SoilCoverFraction[i]*0.5;
				out.push_back(x);
			}
		}
		else if (p_Plant->SpatialPlantDist==1) {
			for(size_t i=0; i<p_Plant->NumPlants; i++) {
				p_Plant->SoilCoverFraction[i]=min(2.,p_Plant->SoilCoverFraction[i]);
				x.Left=p_Plant->Xcenter[i]-p_Plant->SoilCoverFraction[i]*0.5;
				x.Right=p_Plant->Xcenter[i]+p_Plant->SoilCoverFraction[i]*0.5;
				out.push_back(x);
			}
		}
		else {
			double fran;
			for(size_t i=0; i<p_Plant->NumPlants; i++) {
				fran=frand();
				p_Plant->SoilCoverFraction[i]=min(2.,p_Plant->SoilCoverFraction[i]);
				x.Left=fran*(1-p_Plant->SoilCoverFraction[i]);
				x.Right=x.Left+p_Plant->SoilCoverFraction[i];
				out.push_back(x);
			}
		}
		return out;
}
vector<double> Radiation::DefineEdges(vector<BORDERS> *pBorder) {
	vector<double> out, out_des;
	
	out.push_back(0.);
	for(size_t i=0; i<pBorder->size(); i++) {
		out.push_back(pBorder->at(i).Left);
		out.push_back(pBorder->at(i).Right);
	}
	out.push_back(1.);

	if(p_Plant->SpatialPlantDist==0) {
		for (size_t i=0; i<out.size();i++) {
			if(out[i]>1.) out[i]-=1.;
			if(out[i]<0.) out[i]+=1;
		}
	}

	SortDArray(out.data(), out.size());

	//Delete similar values
	for (size_t i=0; i<out.size()-1;i++)
		if(out[i]<out[i+1]+1.E-6&&out[i]>out[i+1]-1.E-6)
			out.erase(out.begin()+i+1);


	for(size_t i=out.size()-1; i>0; i--)
		out_des.push_back(out[i]);

	return out_des;
}
vector<double> Radiation::DefineRadAbove(vector<double> *pEdges, double Rad)
{
	vector<double> out;
	double value;
	for(size_t i=0; i<pEdges->size()-1; i++) {
		value=(pEdges->at(i)-pEdges->at(i+1))*Rad;
		out.push_back(value);
	}
	out.push_back(pEdges->back()*Rad);
	return out;
}
vector<ZONES> Radiation::DefineRadLongZones(vector<double> *pEdges, double Rad, double LongOut)
{
	vector<ZONES> out;
	ZONES value;
	for(size_t i=0; i<pEdges->size()-1; i++) {
		value.above=(pEdges->at(i)-pEdges->at(i+1))*Rad;
		for(size_t j=0; j<MC_TempCanopy.size();j++) {
			if(SnowReduceLAIFactor[j]>=SnowReduceLAIThreshold)
				value.canopy.push_back(f_radlongout_e(MC_TempCanopy[j],Emissivity)*(pEdges->at(i)-pEdges->at(i+1)));
			else
				value.canopy.push_back(0.);
		}
		value.below=(pEdges->at(i)-pEdges->at(i+1))*LongOut;
		out.push_back(value);
	}
	return out;
}
vector<double> Radiation::DefineSortedHeight()
{ vector<double> outas, outds;
	for(size_t i=0; i<p_Plant->PlantHeight.size();i++) 
		outas.push_back(p_Plant->PlantHeight[i]);

	outas.push_back(0.);
	SortDArray(outas.data(), outas.size());
	for(size_t i=outas.size()-1; i<outas.size();--i)
		outds.push_back(outas[i]);
	
	return outds;
}
OUTDIST Radiation::DistributeRad(vector<double> *pRadAboveZone, vector<double> *pHSort, vector<BORDERS> *pBorder,vector<double> *pEdges, vector<ZONES> *pRadLongZones)
{
	vector<double> RDistZone, LAI_Zone;
	OUTDIST  out;
	vector<int> IndexArray;

	double KrntLAI_Zone_All,LAI_Zone_Add;
//! 5 loop through the vertical segments from top to bottom and distribute the radiation
  //  !
    //do i=2,N_new	
	bool LongWave=false;
	if(pRadLongZones!=nullptr) {
		LongWave=true;
		pRadAboveZone->resize(pRadLongZones->size());
		for(size_t i=1;i<pRadAboveZone->size(); i++) pRadAboveZone->at(i)=pRadLongZones->at(i).above;
	}
	LAI_Zone.resize(pRadAboveZone->size()+1);
	RDistZone.resize(pRadAboveZone->size());
	//int Num;
	for(size_t i=1;i<pHSort->size(); i++) {
		IndexArray.resize(0);
       //!
       //! 5.1 identify canopies above the present height
       //!
      //do j=1,N											
	  for(size_t j=0;j<p_Plant->PlantHeight.size();j++) {			//	! How many horizon are above the specific height 
		  if(p_Plant->PlantHeight[j]>pHSort->at(i))  	//	! Which canopies
			  IndexArray.push_back(j);
	  }
/*       !
       ! 5.2 for a specific vertical segment, loop through all horizontal zones  
       !	  
       !
       ! Dg 2006-12-18: Only plants that are above the snow cover should be included!!
       !                Please note new if statement in the code below..
       !                Above snow cover means SnowreduceLAIFactor>=0.01....
       !
       ! DG 2010-09-29: It was revealed that the original radiation distribution function 
       !                is INCONSISTENT for MULTIPLE PLANTS if DIFFERENT EXTINCTION COEFFICIENTS are used.
       !
       !                the original model is first summing up LAI in the Zones, and { distributed radiation according to the individual 
       !                extinction coefs and total LAI, and { the absorbed is scaled by the fractional LAI.... basically this is wrong for two reasons...
       !
       !                however, a correct weighting of multiple plants transmission should be made with the 
       !                extinction_coef * LAI and not LAI only, in fact, the total adsorbed is given directly if the sum of k(j)*LAI(j) is used in beer's law.
       !                { distribution on the plants can be done by Rtot * (lai(j)*k(j))/sum(k(:)*lai(:))
       !
       //do k=2,Zones+1*/
	  double HighEdge, LowEdge, DeltaEdge;
	   for(size_t k=1; k<pRadAboveZone->size()+1;k++) {	
          LAI_Zone.assign(LAI_Zone.size(),0.);
          KrntLAI_Zone_All=0.0;
		  HighEdge=pEdges->at(k-1);
		  if(k<pEdges->size())
			LowEdge=pEdges->at(k);
		  else
			LowEdge=0;
		  DeltaEdge=HighEdge-LowEdge;
          //!   (5.2.1) accumulate LAI from all plants within this zone
          //!
          //!  DG 2010-09-29: the total LAI in the zone is no longer interesting:
          //!      we need sum(k(:)*LAI_zone(:)) and the individual LAI for each plant in the zone
          //do j=1,
		  for(size_t j=0; j<IndexArray.size(); j++) {	
             size_t CanID=IndexArray[j];
             CanID=min(CanID,pRadAboveZone->size()  );
			 double hscaling=DeltaEdge/p_Plant->SoilCoverFraction[CanID];
			 double vscaling=(pHSort->at(i-1)-pHSort->at(i))/p_Plant->PlantHeight[CanID];
			 
             if(pBorder->at(CanID).Right>LowEdge&&pBorder->at(CanID).Left<HighEdge) {
//                ! This is the new code from DG (061218)
                if(SnowReduceLAIFactor[CanID]>=SnowReduceLAIThreshold){
                   LAI_Zone[j]=p_Plant->LAI[CanID]*vscaling*hscaling;
                   KrntLAI_Zone_All=KrntLAI_Zone_All+LAI_Zone[j]*RntLAIv[CanID];
                }
             }
             if(p_Plant->SpatialPlantDist<=1){
                if(pBorder->at(CanID).Right>LowEdge-1&&pBorder->at(CanID).Left<HighEdge-1) {
                   if(SnowReduceLAIFactor[CanID]>=SnowReduceLAIThreshold){
                      LAI_Zone_Add=p_Plant->LAI[CanID]*vscaling*hscaling;
                      KrntLAI_Zone_All=LAI_Zone_Add*RntLAIv[CanID];
                      LAI_Zone[j]=LAI_Zone[j]+LAI_Zone_Add;
                   }
                }
                if(pBorder->at(CanID).Right>1+LowEdge&&pBorder->at(CanID).Left<1+HighEdge) {
                   if(SnowReduceLAIFactor[CanID]>=SnowReduceLAIThreshold){
                      LAI_Zone_Add=p_Plant->LAI[CanID]*vscaling*hscaling;
					  KrntLAI_Zone_All+=LAI_Zone_Add*RntLAIv[CanID];
                      LAI_Zone[j]=LAI_Zone[j]+LAI_Zone_Add ; 
                   }
                }
             }
		  }
          //!   (5.2.2) distribute radiation on plants within this zone
          if(KrntLAI_Zone_All>0.){
             //do j=1,Num		
			 for(size_t j=0; j<IndexArray.size(); j++) {
                size_t CanID=IndexArray[j];
                //! DG 2008-09-02 NOTE the introduction of ViewFactorScaling in the next statement, this is a function of 
                //! the new model for DIRECT solar radiation as function of solar elevation (for diffuse and longwave this factor is 1) 
                //! DG 2010-09-29, it should also be 1 for direct see notes above, so better remove it completely
                
                //! DG 2010-09-29, the total absorbed radiation in the zone is now distributed between the plants
                //!                using the ratio k(j)*lai(j)/sum(k(:)*lai(:))..... this is correct, check the matlab test below:
                
                if(SnowReduceLAIFactor[CanID]>=SnowReduceLAIThreshold) {
					double frac;
//! DG 100929           RDistZone(CanID)=RadAboveZone(k-1)*(1-exp(-ViewFactorScaling(CanID)*RntLAIv(CanID)*LAI_Zone_All/(Edges(k-1)-Edges(k))))*LAI_Zone(j)/LAI_Zone_All
					if(pRadLongZones==nullptr) {// shortwave
						frac=(1-exp(-KrntLAI_Zone_All/DeltaEdge))*LAI_Zone[j]*RntLAIv[CanID]/KrntLAI_Zone_All;
						RDistZone[CanID]=pRadAboveZone->at(k-1)*frac;
					}
					else					// longwave explicit
						RDistZone[CanID]=(pRadAboveZone->at(k-1)-pRadLongZones->at(k-1).canopy[j])*(1-exp(-RntLAIv[CanID]*KrntLAI_Zone_All/DeltaEdge))
						*LAI_Zone[j]/KrntLAI_Zone_All;
					
				}
                else
                   RDistZone[CanID]=0.0;
             }
		
             //!  (5.2.3) output radiation to next vertical segment within this zone
             //do j=1,Num
			out.can_distribution.resize(p_Plant->PlantHeight.size());
			for(size_t j=0; j<IndexArray.size(); j++) {
                size_t CanID=IndexArray[j];
				pRadAboveZone->at(k-1)-=RDistZone[CanID];
                out.can_distribution[CanID]+=RDistZone[CanID];
			}

	    } // Adsorption Valid
		else {



		}

		
       } // Horizontal zones
	}//  Height
		
	//Makes sure a horizon exist also when height is zero 

	while(out.can_distribution.size()<p_Plant->PlantHeight.size())
		out.can_distribution.push_back(0.);

	return out;
	}
SOILSURFACEZONE Radiation::DistributeRad_At_Surface(vector<double> *pRadAboveZone, vector<BORDERS> *pBorder,vector<double> *pEdges, vector<double>*pFracCoverSoilZone, vector<ZONES> *pRadLongZones) {
	SOILSURFACEZONE out;
	vector<double> RadSoilZone;

	size_t N=p_Plant->NumPlants;
    double HighEdge, LowEdge, DeltaEdge;
	for(size_t j=0; j<2; j++) {
		RadSoilZone.assign(RadSoilZone.size(),0.);
		for(size_t k=0; k<pRadAboveZone->size();k++) {	
			  HighEdge=pEdges->at(k);
			  if(k+1<pEdges->size())
				LowEdge=pEdges->at(k+1);
			  else
				LowEdge=0;
			  DeltaEdge=HighEdge-LowEdge;

			//! Collect radiation from zones within the unit area only
             if(HighEdge<=1&&LowEdge>0) {  
                //! 1) both edges of soil zone between 0 and 1
                if(HighEdge<=pBorder->at(p_Plant->NumPlants+j).Right&&LowEdge>=pBorder->at(p_Plant->NumPlants+j).Left )
                   RadSoilZone[j]+=pRadAboveZone->at(k);
                
                //! 2) right border > 1
                if((HighEdge<=(pBorder->at(N+j).Right-1))&&LowEdge>=(pBorder->at(N+j).Left-1)) 
                   RadSoilZone[j]+=pRadAboveZone->at(k);
                
               // ! 3) left border < 0
                if(HighEdge<=(pBorder->at(N+j).Right+1.0)&&LowEdge>=(pBorder->at(N+j).Left+1.0))
					 RadSoilZone[j]+=pRadAboveZone->at(k);
			 }

		}
		if(j==0) 
			out.RadSoil1=RadSoilZone[0];
		else
			out.RadSoil2=RadSoilZone[1];
	}
			// Now scale to m-2 by dividing with the soil fraction area (only if>0)
	if(pFracCoverSoilZone->at(0)>0)
		out.RadSoil1/=pFracCoverSoilZone->at(0);
	else
		out.RadSoil1=0;

	if(pFracCoverSoilZone->at(1)>0)
		out.RadSoil2/=pFracCoverSoilZone->at(1);
	else
		out.RadSoil2=0;

	out.TotalSoilCover=0;
//! 6.1 calculate total canopy cover of soil within unit area
	for(size_t k=0; k<pRadAboveZone->size();k++) {	
			  HighEdge=pEdges->at(k);
			  if(k+1<pEdges->size())
				LowEdge=pEdges->at(k+1);
			  else
				LowEdge=0;
			  DeltaEdge=HighEdge-LowEdge;

             if(HighEdge<=1&&LowEdge>0) {  //! 1) both edges of soil zone between 0 and 1
				 for(size_t j=0; j<N; j++) {
					
					if(pBorder->at(p_Plant->NumPlants+j).Right>LowEdge&&pBorder->at(p_Plant->NumPlants+j).Left<HighEdge&&p_Plant->LaiV[j]>0) {
							out.TotalSoilCover+=DeltaEdge;
							break;
					}
// The Section below is exclude because of the CanID is not defined in the context here - PEJ
/*                If(SpatialPlantDist.le.1)then
                   if(Xborders(CanID,2).GT.Edges(k+1)-1.AND.Xborders(CanID,1).LT.Edges(k)-1 .AND.LAI(j).gt.0.0) then
                      TotalSoilCover=TotalSoilCover+edges(k)-edges(k+1)
                      exit dgloop
                   endif
                   if(Xborders(CanID,2).GT.1+Edges(k+1).AND.Xborders(CanID,1).LT.1+Edges(k) .AND.LAI(j).gt.0.0) then
                      TotalSoilCover=TotalSoilCover+edges(k)-edges(k+1)
                      exit dgloop
                   endif
                endif*/
			 }
		}
	}
	return out;
}
	


void Radiation::Integ()
{
}

bool Radiation::End()
{
	Meteorological_Data::End();
return true;
}
void Radiation::Snow_On_RadImpact(double SnowHeight) 
{
 //**************** SnowReduceLAIFactor test by DG 29/9 ******************
 //SnowCorrection of leaf area index suggested by DG 29/9-97, update to MC 18/4-01
 // Multiple canopies

  if(m_pModelStructure->PlantType==3){
     SnowReduceLAIMax = 0.0;
     for(size_t i=0; i<p_Plant->NumPlants; i++) {
		 if (p_Plant->PlantHeight[i] > 0)
			 SnowReduceLAIFactor[i] = max(0.01, 1. - SnowHeight / p_Plant->PlantHeight[i]);
		 else if (SnowHeight <= 0.0001)
			 SnowReduceLAIFactor[i] = 1.;
		 else
			SnowReduceLAIFactor[i]=0.;
        if(SnowReduceLAIFactor[i]>SnowReduceLAIMax) SnowReduceLAIMax=SnowReduceLAIFactor[i];
	 }
  }  // Single Canopy
  else {
     if(p_Plant->Roughness==2||p_Plant->Displacement==2) {	
        SnowReduceLAIFactor.front()=max(0.01,1.- SnowHeight/p_Plant->CanopyHeightVar);
	 }
     else if(m_pModelStructure->PlantType==2) 
        SnowReduceLAIFactor.front()=max(0.01,1.- SnowHeight/(p_Plant->RoughLVar*10));	
     else
        SnowReduceLAIFactor.front()=0.;
     SnowReduceLAIMax=SnowReduceLAIFactor.front();
  }

}
Radiation::~Radiation(void)
{
}
