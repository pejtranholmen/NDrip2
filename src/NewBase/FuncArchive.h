#pragma once
#include "../ModelTypes/Func.h"
#include "../Model/PhysConst.h"
#include <cmath>

class FunctorClass 
{
public:
	FunctorClass(enum  FUNC_CHOICE x) : OPTION(x) {}
	double operator() (double x) {
		switch(OPTION) {
		case FUNC_CHOICE::AIRDENSITYTEMP:
			return Fix::DENSA*293.15 / (273.15 + x);
		case FUNC_CHOICE::SVP: //Pa
			return _SVP(x);
		case FUNC_CHOICE::SVP_DAVID: 
			return 613.68*exp((22.452*(x)) / (x + 273.15 - 0.61));
		case FUNC_CHOICE::DSVP:
			return _DSVP(x);
		case FUNC_CHOICE::TEMP_VPS:
			return (241.88*log(x / 610.708) / (17.558 - log(x / 610.708)));		
		case FUNC_CHOICE::SUNDAY_INIT:
			return (241.88*log(x / 610.708) / (17.558 - log(x / 610.708)));
		case FUNC_CHOICE::POTRISDAILY: {
			DEC = -23.45*cos(Fix::PI*(x + 10.173) / 182.61);
			auto SSIN = SNLT*sin(Fix::RAD*DEC);
			auto CCOS = CSLT*cos(Fix::RAD*DEC);
			auto SQUOT = min(1., max(-1., SSIN / CCOS));
			auto DAYL = 1440. - 120 * (acos(SQUOT) / (Fix::RAD * 15));
			auto HOURU = 12. - DAYL / 120.;
			auto SNHSM = SSIN - CCOS / ((12 - HOURU)*Fix::RAD * 15)*sin(Fix::RAD * 15 * (HOURU + 12));
			return DAYL*SNHSM*1360.*60.;
		}
		case FUNC_CHOICE::POTRISHOUR: {
			if (x < 0)
				x = m_Hour;
			else
				m_Hour = x;
			auto OMEGA = (x + Solar_Time_Adjust / 60.)*15.*Fix::RAD;
			auto SNHS = SNLT*sin(Fix::RAD*DEC) - CSLT*cos(Fix::RAD*DEC)*cos(OMEGA);
			return max(0., SNHS*Fix::SOLAR_CONST*Fix::SECPERDAY);
		}
		case FUNC_CHOICE::DAYLENGTH: {
			DEC = -23.45*cos(Fix::PI*(x + 10.173) / 182.61);
			auto SQUOT = min(1., max(-1., SNLT*sin(Fix::RAD*DEC) / CSLT*cos(Fix::RAD*DEC)));
			return 1440. - 120 * (acos(SQUOT) / (Fix::RAD * 15));
		}
		case FUNC_CHOICE::SLOPE_CORR:
			if (SLOPE_CORR) {
				double OMEGA = (x + Solar_Time_Adjust / 60.)*15.*Fix::RAD;
				double SNHS = SNLT*sin(Fix::RAD*DEC) - CSLT*cos(Fix::RAD*DEC)*cos(OMEGA);
				double Theta_Angle = acos(SNHS);
				double ELEV = Fix::PI / 2. - Theta_Angle;
				if (Theta_Angle < Fix::PI / 2.) {
					double COSAZ = (SNLT*SNHS - sin(Fix::RAD*DEC)) / (CSLT*sin(Theta_Angle));
					double SINAZ = sin(OMEGA)*cos(DEC*Fix::RAD) / sin(Theta_Angle);
					double PHI = atan(abs(SINAZ / COSAZ));
					if (COSAZ > 0.&&SINAZ > 0.)
						PHI = 2.*Fix::PI - PHI;
					else if (COSAZ<0.&&SINAZ>0.)
						PHI = Fix::PI + PHI;
					else if (COSAZ < 0.&&SINAZ < 0.)
						PHI = Fix::PI - PHI;
					double SX = sin(PHI)*cos(ELEV);
					double SY = cos(PHI)*cos(ELEV);
					double SZ = sin(ELEV);
					double VEKTOR = (px*SX + py*SY + SZ) / (sqrt((px*px) + (py*py) + 1) * sqrt(SX*SX + SY*SY + SZ*SZ));
					return min(10., VEKTOR*sqrt(SX*SX + SY*SY + SZ*SZ) / SZ);
				}
				else
					return 1.;
			}
			else
				return 1.;
		case FUNC_CHOICE::STABILITY_MOM: 
				//real xi,x,a,b,c,d
				//integer func
				//! UNSTABLE CASE FROM PAULSEN ET AL 1970
				if (x <= 0) {
					x = pow(1. - 19.*x, 0.25);   //    ! 19 from Högström, 1996
					return log(pow((1. + x), 2)*(1. + x*x) / 8.) - 2 * atan(x) + Fix::PI / 2.;
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
					double a = 1.; double	b = 2. / 3.; double	c = 5.; double d = 0.35;
					return -(a*x + b*(x - c / d)*exp(-d*x) + b*c / d);
				}
		case FUNC_CHOICE::STABILITY_HEAT:
			if (x <= 0) {
				double x = pow((1. - 11.6*x), 0.25);  //   ! 11.6 from Högström, 1996
				return 2.*log((1. + x*x) / 2.);
			}
			else {
				/*! Stable case, func=0, Paulsen 1970
				!if(func.lt.1)then
				!	psih=-8.*xi ! 8 from Högström, 1996
				!else
				! Stable case, func=1, equation from Holtslag and De Bruin following Beljaars & Holstlag, 1991 */
				double a = 1.; double b = 2. / 3.; double	c = 5.; double d = 0.35;
				return -(pow(1. + 2. / 3.*a*x, 3. / 2.) + b*(x - c / d)*exp(-d*x) + b*c / d - 1.);
			}
		case FUNC_CHOICE::DGAS_TEMP: //Dtur=x, AirP =y, Temp=z
			return max(0.001, pow((x + 273.15) / 273.15, 1.75));
		}	
		return  x; 
	}
	double operator() (double x, double y) { 
		switch (OPTION) {
		case FUNC_CHOICE::CONC_VAPOUR:
			return x*Fix::MOLW / (Fix::RGAS*(y + 273.15));
		case FUNC_CHOICE::THETA_ANGLE: {
			if (y < 0) // x=Hour y=Day
				x = m_Hour;
			auto OMEGA = (x + Solar_Time_Adjust / 60.)*15.*Fix::RAD;
			if (y > 0)
				DEC = -23.45*cos(Fix::PI*(y + 10.173) / 182.61);
			double SNHS = SNLT*sin(Fix::RAD*DEC) - CSLT*cos(Fix::RAD*DEC)*cos(OMEGA);
			return acos(SNHS);
		}
		case FUNC_CHOICE::SENSHEAT_AIRFLUX: //
			return 86400.*x*Fix::DENSA*Fix::SPECA / max(y, 0.0001);
		case FUNC_CHOICE::LATENTHEAT_AIRFLUX: // VapourDiff = x, Ra = y
			return 86400.*x / Fix::GAMMA*Fix::DENSA*Fix::SPECA / y;
		case FUNC_CHOICE::RADLONG_OUT: // TempSurf = x, Emisisivity = y
			return y * 86400. * Fix::STEFB*pow(x+273.15, 4);
		}
		return x;
	}
	double operator() (double x, double y, double z) {
		if (OPTION==FUNC_CHOICE::WETBULB) {
			/*! ---arguments---//WETBULB . case
			real tkair(x),ea(y),bp(z)
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
			y = y / 100;
			double tcair, eas, delt, twc, tav, eav;
			//! ---koden---
			tcair = x - 273.15;
			eas = 2.7489e8*exp(-4278.63 / (tcair + 242.792));
			delt = eas*4278.63 / ((tcair + 242.792)*(tcair + 242.792));

			for (int i = 0; i < 3; i++) {
				twc = delt*tcair + 6.6e-4 *z*tcair + 7.59e-7*z*tcair*tcair + y - eas;
				twc = twc / (delt + 6.6e-4*z + 7.59e-7*z*tcair);
				tav = 0.5*(tcair + twc);
				eav = 2.7489e8*exp(-4278.63 / (tav + 242.792));
				delt = eav*4278.63 / ((tav + 242.792)*(tav + 242.792));
			}

			return twc;
		}
		else if (OPTION == FUNC_CHOICE::ATM_CORR) {
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





			double wjdruck = 1013.25;

			/*  !c	Berechnung der Deklination
			TAGNR=SNGL(DMOD(T_Current,365.2422D0))
			dekl=-23.45*COS(Fix::PI*(tagnr+10.173)/182.61)

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

			double OMEGA = (z + Solar_Time_Adjust / 60.)*15.*Fix::RAD;
			double SNHS = SNLT*sin(Fix::RAD*DEC) - CSLT*cos(Fix::RAD*DEC)*cos(OMEGA);
			double Theta_Angle = acos(SNHS);


			if (Theta_Angle<Fix::PI / 2) {
				//!c	Berechnung der potentiellen direkten Strahlung nach Iqbal, model C
				//!c	  berechnung der optischen Dicke
				double mr = 1 / (cos(Theta_Angle) + 0.15*pow(93.885 - Theta_Angle, -1.253));
				double ma = mr*(Fix::NORM_ATMPRES*exp(-m_Altitude*Fix::GRAVCONST / (287.04*(x + 273.15)))) / Fix::NORM_ATMPRES;

				//!c	  transmittance due to Raileigh Scattering
				double taur = exp(-0.0903*pow(ma, 0.84))*(1. + ma - pow(ma, 1.01));

				//!c	  transmittance by ozone
				double olt = 0.34; 		//! ozone layer thickness (cm) u.s. standard=0.34
				double u3 = olt*mr;
				double tauoz = 1 - (0.161*u3*pow(1. + 139.48*u3, -0.3035) - 0.002715*u3*
					pow(1. + 0.044*u3 + 0.0003*u3*u3, -1));

				//!c	  transmittance by mixed gases
				double taug = exp(-0.0127*pow(ma, 0.26));

				//!c	  transmittance by water vapour
				double ps = exp(26.23 - 5416 / (x + 273.15));
				double precw = 0.493*y*ps / (x + 273.15);
				double u1 = precw*mr;
				double tauw = 1 - 2.4959*u1*(1. / (pow(1.0 + 79.034*u1, 0.683) + 6.385*u1));

				//!c	  transmittance by aerosols, angstroem
				//!c	  taua=(0.97-1.265*vis**(-0.66))**ma**0.9


				double alpha = 1.3;		//! angstroem koeffizient
				double beta = 0.01;		//! angstroem koeffizient (beta <0.4)
				double ka = 0.2758*beta*pow(0.38, -alpha) + 0.35*beta*pow(0.5, -alpha);
				double taua = exp(-pow(ka, 0.873)*(1. + ka - pow(ka, 0.7088))*pow(ma, 0.9108));

				return taur*tauoz*taug*taua*tauw / (m_Angstrom1 + m_Angstrom2);
			}
			else
				return 1.;



		}
		else {
			switch (OPTION) {
			case FUNC_CHOICE::RH_PSI: //´x=PSI, Y=Temp, z=ECorr
				//double PSIPosM = max(0.1, x) / 100.;
				return exp(-max(0.1, x) / 100.*z / (Fix::RGASC*(y + 273.16)));
			case FUNC_CHOICE::D_VAP: //Dtur=x, AirP =y, Temp=z
				return x*y*Fix::DNOT*pow((z + 273.15) / 273.15, 1.75);  //!   (-)*m2/s
			case FUNC_CHOICE::D_CO2: //Dtur=x, AirP =y, Temp=z
				return max(Fix::DNOT_CO2 / 1.E4, x*y*Fix::DNOT_CO2*pow((z + 273.15) / 273.15, 1.75));
			case FUNC_CHOICE::D_NO:
			case FUNC_CHOICE::D_O2: //Dtur=x, AirP =y, Temp=z
				return max(Fix::DNOT_O2 / 1.E4, x*y*Fix::DNOT_O2*pow((z + 273.15) / 273.15, 1.75));
			case FUNC_CHOICE::RES_AIR_COM: //x=wind, y=height, z=Roughl
				x = max(x, 0.1);
				if (y / z <= 1) {ICountErr++; return 10.;}
				else
					return pow(log(y / z), 2.) / (Fix::KARMAN*Fix::KARMAN*x);

			}

			return x;



		}
		return MISSING;
	}
	double operator() (double a, double b, double  c, double d) {
		if (OPTION == FUNC_CHOICE::SHAWP_DISPL) { //a=CandDensMax, b=PAI, c=RefHeight, d=CanopyHeight
			double DISPLH = 0.803 + 0.108*a - (0.462 - 0.086*a)*
				exp(-(0.163 + 0.283*a)*b);
			return max(0., min(c - 0.5, DISPLH*d));
		}
		else if (OPTION == FUNC_CHOICE::RES_AIR_MOM_HEAT) { // Windspeed=a, Height=b, RouhgM=c, RoughH=d
			a = max(a, 0.1);
			if (b <= c || b <= d) {
				ICountErr++;
				return 10.;
			}
			else
				return log(b / c)*log(b / d) / (Fix::KARMAN*Fix::KARMAN*a);

		}
		else if (OPTION == FUNC_CHOICE::STABILITY_ANALYTICAL) { // Delta T=b, Deltaheight=a, Temp=c,*Wind=d
			double RICH = Fix::GRAVCONST*a*b / ((c + 273.15)*d*d);
			if (RICH>0)
				return pow((1 + m_StabCoef1*RICH), m_StabCoef2);	//! Stable conditions
			else
				return pow((1 - m_StabCoef3*RICH), -m_StabCoef4);
		}
		else if (OPTION == FUNC_CHOICE::AIRTEMP_YEAR) { // d= shift in days of year
				return a - b*cos(2 * Fix::PI*(c - d) / 365.25);
		}
		else if (OPTION == FUNC_CHOICE::AIRTEMP_DAY) { //d=y shift in fraction of day
			c = c - int(c);
			return a - b*cos(2 * Fix::PI*(c - d) / 1.);
		}
		else if (OPTION == FUNC_CHOICE::SOILTEMP_ANALYTICAL_DAY) { //d depth (m)
			c = c - int(c);
			return a - b*exp(-d/m_DampingDepth)*cos(2 * Fix::PI*(c-d*m_DampingDepth) / 1.);
		}
		else if (OPTION == FUNC_CHOICE::SOILTEMP_ANALYTICAL_YEAR) { //d depth (m)
			return a - b*exp(-d/m_DampingDepth)*cos(2 * Fix::PI*(c - d*m_DampingDepth) / 365.25);
		}
		else if (OPTION == FUNC_CHOICE::FRICTION_VELOCITY) { //a=eta, b=wind, c=DZ, d=zom
			if (d <= 0) return .01;

			double Ratio = c / d;
			if (Ratio <= 2) Ratio = 2;

			return b*Fix::KARMAN / (log(Ratio) - _psi_m(a) + _psi_m(a / Ratio));
		}
        return 0.0;
	}
	double operator() (double a, double b, double c, double d, double e) {
		if (OPTION == FUNC_CHOICE::SHAWP_ROUGHL) { //a=CanDensMax, b=b, c=CanopyHeight, d=RoughLmin, e=RoughLmax
			double EQ1 = (0.175 - 0.098*a) + (-0.098 + 0.045*a)*log10(b);
			double EQ2 = (0.150 - 0.025*a) + (0.122 - 0.0135*a)*log10(b);
			return min(e, max(c*min(EQ1, EQ2), d));
		}
		else if (OPTION == FUNC_CHOICE::RES_AIR_HEAT_GENERAL) { //a=MinExChangeCoef,b= Ustar, c= DiffHeight,d Zo, e Eta)
			double Ratio = c / d;
			if (Ratio <= 1) Ratio = 10.;
			return 1 / (a + b*Fix::KARMAN / (log(Ratio) - _psi_h(e) + _psi_h(e / Ratio)));
		}
		else if (OPTION == FUNC_CHOICE::WINDPROFILE) { //a=UU,b= ZU, c= zT,d Eta, e=zom)
			double RatioU = b / e;
			double RatioT = c/ e;
			if (RatioU <= 1 || RatioT <= 1)
				return a;
			double UstarKarman = a/(log(b/e) - _psi_m(d*b/c) + _psi_m(d*e /c));
			//! Windspeed at height of temperature measurements, zT
			return UstarKarman*(log(c/e) - _psi_m(d) + _psi_m(d*e/c));
		}
        return 0.0;
	}
	double operator() (double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI) {
		switch (OPTION) {
			case FUNC_CHOICE::PM:
			return (_DSVP(TAir)*(RadAvail) / Fix::LATE + 86400.*min(1., LAI)*Fix::DENSA*Fix::SPECA
				*VapourPDef / ResAir / Fix::LATE) / (_DSVP(TAir) + Fix::GAMMA*(1 + ResSurf / ResAir));
			case FUNC_CHOICE::PM_RAD:
				return (_DSVP(TAir)*(RadAvail) / Fix::LATE) / (_DSVP(TAir) + Fix::GAMMA*(1 + ResSurf / ResAir));
			case FUNC_CHOICE::PM_VENT:
				return (86400.*min(1., LAI)*Fix::DENSA*Fix::SPECA*VapourPDef / ResAir / Fix::LATE) / (_DSVP(TAir) + Fix::GAMMA*(1 + ResSurf / ResAir));
		}
		return TAir;
	}
	double operator() (double DiffZ, double TempAir, double DiffTemp, double WindSpeed, double zom, double zoh, double etaold)
	{
		if (OPTION == FUNC_CHOICE::MONIN_OBUDKOV_PAR_RICHARD) {
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
			double Ri = Fix::GRAVCONST*DiffTemp*DiffZ / ((TempAir + 273.15)*WindSpeed*WindSpeed);

			int itt = 1;
			double acc = 0.0001;
			double RatioM = DiffZ / zom;    if (RatioM <= 1) RatioM = 10.;

			double RatioH = DiffZ / zoh;    if (RatioH <= 1) RatioH = 10.;

			//! STEP 1: Calculate function Ri2Xi(XiOld)
			double Ri2eta = pow((log(RatioM) - _psi_m(etaold) + _psi_m(etaold / RatioM)), 2) /
				(log(RatioH) - _psi_h(etaold) + _psi_h(etaold / RatioH));
			//! STEP 2: Calculate error in terms of Ri using etaOld and Ri2eta(etaOld)
			double Error = etaold / Ri2eta - Ri;
			//! STEP 3: solve iteratively
			while (abs(Error) > acc&&itt < m_maxitt_monim) {
				//! 3.1 new Xi
				value = Ri2eta*Ri;
				Ri2eta = pow((log(RatioM) - _psi_m(value) + _psi_m(value / RatioM)), 2) /
					(log(RatioH) - _psi_h(value) + _psi_h(value / RatioH));
				//! 3.2 error in terms of Richardson number
				Error = value / Ri2eta - Ri;
				//! 3.3 update number of iterations
				itt++;
			}
			//! STEP 4: Return new eta when convergance criteria is fullfilled
			return Ri2eta*Ri;
			//!RichardsonToEta(2)=itt
		}
        return 0.0;
	}
 	bool operator() (enum  FUNC_CHOICE choice, double a, double b, double c, double d) {
		//CASuNInit
		OPTION = choice;
		if (choice == FUNC_CHOICE::CASUN_INIT) {
			m_Altitude = b;
			m_Angstrom1 = c;
			m_Angstrom2 = d;
			SNLT = sin(Fix::RAD*a);
			CSLT = cos(Fix::RAD*a);
			if (px > 0. || py > 0.)
				SLOPE_CORR = true;
			else
				SLOPE_CORR = false;
			return true;
		}
		else if (choice == FUNC_CHOICE::STABILITY_ANALYTICAL) {
			m_StabCoef1 = a; m_StabCoef2 = b; m_StabCoef3 = c; m_StabCoef4 = d;
		}
		else
			return false;
        return false;
	}
	bool operator() (enum  FUNC_CHOICE choice, double x) {
		if (choice == FUNC_CHOICE::SUNDAY_INIT) {
			DEC = -23.45*cos(Fix::PI*(x + 10.173) / 182.61);
			SSIN = SNLT*sin(Fix::RAD*DEC);
			CCOS = CSLT*cos(Fix::RAD*DEC);
			OPTION = choice;
			return true;
		}
		else if (choice == FUNC_CHOICE::SOILTEMP_ANALYTICAL_DAY|| choice == FUNC_CHOICE::SOILTEMP_ANALYTICAL_YEAR) {
			m_DampingDepth = x;
			OPTION = choice;
		}
		else if (choice == FUNC_CHOICE::MONIN_OBUDKOV_PAR_RICHARD) {
			m_maxitt_monim = int(x);
			OPTION = choice;
		}
		else {
			return false;
		}
        return false;
	}
	double operator() (enum  FUNC_CHOICE n) {
		OPTION = n;
        return 0.0;
	};
private:
	double _SVP(double temp) {
		if (temp<0)
			return pow(10., (12.5553 - 2667. / (273.16 + temp)));
		else
			return pow(10., (11.4051 - 2353. / (273.16 + temp)));
	}
	double _DSVP(double temp) {
		if (temp < 0)
			return _SVP(temp)*log(10.)*2267. / pow(273.16 + temp, 2.);
		else
			return _SVP(temp)*log(10.)*2353. / pow(273.16 + temp, 2.);
	};
	double _psi_m(double xi) {
		//! UNSTABLE CASE FROM PAULSEN ET AL 1970
		if (xi <= 0) {
			double x = pow(1. - 19.*xi, 0.25);   //    ! 19 from Högström, 1996
			return log(pow((1. + x), 2)*(1. + x*x) / 8.) - 2 * atan(x) + Fix::PI / 2.;
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
			double a = 1.; double	b = 2. / 3.; double	c = 5.; double d = 0.35;
			return -(a*xi + b*(xi - c / d)*exp(-d*xi) + b*c / d);
		}

	};
	double _psi_h(double xi) {
		if (xi <= 0) {
			double x = pow((1. - 11.6*xi), 0.25);  //   ! 11.6 from Högström, 1996
			return 2.*log((1. + x*x) / 2.);
		}
		else {
			/*! Stable case, func=0, Paulsen 1970
			!if(func.lt.1)then
			!	psih=-8.*xi ! 8 from Högström, 1996
			!else
			! Stable case, func=1, equation from Holtslag and De Bruin following Beljaars & Holstlag, 1991 */
			double a = 1.; double b = 2. / 3.; double	c = 5.; double d = 0.35;
			return -(pow(1. + 2. / 3.*a*xi, 3. / 2.) + b*(xi - c / d)*exp(-d*xi) + b*c / d - 1.);
		}

	};
	enum  FUNC_CHOICE OPTION;
	double SNLT, CSLT, SSIN, CCOS, DEC;
	int INIT_CASUN;
	bool SLOPE_CORR;
	int ICountErr;
	double Solar_Time_Adjust, px, py;
	double m_Altitude, m_Angstrom1, m_Angstrom2, m_Hour, m_ThetaAngle;
	double m_StabCoef1, m_StabCoef2, m_StabCoef3, m_StabCoef4;
	double m_DampingDepth;
	int m_maxitt_monim;
};


class FunArchive { 

	FunArchive();
	~FunArchive();


};

