#pragma once

#include "../SimModule.h"
#include "./Functors/Profile_Functors.h"

class Profile_Functions : public SimModule
{
	public:
		Profile_Functions(void);
		~Profile_Functions(void);
		bool Def();
		bool FunctionDef();
		bool  GetFunctionValue(size_t option);
		
		DEPTH_type DepthDist(size_t choice, double sum,  double s_depth, double s_exptail);
		bool Profile_ReCalc();
		size_t NoLayerM, NL;

		vector<double> UpperDepth, LowerDepth;
		vector<double> GravelContent, ClayContent, SandContent, OrganicContent, QuartsFraction;

		vector<double> ThicknessLayers;
		vector<double> DepthMidpLayer, DistanceMidpLayers, AThicknessLayers, DepthLayers;
		vector<double> ModelUpperDepth, ModelLowerDepth;

		// Local Variable
		size_t NCalc;
		vector<double> OriginalThickness;	
		vector<P*> mTab_BC_Model,mTab_HC_Model, mTab_Gen_Model, mTab_Compartment, mTab_MeasTexture, mTab_ModelTexture;
	
	//	function<void(vector<double> &pOriginalThickness, vector<double> &pThicknessLayers, vector<double> *pAThicknessLayers, vector<double> *pDistanceMidpLayers,
	//		vector<double> *pDepthLayers, vector <double> *pModelUpperDepth, vector<double> *pDepthMidpLayer, vector<double> *pModelLowerDepth)> f_ReCalcProfile;
		function<DEPTH_type(double, double)> f_RootDepthDistribution;

};

