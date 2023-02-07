#include "./PhysFunctions.h"

/*
double PhysFunctions::SinTemp( double TAirMean, double TAirAmpl, double TAirPhase, double YCH, double Damp, double Time, double day, double depth) {
	double timm;

	if(YCH<1.0001)
		timm=Time-int(Time);
	else
		timm=day;

	if(Damp>1.E-5)
		return TAirMean-TAirAmpl*exp(-depth/Damp)*cos(2*PI*(timm-TAirPhase)/YCH- depth/Damp);
	else
		return TAirMean-TAirAmpl*cos(2*PI*(timm-TAirPhase)/YCH);

}
double PhysFunctions::AirDensityTempFunc(double temp)
{
	return DENSA*293.15/(273.15+temp);
}
double PhysFunctions::SVP(double temp)
{
	  if(temp<0) 
		return pow(10.,(12.5553-2667./(273.16+temp)));
	  else
		return pow(10.,(11.4051-2353./(273.16+temp)));
}*/



//!***********************************************************************
//! WETBULB computes wet-bulb temperatures from dry-bulb (tkair) and 
//! vapor pressure of air(ea). routine adapted from e. anderson, p. 188.
//! (adopted from SNTHERM.89, David G 23/4-01)
//!***********************************************************************
double WetBulb(double tkair, double ea, double bp) {
	/*! ---arguments---
	real tkair,ea,bp
	! tkair : Air temperature [K]
	! ea : Water Vapor pressure in air  [mb]
	! bp : approximate average barometric pressure [mb]
	! ---local---
	! delt: delt=eas*4278.63/((tcair+242.792)*(tcair+242.792)).
	! eas: saturated Water Vapor pressure in air  [mb].
	! eav: eav=2.7489d8*dexp(-4278.63/(tav+242.792)).
	! i: looping index.
	! tav: average of dry-bulb temperature and wet bulb temperature in celsius.
	! tcair: dry-bulb temperature in celsius.
	! twc: wet bulb temperature in celsius.
	integer i*/

	// Changed to use ea in Pa by PEJ 7/3 2013
	ea=ea/100;
	double tcair,eas,delt,twc,tav,eav;
	//! ---koden---
	tcair=tkair-273.15;
	eas=2.7489e8*exp(-4278.63/(tcair+242.792));
	delt=eas*4278.63/((tcair+242.792)*(tcair+242.792));
	
	for(int i=0; i<3; i++) {
	   twc=delt*tcair+6.6e-4 *bp*tcair+7.59e-7*bp*tcair*tcair+ea-eas;
	   twc=twc/(delt+6.6e-4*bp+7.59e-7*bp*tcair);
		tav=0.5*(tcair+twc);
	   eav=2.7489e8*exp(-4278.63/(tav+242.792));
	   delt=eav*4278.63/((tav+242.792)*(tav+242.792));
	}

	return twc;
}



