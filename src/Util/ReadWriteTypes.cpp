#include "../std.h"
#include "../Util/ReadWriteTypes.h"
#include "FUtil.hpp"


ReadWriteTypes::ReadWriteTypes(void)
{
	m_FileVersionNumberRead=51;
	m_ShortTimeRead=true;

}


ReadWriteTypes::~ReadWriteTypes(void)
{
}
bool ReadWriteTypes::OpenDocFileIn(string filename) {

	m_DocFileIn.open(filename,ios::binary|ios::in|ios::out);
	if (!m_DocFileIn.good()) {
		m_DocFileIn.clear();
		m_DocFileIn.open(filename, ios::binary | ios::in | ios::out);
	}
	m_FileName = filename;
	return m_DocFileIn.good();

}
bool ReadWriteTypes::OpenDocFileout(string filename) {
	if(FUtil::IsFileExisting(filename))
		m_DocFileOut.open(filename,ios::binary|ios::out|ios::in);
	else
		m_DocFileOut.open(filename, ios::binary | ios::out );
	size_t n = 0;
	SetPutPosition(&n);
	m_FileName = filename;
	return m_DocFileOut.good();

}


void ReadWriteTypes::CloseFileIn() {
	m_DocFileIn.close();
}
void ReadWriteTypes::CloseFileOut() {
	m_DocFileOut.close();
}
bool ReadWriteTypes::wString(string *str) 
{
	//char il;
	long long a=m_DocFileOut.tellp();
	auto il=str->size();
	if(il>=0&&il<255)
		m_DocFileOut.write((char*)(&il),1);
	else {
		long long s=m_DocFileOut.tellp();
		//m_DocFileOut.seekp(0,ios::beg);
		unsigned char ik=255;
		m_DocFileOut.write((char*)(&ik),1);
		m_DocFileOut.write((char*)(&il),2);
		s=m_DocFileOut.tellp();
	}
	if(il>0)
		m_DocFileOut.write((char*)str->data(), il);
	a=m_DocFileOut.tellp();

	return true;

}
bool ReadWriteTypes::rString(string *str)
{
	unsigned char il;
	size_t ic;
	char cc;
		auto s=m_DocFileIn.tellg();
		if(s<0) return false;
		m_DocFileIn.read((char*)(&il),1);
		cc=il;
		if(il==255) {
			char s1,s2;
			m_DocFileIn.read((char*)(&s1), 1);
			m_DocFileIn.read((char*)(&s2), 1);
			ic=255+s2;
			m_DocFileIn.seekg(s,ios::beg);
			s=m_DocFileIn.tellg();
			m_DocFileIn.read((char*)(&il),1);
			m_DocFileIn.read((char*)&ic, 2);


		}
		else if(cc<0xFF) {
			ic=il;

		}
		else if(cc==0xFF) {
			char s1,s2;
			s=m_DocFileIn.tellg();
			m_DocFileIn.read((char*)(&s1),1);
			m_DocFileIn.read((char*)(&s2), 1);
			ic=255+s2;
		//	m_DocFileIn.read((char*)&ic, 2);

		}
		else {
			m_DocFileIn.read((char*)&ic, 2);


		}
		
		//m_DocFileIn.read((char*)(&ic),sizeof(ic));
		if(il== 0xfffe) {
			int ivv;
			m_DocFileIn.read((char*)&ivv, 2);

			str->clear();
			long long s=m_DocFileIn.tellg();
			if(s<0) {
				return false;

			}
		}
		else if(il== 0xffff) {
			int ivv;
			m_DocFileIn.read((char*)&ivv, 4);


		}
		else {
			m_DocFileIn.read(m_cbuf, ic);
			str->clear();
			if(ic>0)
				str->assign(&m_cbuf[0],ic);
		}
		return true;

}
bool ReadWriteTypes::rInt(int *iv)
{   int i;
	m_DocFileIn.read((char*)(&i) ,4);
	*iv=i;
	return true;
}
bool ReadWriteTypes::rsize_t(size_t *iv)
{   unsigned int i;
	m_DocFileIn.read((char*)(&i) ,4);
	*iv=i;
	return m_DocFileIn.good();
}
bool ReadWriteTypes::SetGetPosition(size_t *iv)
{  
	m_DocFileIn.seekg(*iv ,ios::beg);
	auto test=m_DocFileIn.tellg();
	return true;
}
bool ReadWriteTypes::SetPutPosition(size_t *iv)
{
	m_DocFileOut.seekp(*iv, ios::beg);
	if (size_t(m_DocFileOut.tellg()) == *iv) return true;
	return false;
}

bool ReadWriteTypes::rBOOL(bool *iv)
{
	int iBOOL;
	m_DocFileIn.read((char*)(&iBOOL) ,4);
	if(iBOOL!=0)
		*iv=true;
	else
		*iv=false;
	return true;
}
bool ReadWriteTypes::rbool(bool *iv)

