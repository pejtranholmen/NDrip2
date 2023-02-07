#pragma once
#include "./Surface_Water.h"



class Irrigation_Functions : public Surface_Water
{
	public:
		Irrigation_Functions(void);
		~Irrigation_Functions(void);
		bool Def();

	//Use CommonstrucN, Only	: Numplants, GrowthStageIndex, Drip, Dripper

	//Use Drivings


	size_t UnitIrrig, Dripper, IrrigationInput;

	double	IStoreLayer, IStoreMin, IrrigRate, IrrigAmount;
    double	 DrippIrrigAmount,  DripIrrigRate;
	vector<double> DrippInfilDistF;
	vector<P*> mTab_Dripper;
	
 	double DripContainer, DripFill, DripOutlet;
	
    bool IrriON, OpenTank, PlantExist;
	double WStore,  IrriSum;

	bool  DripInUse;  //! From Irrigation
	size_t  PlantIndexDrip; // ! Irrigation

// Connection to other external modules

};