double PhysFunctions::SVP_David(double temp)
{	//SVP mbar - Changed to Pa by PEJ
	double f;

	  f=(22.452*(temp))/(temp+273.15-0.61);
	  return 613.68*exp(f);		
}
double PhysFunctions::DSVP(double temp)
{
	  if(temp<0) 
		return SVP(temp)*log(10.)*2267./pow(273.16+temp, 2.);
	  else
		return SVP(temp)*log(10.)*2353./pow(273.16+temp, 2.);
}
double PhysFunctions::Conc_Vap(double pressure, double temp)
{
	return pressure*MOLW/(RGAS*(temp+273.15));
}
bool PhysFunctions::CaSun_Init(double Lat, double Alt, double Ang1, double Ang2) {
	  m_Altitude=Alt;
	  m_Angstrom1=Ang1;
	  m_Angstrom2=Ang2;
	  SNLT=sin(RAD*Lat);
      CSLT=cos(RAD*Lat);

	  if(px>0.||py>0.) 
        SLOPE_CORR=true;
	  else
        SLOPE_CORR=false; 
	  return true;

}	
double PhysFunctions::ThetaAngle(double Hour, double Day) {
	if(Hour<0)
		Hour=m_Hour;
    double OMEGA=(Hour+Solar_Time_Adjust/60.)*15.*RAD;
	if(Day>0)
		DEC=-23.45*cos(PI*(Day+10.173)/182.61);
	double SNHS=SNLT*sin(RAD*DEC)-CSLT*cos(RAD*DEC)*cos(OMEGA);
    return acos(SNHS);
  
}
bool PhysFunctions::SunDayInit(double Day){

	     DEC=-23.45*cos(PI*(Day+10.173)/182.61);
         SSIN=SNLT*sin(RAD*DEC);
         CCOS=CSLT*cos(RAD*DEC);
	     return true;
}	  
double PhysFunctions::PotRisDaily(double Day) {
	  
	  //Real Day
      //REal DayL, HOURU, SNHSM, SQUOT,ACOS
      
      DEC=-23.45*cos(PI*(Day+10.173)/182.61);
      double SSIN=SNLT*sin(RAD*DEC);
      double CCOS=CSLT*cos(RAD*DEC);
      double SQUOT= min(1., max(-1.,SSIN/CCOS) );
      double DAYL=1440.-120*(acos(SQUOT)/(RAD*15));
      double HOURU=12.-DAYL/120.;
      double SNHSM=SSIN-CCOS/((12-HOURU)*RAD*15)*sin(RAD*15*(HOURU+12));
      return DAYL*SNHSM*1360.*60.;
}
double PhysFunctions::DayLengthFunction(double Day) {
	 //minutes
        DEC=-23.45*cos(PI*(Day+10.173)/182.61);
        double SSIN=SNLT*sin(RAD*DEC);
        double CCOS=CSLT*cos(RAD*DEC);
        double SQUOT=min(1.,max(-1.,SSIN/CCOS) );
		return 1440.-120*(acos(SQUOT)/(RAD*15));     
 }
