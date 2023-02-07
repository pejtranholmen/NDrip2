#pragma once
#include <math.h>
double PedoFunc_AirEntry(double SAND, double CLAY, double PORO) {

	   double C[13]= {5.3396738,   0.1845038, -2.48394546, -0.00213853, 
       -0.04356349, -0.61745089, 0.00143598, -0.00855375,
       -0.00001282,  0.00895359, -0.00072472, 0.0000054, 0.50028060};
      double A= C[0]+C[1]*CLAY+ C[2]*PORO + C[3]*CLAY*CLAY +C[4]*SAND*PORO 
       +C[5]*PORO*CLAY + C[6]*SAND*SAND*PORO*PORO +C[7]*CLAY*CLAY*PORO*PORO 
       +C[8]*SAND*SAND*CLAY + C[9]*CLAY*CLAY*PORO 						  
       +C[10]*SAND*SAND*PORO  + C[11]*CLAY*CLAY*SAND +C[12]*PORO*PORO*CLAY;
	   return exp(A);
}
double PedoFunc_Lambda(double SAND, double CLAY, double PORO) {
       double C[12]= {-0.7842831, 0.0177544, -1.062468, -0.00005304, 
       -0.00273493, 1.11134946, -0.03088295, 0.00026587,
	   -0.00610522, -0.00000235, 0.00798746,-0.00674491};

	  double A;
      A= C[0]+C[1]*SAND+ C[2]*PORO + C[3]*SAND*SAND +C[4]*CLAY*CLAY 
       +C[5]*PORO*PORO + C[6]*SAND*PORO + C[7]*SAND*SAND*PORO*PORO
       +C[8]*CLAY*CLAY*PORO*PORO + C[9]*SAND*SAND*CLAY 
       +C[10]*CLAY*CLAY*PORO   + C[11]*CLAY*PORO*PORO ;
     return exp(A);

}
double PedoFunc_Residual(double SAND, double CLAY, double PORO) {
       double C[9]= {-0.0182482 , 0.00087269, 0.00513488, 0.02939286, 
         -0.00015395,-0.0010827 ,-0.00018233, 0.00030703,	
         -0.0023584};
	  double  A ;
	   A= C[0];
	   A+=C[1]*SAND;
	   A+=C[2]*CLAY;
	   A+=C[3]*PORO;
	   A+=C[4]*CLAY*CLAY;
	   A+=C[5]*SAND*PORO;
	   A+=C[6]*CLAY*CLAY*PORO*PORO;
	   A+=C[7]*CLAY*CLAY*PORO;
	   A+=C[8]*CLAY*PORO*PORO;
	   A=max(0.,100.*A);
	   return A;

}
double PedoFunc_HydraulicCond(double SAND, double CLAY, double PORO) {
       double C[13]= {        19.52348, -8.96847 , -0.028212, 0.00018107, 
       -0.0094125, -8.395215, 0.077718, -0.00298,	  
       -0.019492,  0.0000173, 0.02733, 0.001434, -0.0000035};
	  double A ; 
	  A= PORO ;	  A=A*C[0];	  A=A+C[1];	  A=A+ C[2]*CLAY;  A=A +C[3]*SAND*SAND;
	  A=A +C[4]*CLAY*CLAY;	  A=A +C[5]*PORO*PORO;
	  A=A +C[6]*SAND*PORO;	  A=A +C[7]*SAND*SAND*PORO*PORO;
	  A=A +C[8]*CLAY*CLAY*PORO*PORO;	  A=A +C[9]*SAND*SAND*CLAY ;
	  A=A +C[10]*CLAY*CLAY*PORO ;	  A=A +C[11]*SAND*SAND*PORO ;
	  A=A +C[12]*CLAY*CLAY*SAND;
      return 240.*exp(A);

}
double PedoFunc_WiltingPoint(double CLAY) {
	return 0.45*CLAY+2;
}