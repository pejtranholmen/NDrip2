#include "RunInfo.h"
CRunInfo::CRunInfo() {


}
CRunInfo::~CRunInfo()
{

}
CRunInfo::CRunInfo(int index)
{
	switch(index) {
		case 0:
			m_Group="General";
			m_Name="Run Number";
			m_Format=0;
			break;
		case 1:
			m_Group="General";
			m_Name="No of MultiRun";
			m_Format=0;
			break;
		case 2:
			m_Group="Simulation Period";
			m_Name="Start Date";
			m_Format=1;
			break;
		case 3:
			m_Group="Simulation Period";
			m_Name="End Date";
			m_Format=1;
			break;
		case 4:
			m_Group="Simulation Period";
			m_Name="Pre Period Date";
			m_Format=1;
			break;
		case 5:
			m_Group="Simulation Period";
			m_Name="Scaling of Period";
			m_Format=0;
			break;
		case 6:
			m_Group="Simulation Time";
			m_Name="Input Time Resolution";
			m_Format=0;
			break;
		case 7:
			m_Group="Simulation Time";
			m_Name="Output Time interval";
			m_Format=0;
			break;
		case 8:
			m_Group="Simulation Time";
			m_Name="Number of Iterations";
			m_Format=0;
			break;
		case 9:
			m_Group="Additional Info";
			m_Name="Short Id";
			m_Format=2;
			break;
		case 10:
			m_Group="Additional Info";
			m_Name="Comment";
			m_Format=2;
			break;
		case 11:
			m_Group="Executions";
			m_Name="Started";
			m_Format=2;
			break;
		case 12:
			m_Group="Executions";
			m_Name="Finnished";
			m_Format=2;
			break;
		case 13:
			m_Group="File Info Created";
			m_Name="NewRunNo";
			m_Format=2;
			break;
	}
}
string CRunInfo::GetName()
{
	return m_Name;
}
string CRunInfo::GetGroup()
{
	return m_Group;
}