double PhysFunctions::SlopeCorr(double Hour) {
 //     Use CommonStruc, Only : px, py, Solar_Time_Adjust
 //     REAL THETA_ANGLE, HOUR, OMEGA, SNHS, COS, ELEV 
 //     REAL COSAZ, SINAZ, SIN, PHI, ATAN, SX, SY,SZ,  VEKTOR
      
      if(SLOPE_CORR){
	  
	    double OMEGA=(Hour+Solar_Time_Adjust/60.)*15.*RAD;
	    double SNHS=SNLT*sin(RAD*DEC)-CSLT*cos(RAD*DEC)*cos(OMEGA);
        double Theta_Angle=acos(SNHS);
	    double ELEV=PI/2.-Theta_Angle;
	    if(Theta_Angle<PI/2.) {
		    double COSAZ=(SNLT*SNHS-sin(RAD*DEC))/(CSLT*sin(Theta_Angle));
		    double SINAZ=sin(OMEGA)*cos(DEC*RAD)/sin(Theta_Angle);
		    double PHI=atan(abs(SINAZ/COSAZ));
		    if (COSAZ>0.&&SINAZ>0.)
				PHI=2.*PI-PHI;
		    else if (COSAZ<0.&&SINAZ>0.)
				PHI=PI+PHI;
			else if(COSAZ<0.&&SINAZ<0.) 
				PHI=PI-PHI;
		    double SX=sin(PHI)*cos(ELEV);
		    double SY=cos(PHI)*cos(ELEV);
		    double SZ=sin(ELEV);
		    double VEKTOR=(px*SX+py*SY+SZ)/( sqrt((px*px)+(py*py)+1) * sqrt(SX*SX+SY*SY+SZ*SZ) );
		    return min(10.,VEKTOR*sqrt(SX*SX+SY*SY+SZ*SZ)/SZ);
		}
		else
		  return 1.;
	}
	else
		return 1.;
           
}  
double PhysFunctions::AtmosphereCorrection(double temp,double relhum, double Hour) {
 /*   !*****************************************************************************
    !*	Berechnung der potentiellen direkten Strahlung in einem 
    !*	Gelaendemodell.
    !*	I0 nach der Parametrisation C Iqbal(1983), Version fuer Stundenwerte
    !*	Die files ha.dat, fs.dat, sx.dat und sy.dat werden durch die Programme
    !*	von Martin Funk erstellt
    !*	Ch. Pluess Mai 1995, angepasst an Gaudergrat-Daten: Januar 1997
    !*****************************************************************************
    real dekl,omega,tagnr,olt,hour
    real ma,mr,ka1,ka2,ka
    real wjrh,wjdruck,wjtemp	
    real z,zstat
    real druck,strahlung
    real vis,beta,alpha,lapse,ig,Theta_Angle,taur,u3,tauoz
    real taug,ps,precw,u1,tauw,taua,cosaz,elev
    !	real sinaz,sx,sy,sz,vektor,elev

    integer idt*/


   

	
    double wjdruck=1013.25;

  /*  !c	Berechnung der Deklination
    TAGNR=SNGL(DMOD(T_Current,365.2422D0))
    dekl=-23.45*COS(PI*(tagnr+10.173)/182.61)

    !	dangle=2.*pi*(tagnr-1.)/365.                       	!rad
    !	dekl=(0.006918-0.399912*cos(dangle)+0.070257*sin(dangle)- &
    !    	0.006758*COS(2*DANGLE)+0.000907*sin(2*dangle)-0.002697*  &
    !    	cos(3.*dangle)+0.00148*sin(3.*dangle))*(180./pi)           !deg

    !	tair=wjtemp+(zstat-z)*lapse	*/



  //  !c	Berechnung des Stundenwinkels		                         
  //  omega=(12.-hour)*15.					!deg
  //  !c	Berechnung des Zenithwinkels
  //  Theta_Angle=acosd(sind(dekl)*sind(Latitude)+cosd(dekl)*cosd(Latitude)* &
//         cosd(omega))
  //  elev=90.-Theta_Angle;

		double OMEGA=(Hour+Solar_Time_Adjust/60.)*15.*RAD;
	    double SNHS=SNLT*sin(RAD*DEC)-CSLT*cos(RAD*DEC)*cos(OMEGA);
        double Theta_Angle=acos(SNHS);


    if (Theta_Angle<PI/2) {
       //!c	Berechnung der potentiellen direkten Strahlung nach Iqbal, model C
       //!c	  berechnung der optischen Dicke
       double mr=1/(cos(Theta_Angle)+0.15*pow(93.885-Theta_Angle,-1.253));		
       double ma=mr*(NORM_ATMPRES*exp(-m_Altitude*GRAVCONST/(287.04*(temp+273.15)) ))/NORM_ATMPRES;

       //!c	  transmittance due to Raileigh Scattering
       double taur=exp(-0.0903*pow(ma,0.84))*(1.+ma-pow(ma,1.01));

       //!c	  transmittance by ozone
	   double olt=0.34; 		//! ozone layer thickness (cm) u.s. standard=0.34
       double u3=olt*mr;
       double tauoz=1-(0.161*u3*pow(1.+139.48*u3,-0.3035)-0.002715*u3* 
            pow(1.+0.044*u3+0.0003*u3*u3,-1));

       //!c	  transmittance by mixed gases
       double taug=exp(-0.0127*pow(ma,0.26));

       //!c	  transmittance by water vapour
       double ps=exp(26.23-5416/(temp+273.15));
       double precw=0.493*relhum*ps/(temp+273.15);
       double u1=precw*mr;
       double tauw=1-2.4959*u1*(1./(pow(1.0+79.034*u1,0.683)+6.385*u1));

       //!c	  transmittance by aerosols, angstroem
       //!c	  taua=(0.97-1.265*vis**(-0.66))**ma**0.9


	   double alpha=1.3;		//! angstroem koeffizient
       double beta=0.01;		//! angstroem koeffizient (beta <0.4)
	   double ka=0.2758*beta*pow(0.38,-alpha)+0.35*beta*pow(0.5,-alpha);
       double taua=exp(-pow(ka,0.873)*(1.+ka-pow(ka,0.7088))*pow(ma,0.9108));

       return taur*tauoz*taug*taua*tauw/(m_Angstrom1+m_Angstrom2);
	}
    else
       return 1.;
    
}
double PhysFunctions::PotRisHour(double Hour) {

	  if(Hour<0) 
		  Hour=m_Hour;
	  else
		  m_Hour=Hour;
	  double OMEGA=(Hour+Solar_Time_Adjust/60.)*15.*RAD;
	  double SNHS=SNLT*sin(RAD*DEC)-CSLT*cos(RAD*DEC)*cos(OMEGA);
      return max(0., SNHS*SOLAR_CONST*SECPERDAY);

}
 



