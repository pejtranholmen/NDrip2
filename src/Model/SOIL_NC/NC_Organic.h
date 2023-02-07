#pragma once

#include "./NC_Organic_Functions.h"
class NC_Organic : public NC_Organic_Functions
{
	public:
		NC_Organic(void);
		~NC_Organic(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();


		void UptakeOrgReset();
	private:
		bool m_DisplacementDone;

};