{ 
	m_DocFileIn.read((char*)(&*iv) ,1);
	return true;
}
bool ReadWriteTypes::rLong(long *iv)
{
	long i;
	m_DocFileIn.read((char*)(&i) ,4);
	*iv=i;
	return true;
}
bool ReadWriteTypes::ruLong(unsigned long *iv)
{
	unsigned long i;
	m_DocFileIn.read((char*)(&i), 4);
	*iv = i;
	return true;
}
bool ReadWriteTypes::rFloat(float *f)
{   float v;
	m_DocFileIn.read((char*)(&v) ,4);
	*f=v;
	return true;
}
bool ReadWriteTypes::rFloatArray(float *f, size_t n)
{
	vector<float> v;
	v.resize(n);

	m_DocFileIn.read((char*)&v[0], 4*n);
	*f = v[0];
	return m_DocFileIn.good();
}
bool ReadWriteTypes::ruLongArray(unsigned long *v, unsigned long n)
{

	m_DocFileIn.read((char*)&*v, 4 * n);
	return m_DocFileIn.good();
}
vector<size_t> ReadWriteTypes::rsize_tArray(size_t n)
{
	vector<size_t> v;
	v.resize(n);
	vector<unsigned int> v1;
	v1.resize(n);
	m_DocFileIn.read((char*)&v1[0], 4 * n);
	for (size_t i = 0; i < n; i++)
		v[i] = v1[i];
	return v;
}
bool ReadWriteTypes::wFloatArray(float *f, size_t n)
{
	vector<float> v;
	v.resize(n);
	for (size_t i = 0; i < n; i++)
		v[i] = *(f+i);
	if(n>0)	m_DocFileOut.write((char*)&v[0], 4 * n);

	return m_DocFileOut.good();
}
bool ReadWriteTypes::wFloatArray2(vector<float> v, size_t n)
{
	if(n>0) m_DocFileOut.write((char*)&v[0], 4 * n);
	return m_DocFileOut.good();
}
vector<float> ReadWriteTypes::rFloatArray2(size_t n)
{
	vector<float> v; v.resize(n);
	if(n>0) m_DocFileIn.read((char*)&v[0], 4 * n);
	return v;
}
bool ReadWriteTypes::wuLongArray(unsigned long *v, size_t n)
{
	if(n>0) m_DocFileOut.write((char*)&*v, 4 * n);
	return m_DocFileOut.good();
}
bool ReadWriteTypes::wsize_tArray(vector<size_t> *v, size_t n)
{
	if (n > 0) {
		auto k=sizeof(v[0]);
		vector<unsigned int> v1;
		vector<unsigned long> v2;
		v1.resize(n); v2.resize(n);
		for (size_t i = 0; i < n; i++) {
			v1[i] = v->at(i);
			v2[i] = v->at(i);

		}
		 k = sizeof(v1[0]);
		 k = sizeof(v2[0]);
		m_DocFileOut.write((char*)&v1[0], n * 4);
	}
	return m_DocFileOut.good();
}
bool ReadWriteTypes::rDouble(double *f)
{   double v;
	m_DocFileIn.read((char*)(&v) ,8);
	*f=v;
	return true;
}
bool ReadWriteTypes::wInt(int *iv)
{   
	m_DocFileOut.write((char*)(&*iv) ,4);
	
	return true;
}
bool ReadWriteTypes::wInt(size_t *iv)
{   

	m_DocFileOut.write((char*)(&*iv) ,4);
	if(m_DocFileOut.good())
		return true;
	else
		return false;
}
//bool ReadWriteTypes::wInt(unsigned long *iv)
//{
//
//    m_DocFileOut.write((char*)(&*iv), 4);
//    if (m_DocFileOut.good())
//        return true;
//    else
//        return false;
//}
bool ReadWriteTypes::wPosition()
{   
	auto pos=m_DocFileOut.tellp();
	size_t ipos=size_t(pos);
	m_DocFileOut.write((char*)&ipos,4);
	if(m_DocFileOut.good())
		return true;
	else
		return false;
}
bool ReadWriteTypes::wbool(bool *iv)
{ 
	m_DocFileOut.write((char*)(&*iv) ,1);
	//*iv=i;
	return true;
}
bool ReadWriteTypes::wLong(long *iv)
{
	m_DocFileOut.write((char*)(&*iv) ,4);
	
	return true;
}
bool ReadWriteTypes::wFloat(float *f)
{   
	m_DocFileOut.write((char*)(&*f) ,4);
	return true;
}
bool ReadWriteTypes::wDouble(double *f)
{   
	m_DocFileOut.write((char*)(&*f) ,8);
	return true;
}
bool ReadWriteTypes::wTime(time_t *time)
{

	m_DocFileOut.write((char*)&*time,8);
	return true;
}
bool ReadWriteTypes::rTime(time_t *time)
{
	char buffer[8];
	char* buf;
	buf=&buffer[0];
	if(m_FileVersionNumberRead>=50) 
		m_DocFileIn.read((char*)&*time,8);
	else if(m_FileVersionNumberRead<48||m_ShortTimeRead) {
		//m_DocFileIn.read((char*)&*time,8);
		m_DocFileIn.read(m_cbuf,4);
		for(int i=4;i<8;i++) 
			*(buf+i)=0;
		for(int i=0;i<4;i++) 
			*(buf+i)=m_cbuf[i];
		*time=*(time_t*)buf;
	}
	else  {
		m_DocFileIn.read(m_cbuf,12);
		for(int i=4;i<8;i++) 
			*(buf+i)=0;
		for(int i=0;i<4;i++) 
			*(buf+i)=m_cbuf[4+i];
		*time=*(time_t*)buf;
	}

		
	//if(m_FileVersionNumberRead<50)
	//	*time=0;

	return true;
}
size_t ReadWriteTypes::GetCurrentInPosition() {
	auto actpos = m_DocFileIn.tellg();
	return size_t(actpos);
}
size_t ReadWriteTypes::GetInputFileSize() {
	m_DocFileIn.seekg(0, ios::end);
	auto actpos = m_DocFileIn.tellg();
	m_DocFileIn.seekg(0, ios::beg);
	return size_t(actpos);
}
std::streamsize ReadWriteTypes::GetStdInputFileSize() {
	m_DocFileIn.seekg(0, ios::end);
	std::streamsize actpos = m_DocFileIn.tellg();
	m_DocFileIn.seekg(0, ios::beg);
	return actpos;
}