//! GENERAL FUNCTIONS FOR TURBULENT EXCHANGE OF LATENT AND SENSIBLE HEAT 
//!
//! 2000-02-16 /DAVID.G
//!
//! 1. Sensible heat flux as a function of temperature difference and aerodynamic resistance
//!
/*
double PhysFunctions::Sens_AirF(double TempDiff, double Ra) {

 return 86400.*(TempDiff)*DENSA*SPECA/max(Ra,0.0001);
}

//! 2. Latent heat flux as a function of vapour pressure difference and aerodynamic resistance
//!
//RealLatent_AirF(VapourDiff, Ra)
//Real VapourDiff, Ra
double PhysFunctions::Latent_AirF(double VapourDiff, double Ra) {
	 return 86400.*(VapourDiff)/GAMMA*DENSA*SPECA/Ra;
}*/
/*!--------------------------------------------------------------------------------------
! GENERAL FUNCTIONS OF AERODYNAMIC RESISTANCE
!
! 2001-01-15 BY D.GUSTAFSSON
!
! METHOD 1: ANALYTICAL APPROXIMATION OF STABILITY CORRECTIONS
!           ACCORDING TO METHOD IN FORMER SOIL MODEL (JANSSON, 1998)
!           BASED ON PRUITT ET AL 1976:
!
!			USAGE:
!           1. RaUnCorr = RAFunction(DiffHeight, Roughnesslength, WindSpeed) 
!                       	
!           2. RaCorr   = RaUncorr*AnalyticalStabilityCorr(DiffHeight, DiffTemp, TempAir, WindSpeed)
!
!           
! METHOD 2: NUMERICALLY DERIVED STABILITY CORRECTIONS BASED ON 
!           EMPIRICALLY FOUND STABILITY FUNCTIONS ADOPTED FROM
!           BELJAARS AND HOLTSLAG 1991
!
!           USAGE:
!           1. RaCorr= function 3.-7.
!
!                 see for example RaHeatFunctionBareSoil(...) 
!                 on how to use these functions
!--------------------------------------------------------------------------------------

! 1. Aerodynamic resistance at neutral stability
!    function of common roughness lengths for momentum and heat:
!*/
/*
double PhysFunctions::RAFunction(double WindSpeed,double Height, double RoughLVar) {
	WindSpeed=max(WindSpeed, 0.1);
	if(Height/RoughLVar<=1) { 
		ICountErr++;
		return 10.;
	}
	else
		return pow(log(Height/RoughLVar),2.)/(KARMAN*KARMAN*WindSpeed);
}

double PhysFunctions::RAFunction( double WindSpeed, double Height, double RoughLVarMom,double RoughLVarHeat) {
	WindSpeed=max(WindSpeed, 0.1);
	if(Height<=RoughLVarMom||Height<=RoughLVarHeat) {
		ICountErr++;
		return 10.;
	}
	else
		return log(Height/RoughLVarMom)*log(Height/RoughLVarHeat)/(KARMAN*KARMAN*WindSpeed);
}*/
/*!
! 2. Analytical stability correction of the aerodynamic resistance
!
Real Function StabilityCorrAnalytical(DiffHeight, DiffTemp, TempAir, WindSpeed, StabCoef1, & 
                                      StabCoef2, StabCoef3, StabCoef4)
Real DiffHeight,DiffTemp, TempAir, WindSpeed, StabCoef1, StabCoef2, StabCoef3, StabCoef4
Real Rich*/
double PhysFunctions::StabilityCorrAnalytical(double DiffHeight, double DiffTemp, double  TempAir, double WindSpeed, double StabCoef1, 
                                    double StabCoef2, double StabCoef3, double StabCoef4) {
	double RICH=GRAVCONST*(DiffHeight)*(DiffTemp)/((TempAir+273.15)*WindSpeed*WindSpeed);
    if(RICH>0) 
       return pow((1+StabCoef1*RICH),StabCoef2);	//! Stable conditions
	else
       return pow((1-StabCoef3*RICH),-StabCoef4);  //	! Unstable conditions
   
}
/*
! 3. Richardson2Eta() -> calculates stabilityparameter z/L from Ri
!     
!        where   z= reference height, L= Monin-Obukhov length, Ri=Richardson number
!*/

