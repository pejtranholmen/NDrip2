#pragma once
#include "../../../std.h"
#include "../Profile_Functions.h"
#include "../../../Util/StatUtil.h"

class FromMeasuredLayersToModelLayers {
public: 
	FromMeasuredLayersToModelLayers(std::vector<double> UpperDepth, std:: vector<double> LowerDepth, std::vector<double> DepthLayers, std::vector<double> ThicknessLayers, std::vector<double> DepthMidpLayer) :
		UpperDepth(UpperDepth), LowerDepth(LowerDepth), DepthLayers(DepthLayers), ThicknessLayers(ThicknessLayers), DepthMidpLayer(DepthMidpLayer) {}
	void operator() (std::vector<double> XX, std::vector<double> &pm_yy, std::vector<double> &pb_yy, bool aritmetric) {
		double Displacement = 0.;
		size_t NumSteps = 20;
		pm_yy.resize(DepthLayers.size()); pb_yy.resize(DepthLayers.size());

		if (UpperDepth[0]>LowerDepth[0])
			Displacement = UpperDepth[0] - LowerDepth[0];
		double sum;	bool valid;
		for (size_t i = 0; i<DepthLayers.size(); i++) { //SimLayers
			sum = 0;
			double sdep;
			for (size_t i1 = 0; i1<NumSteps; i1++) { //20 Steps within the SimLayers
				double vikt = double(i1) / double(NumSteps - 1);
				sdep = DepthLayers[i] - ThicknessLayers[i] + vikt*ThicknessLayers[i];
				valid = false;
				if (Displacement>0 && sdep <= Displacement) {
					if (aritmetric)
						sum += XX[1];
					else
						sum += 1 / XX[1];
					valid = true;
				}
				else
					sdep = sdep - Displacement;
				if (!valid) {
					for (size_t ii = 0; ii<UpperDepth.size(); ii++) {
						if (sdep >= UpperDepth[ii] && sdep<LowerDepth[ii] || i1 <= NumSteps / 2 && sdep == UpperDepth[ii]
							|| i1>NumSteps / 2 && sdep == LowerDepth[ii]) {
							valid = true; // A depth is found between measured layers
							if (aritmetric)
								sum += XX[ii];
							else
								sum += 1 / XX[ii];
							break;
						}
					}
				}
				if (!valid) { // Interpolation between measured horsizon lower depths
					for (size_t ii = 0; ii<UpperDepth.size()-1; ii++) {
						if (sdep >= LowerDepth[ii] && sdep<UpperDepth[ii + 1]) {
							vikt = (sdep - LowerDepth[ii]) / (UpperDepth[ii + 1] - LowerDepth[ii]);
							if (aritmetric)
								sum += XX[ii] * (1 - vikt) + vikt*XX[ii + 1];
							else
								sum += 1 / (XX[ii] * (1 - vikt) + vikt*XX[ii + 1]);
							valid = true;
							break;
						}
					}
				}
				if (!valid) {  // Extrapolations
					if (sdep<UpperDepth[0]) {
						if (aritmetric)
							sum += XX.front();
						else
							sum += 1 / (XX.front());
					}
					else if (sdep>LowerDepth.back()) {
						if (aritmetric)
							sum += XX.back();
						else
							sum = sum + 1 / (XX.back());
					}
				}
			}//within Layers
			if (aritmetric)
				pm_yy.at(i) = sum / NumSteps;
			else
				pm_yy.at(i) = 1 / (sum / NumSteps);
		}  //Sim Layers

		for (size_t i = 0; i<DepthLayers.size(); i++) {  // Lower Boundary Layers
			if (DepthLayers[i] <= DepthMidpLayer.front())
				pb_yy.at(i) = pm_yy.front();
			else if (DepthLayers[i]>DepthMidpLayer.back())
				pb_yy.at(i) = pm_yy.back();
			else {
				for (size_t ii = 0; ii<DepthLayers.size() - 1; ii++) {
					if (DepthLayers[i] >= DepthMidpLayer[ii] && DepthLayers[i]<DepthMidpLayer[ii + 1]) {
						double vikt = (DepthLayers[i] - DepthMidpLayer[ii]) / (DepthMidpLayer[ii + 1] - DepthMidpLayer[ii]);
						pb_yy.at(i) = pm_yy.at(i)*(1 - vikt) + pm_yy.at(ii + 1)*vikt;
						break;
					}
				}
			}
		}







	} // 
	std::vector<double> operator() (vector<double> XX, vector<double> Bulk_Density, bool aritmetric) { // Return vector of mass per Model Layer
		vector<double> out;
		double Displacement = 0.;
		size_t NumSteps = 20;

		if (UpperDepth[0]>LowerDepth[0])
			Displacement = UpperDepth[0] - LowerDepth[0];
		double sum, sumbd;	bool valid; double count;
		for (size_t i = 0; i<DepthLayers.size(); i++) { //SimLayers
			sum = 0;
			sumbd = 0;
			double sdep;
			count = 0;
			for (size_t i1 = 0; i1<NumSteps; i1++) { //20 Steps within the SimLayers
				double vikt = double(i1) / double(NumSteps - 1);
				sdep = DepthLayers[i] - ThicknessLayers[i] + vikt*ThicknessLayers[i];
				valid = false;
				if (Displacement>0 && sdep <= Displacement) {
					if (aritmetric) {
						sum += XX[1];
						sumbd += Bulk_Density[1];
					}
					else
						sum += 1 / XX[1];
					valid = true;
					count++;
				}
				else
					sdep = sdep - Displacement;
				if (!valid) {
					for (size_t ii = 0; ii<UpperDepth.size(); ii++) {
						if (sdep >= UpperDepth[ii] && sdep<LowerDepth[ii] || i1 <= NumSteps / 2 && sdep == UpperDepth[ii]
							|| i1>NumSteps / 2 && sdep == LowerDepth[ii]) {
							valid = true; // A depth is found between measured layers
							if (aritmetric) {
								sum += XX[ii];
								sumbd += Bulk_Density[ii];
							}
							else
								sum += 1 / XX[ii];
							count++;
							break;
						}
					}

				}
				if (!valid) { // Interpolation between measured horsizon lower depths
					for (size_t ii = 0; ii<UpperDepth.size() - 1; ii++) {
						if (sdep >= LowerDepth[ii] && sdep<UpperDepth[ii + 1]) {
							vikt = (sdep - LowerDepth[ii]) / (UpperDepth[ii + 1] - LowerDepth[ii]);
							if (aritmetric) {
								sum += XX[ii] * (1 - vikt) + vikt*XX[ii + 1];
								sumbd += Bulk_Density[ii] * (1 - vikt) + vikt*Bulk_Density[ii + 1];
							}
							else
								sum += 1 / (XX[ii] * (1 - vikt) + vikt*XX[ii + 1]);
							valid = true;
							count++;
							break;
						}
					}
				}
				if (!valid) {  // Extrapolations
					if (sdep<UpperDepth[0]) {
						if (aritmetric) {
							sum += XX.front();
							sumbd += Bulk_Density.front();
						}
						else
							sum += 1 / (XX.front());
						count++;
					}
					else if (sdep>LowerDepth.back()) {
						if (aritmetric) {
							sum += XX.back();
							sumbd += Bulk_Density.back();

						}
						else
							sum = sum + 1 / (XX.back());
						count++;
					}
				}
			}//within Layers
			if (aritmetric&&count>0)
				out.push_back(10000 * sum*sumbd*ThicknessLayers[i] / (count*count));
		}  //Sim Layers
		return out;
	}
private:
	std::vector<double> UpperDepth, LowerDepth; // Measured Representation Depths
	std::vector<double> DepthLayers, ThicknessLayers, DepthMidpLayer; // Model Representation

};
class ModelProfileLayers {
public: ModelProfileLayers(size_t NL) :NL(NL) {}
		void operator() (vector<double> &OriginalThickness, vector<double> &ThicknessLayers, vector<double> &AThicknessLayers, vector<double> &DistanceMidpLayers,
			vector<double> &DepthLayers, vector <double> &ModelUpperDepth, vector<double> &DepthMidpLayer, vector<double> &ModelLowerDepth) {
			OriginalThickness.resize(NL);
			OriginalThickness = ThicknessLayers;
			for (size_t i = 1; i<NL; i++)
				if (ThicknessLayers[i] == 0)
					ThicknessLayers[i] = ThicknessLayers[i - 1];

			AThicknessLayers = ThicknessLayers;
			DistanceMidpLayers[0] = ThicknessLayers[0] / 2.;
			DepthLayers[0] = ThicknessLayers[0];
			ModelUpperDepth[0] = 0.;

			DepthMidpLayer[0] = ThicknessLayers[0] / 2.;
			for (size_t i = 1; i<NL; i++) {
				DistanceMidpLayers[i] = (ThicknessLayers[i - 1] + ThicknessLayers[i]) / 2.;
				DepthLayers[i] = DepthLayers[i - 1] + ThicknessLayers[i];

			}
			ModelLowerDepth[0] = DepthLayers[0];
			for (size_t i = 1; i<NL; i++) {
				ModelUpperDepth[i] = DepthLayers[i - 1];
				DepthMidpLayer[i] = DepthLayers[i - 1] + ThicknessLayers[i] / 2.;
				ModelLowerDepth[i] = DepthLayers[i - 1] + ThicknessLayers[i];
			}
		}

private:
	size_t NL;
};
class ModelProfile {
public:
	ModelProfile(size_t &NL, vector<double>& OriginalThickness, vector<double>& ThicknessLayer ): NL(NL), OriginalThickness(OriginalThickness),
		ThicknessLayers(ThicknessLayer) {
		OriginalThickness.resize(NL);
		OriginalThickness = ThicknessLayers;
		for (size_t i = 1; i < NL; i++)
			if (ThicknessLayers[i] == 0)
				ThicknessLayers[i] = ThicknessLayers[i - 1];
	
		}
	void operator() (void) {
		OriginalThickness.resize(NL);
		OriginalThickness = ThicknessLayers;
		for (size_t i = 1; i < NL; i++)
			if (ThicknessLayers[i] == 0)
				ThicknessLayers[i] = ThicknessLayers[i - 1];
	}


private:

