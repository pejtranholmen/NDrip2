#pragma once

#include "./P_ExternalInputs_Functions.h"
class P_ExternalInputs : public P_ExternalInputs_Functions
{
	public:
		P_ExternalInputs(void);
		~P_ExternalInputs(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();




};