double PhysFunctions::psi_m(double xi) {
//real xi,x,a,b,c,d
//integer func
//! UNSTABLE CASE FROM PAULSEN ET AL 1970
if(xi<=0) {
	double x=pow(1.-19.*xi,0.25);   //    ! 19 from Högström, 1996
	return log(pow((1.+x),2)*(1.+x*x)/8.)-2*atan(x)+PI/2.;
}
else {
/*! 
!  O B S!! BEFORE 2001-01-16 THE psim(unstable) WAS ERRONEOUSLY WRITTEN:
!
!  psim=log((1.+x)**2*(1.+x**2)/8.)-2/tan(x)+3.141593/2.
!
! Stable case func=0 from Businger et al 1971 and others
 !	if(func.lt.1)then
 !		psim=-5.3*xi  ! 5.3 from Högström, 1996
 !	else
! stable case from Holstlag and Bruin, following Beljaars & Holtslag 1991*/
		double a=1.; double	b=2./3.; double	c=5.; double d=0.35;
		 return -(a*xi+b*(xi-c/d)*exp(-d*xi)+b*c/d);
	}
}

//! 7. STABILITY FUNCTIONS FOR HEAT
/*real function psih(xi) !,func)
real xi,x,a,b,c,d
!integer func
! Unstable case. Integrated by Paulsen, 1970 from phi-functions found by Businger et al, 1971*/
double PhysFunctions::psi_h(double xi) {
	if(xi<=0) {
		double x=pow((1.-11.6*xi),0.25);  //   ! 11.6 from Högström, 1996
		return 2.*log((1.+x*x)/2.);
	}
else {
/*! Stable case, func=0, Paulsen 1970
	!if(func.lt.1)then
	!	psih=-8.*xi ! 8 from Högström, 1996
	!else
! Stable case, func=1, equation from Holtslag and De Bruin following Beljaars & Holstlag, 1991 */
		double a=1.; double b=2./3.; double	c=5.; double d=0.35;
		return -(pow(1.+2./3.*a*xi,3./2.)+b*(xi-c/d)*exp(-d*xi)+b*c/d-1.);
}
}