	size_t NL;
	vector<double> OriginalThickness, ThicknessLayers;

};


class LinearDepthDistribution {
public :
	LinearDepthDistribution(vector<double> DepthLayers, vector<double> ThicknessLayers) : DepthLayers(DepthLayers), ThicknessLayers(ThicknessLayers) {
		NL = ThicknessLayers.size();}
	DEPTH_type operator() (double totsum, double S_Depth) {
		DEPTH_type out;
		out.NumLay = 0;
		out.Dist.assign(NL,0.);
		S_Depth = min(-0.01, S_Depth);

			double dec = 1. / S_Depth;
			double rupper = 1;
			double rlower;
			double thickn;
			size_t i = 0;
			while (DepthLayers[i]<-S_Depth&&i<DepthLayers.size() - 1) {
				rlower = max(0., 1 + dec*DepthLayers[i]);
				thickn = max(0., min(ThicknessLayers[i], -S_Depth - DepthLayers[i] + ThicknessLayers[i]));
				out.Dist[i]=-(rupper + rlower)*thickn*dec*totsum;
				rupper = rlower;
				i++;
			}
			out.NumLay = i;
		return out;
	}
private:
	vector<double> DepthLayers, ThicknessLayers;
	size_t NL;
};
class UniformDepthDistribution {
public:	UniformDepthDistribution(vector<double> DepthLayers, vector<double> ThicknessLayers) : DepthLayers(DepthLayers), ThicknessLayers(ThicknessLayers) { NL = ThicknessLayers.size(); }
		DEPTH_type operator() (double totsum, double S_Depth) {
		DEPTH_type out;
		out.NumLay = 0;
		out.Dist.assign(NL, 0.);
		S_Depth = min(-0.01, S_Depth);
		double rdens = -1 / S_Depth*totsum;
		size_t i = 1;
		out.Dist[0]=min(-DepthLayers[0] + ThicknessLayers[0] - S_Depth, ThicknessLayers[0])*rdens;
		while (DepthLayers[i - 1]<-S_Depth&&i<ThicknessLayers.size()) {
			out.Dist[i]=min(-DepthLayers[i] + ThicknessLayers[i] - S_Depth, ThicknessLayers[i])*rdens;
			i++;
		}
		out.NumLay = i;
		return out;
	}
private: vector<double> DepthLayers, ThicknessLayers;
		 size_t NL;
};
class ExponentialDepthDistribution {
public:	ExponentialDepthDistribution(double ExpTail, vector<double> DepthLayers, vector<double> ThicknessLayers) : ExpTail(ExpTail), DepthLayers(DepthLayers), ThicknessLayers(ThicknessLayers) {
	NL = ThicknessLayers.size();}
		DEPTH_type operator() (double totsum, double S_Depth) {
			double deck;
			DEPTH_type out;
			out.NumLay = 0;
			out.Dist.assign(NL, 0.);
			S_Depth = min(-0.01, S_Depth);
			deck = log(ExpTail) / S_Depth;
			double rupper = 1;
			double rlower;
			size_t icount = 0;
			rlower = max(ExpTail, exp(-deck*DepthLayers[0]) - 0.001*ExpTail);
			while (rlower>ExpTail&&icount<DepthLayers.size() - 1) {
				rlower = max(ExpTail, exp(-deck*DepthLayers[icount]) - 0.001*ExpTail);
				out.Dist[icount]=(rupper - rlower) / (1. - ExpTail)*totsum;
				rupper = rlower;
				icount++;
			}
			if (icount == 0) {
				out.Dist[0] = totsum;
				out.NumLay = 1;
			}
			else
				out.NumLay = icount;

            auto koll = sum(out.Dist)/totsum;
			if (koll < 0.999 || koll > 1.001) {
				for (size_t i = 0; i <= out.NumLay; i++)
					out.Dist[i] *= 1. / koll;
			}

			return out;
		}
private: vector<double> DepthLayers, ThicknessLayers;
		 double ExpTail;
		 size_t NL;
};
class FixedDepthDistribution {
public:
	FixedDepthDistribution(vector<double> Dist, vector<double> DepthLayers, vector<double> ThicknessLayers) : Dist(Dist), DepthLayers(DepthLayers), ThicknessLayers(ThicknessLayers) {}
	DEPTH_type operator() (double totsum, double S_Depth) {
		DEPTH_type out;
		out.Dist;
		out.NumLay = Dist.size();
		// Need Adjustment if accont should be made to 
		return out;
	}

private:
private: vector<double> DepthLayers, ThicknessLayers;
	vector<double> Dist;

};