double PhysFunctions::RichardsonToEta(double DiffZ, double TempAir, double DiffTemp, double WindSpeed,double zom,double zoh, double etaold,double maxitt)
{
	double value;
/*! SOLVES EQUATION eta=Ri*Ri2eta(eta) ITERATIVELY
! 
! ARGUMENTS:
real Diffz,TempAir,DiffTemp,WindSpeed , zom, zoh, etaOld
integer maxitt, func
! LOCAL VARIABLES:
real Ri2eta,Error,Ri,acc, RatioM, RatioH
integer itt
! CALCULATE RICHARDSON NUMBER*/
 double Ri=GRAVCONST*DiffTemp*DiffZ/((TempAir+273.15)*WindSpeed*WindSpeed);

int itt=1;
double acc=0.0001;
double RatioM=DiffZ/zom;    if(RatioM<=1) RatioM=10.;
    
double RatioH=DiffZ/zoh;    if(RatioH<=1) RatioH=10.;

//! STEP 1: Calculate function Ri2Xi(XiOld)
double Ri2eta=pow((log(RatioM)-psi_m(etaold)+psi_m(etaold/RatioM)),2)/ 
   (log(RatioH)-psi_h(etaold)+psi_h(etaold/RatioH));
//! STEP 2: Calculate error in terms of Ri using etaOld and Ri2eta(etaOld)
double Error=etaold/Ri2eta-Ri;
//! STEP 3: solve iteratively
while(abs(Error)>acc&&itt<maxitt) {
   //! 3.1 new Xi
   value=Ri2eta*Ri;
   Ri2eta=pow((log(RatioM)-psi_m(value)+psi_m(value/RatioM)),2)/ 
      (log(RatioH)-psi_h(value)+psi_h(value/RatioH));
   //! 3.2 error in terms of Richardson number
   Error=value/Ri2eta-Ri;
   //! 3.3 update number of iterations
   itt++;
}
//! STEP 4: Return new eta when convergance criteria is fullfilled
return Ri2eta*Ri;
//!RichardsonToEta(2)=itt
}
/*! 4. FrictionVelocityFunc -> calulates frictionvelocity from: 
!
!             eta=stabilityparameter z/L, Wind=Windspeed
!             DZ=referenceheight-displacement, zom=roughness length for momentum
!  */                                 

double PhysFunctions::FrictionVelocityFunction(double eta,double Wind, double DZ, double zom) {

	if(zom<=0) return .01;

    double Ratio=DZ/zom;
    if(Ratio<=2) Ratio=2;
    
	return Wind*KARMAN/(log(Ratio)-psi_m(eta)+psi_m(eta/Ratio));
}

double  PhysFunctions::RaHeatfunctionGeneral(double MinExChangeCoef,double Ustar, double DiffHeight,double Zo, double Eta) {


	double Ratio = DiffHeight / Zo;
	if (Ratio <= 1) Ratio = 10.;



	return 1 / (MinExChangeCoef + Ustar*KARMAN / (log(Ratio) - psi_h(Eta) + psi_h(Eta / Ratio)));
}
/*!
! 5. WIndProfileFunction-> calulates windspeed at the height zT
!
!	UU=windspeed at reference height for wind
!	zU=reference height of the windspeed input
!	zT=reference height of the air temperature input
!	eta=Monin-Obukhov stability parameter z/L
!	zom=roughness length for momentum
!*/
double  PhysFunctions::WindProfileFunction(double UU, double zU, double zT, double eta, double zom) {
//! input Windspeed, zUheight Wind,height temp,M-O stabilityparameter=zT/L,roughness momentum 
/*real UU,zU,zT,eta,zom, RatioU, RatioT
integer psifunc
!Local FrictionVelocity/Karman
real UstarKarman
! Get Friction velocity over Karman, Ustar/Karman*/

    double RatioU=zU/zom;
    double RatioT=zT/zom;
    if(RatioU<=1||RatioT<=1) 
        return UU;

	double UstarKarman=UU/(log(zU/zom)-psi_m(eta*zU/zT)+psi_m(eta*zom/zT));
//! Windspeed at height of temperature measurements, zT
	return UstarKarman*(log(zT/zom)-psi_m(eta)+psi_m(eta*zom/zT));
}

double PhysFunctions::RadLongOut(double TempSurf, double Emissivity) {
    return Emissivity*86400*STEFB*pow(TempSurf+273.15,4);
}
 // End Function RadLongOut

