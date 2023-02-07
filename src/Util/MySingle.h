//
// Created by pej on 2020-01-25.
//
class MyCug;
class FunctionProperties;
class ChartCug;
class MyPGCug;
class PGFileArchive;
class SimArchive;
class CCoupModelDoc;
class CClassView;
class CFileView;
class CMainFrame;


class ChartProp;
class ChartUtil;
class CChartWnd;
class ZoomTimeSerie;
class PlotInfo;
class PFCug;
class SoilData;
class DB_Header;
class DBDataCug;
class DB_BASCug;
class PFCug;
#ifndef DEV_MYSINGLE_H
#define DEV_MYSINGLE_H


class MySingle {

    private:
        MySingle(void) {};
        MyCug* m_pCug;
        MyPGCug* m_pPGCug;
        ChartCug* m_pChartCug;
        FunctionProperties* m_pFunc;
        PGFileArchive* m_pPGArchive;
        SimArchive* m_pSimArchive;
        CCoupModelDoc* m_pPrevDoc;
        CClassView* m_pClassView;
        CFileView* m_pFileView;
        CMainFrame* m_pFrame;

        ChartProp* m_pChartProp;
        ChartUtil* m_pChartUtil;
        CChartWnd* m_pExtraChartWnd;
        ZoomTimeSerie* m_pZoomTimeSerie;
        PlotInfo* m_pPlotInfo;
        PFCug* m_PFCug;
        SoilData* m_SoilData;
        DB_Header* m_pDB;
        DB_BASCug* m_pDBBasCug;
        DBDataCug* m_pDBDataCug;

    public:

        // Not thread safe!!!!!!!!
        // Return by reference
        static MySingle& instance() {
            static MySingle mySingle;
            return mySingle;
        }



        void SetDBDataPointer(DBDataCug* pDBData) { m_pDBDataCug = pDBData; };
        DBDataCug* GetDBDataPointer() { return m_pDBDataCug; };

        void SetDBBasPointer(DB_BASCug* pDBBas) { m_pDBBasCug = pDBBas; };
        DB_BASCug* GetDBBasPointer() { return m_pDBBasCug; };


        void SetSoilDataPointer(SoilData* pSoilData) { m_SoilData = pSoilData; };
        SoilData* GetSoilDataPointer() { return m_SoilData; };

        void SetDB_HeaderPointer(DB_Header* pDB) { m_pDB = pDB; };
        DB_Header* GetDB_HeaderPointer() { return m_pDB; };


        void SetPFCugPointer(PFCug* pPFCug) { m_PFCug = pPFCug; };
        PFCug* GetPFCugPointer() { return m_PFCug; };



        void SetFramePointer(CMainFrame* pFrame) { m_pFrame = pFrame; };
        CMainFrame* GetFramePointer() { return m_pFrame; };

        void SetModelPointer(CClassView* pClass) { m_pClassView = pClass; };
        CClassView* GetClassViewPointer() { return m_pClassView; };

        void SetFilePointer(CFileView* pFile) { m_pFileView = pFile; };
        CFileView* GetFileViewPointer() { return m_pFileView; };

        void SetCugPointer(MyCug* pCug) { m_pCug = pCug; };
        MyCug* GetCugPointer() { return m_pCug; };

        void SetPGCugPointer(MyPGCug* pCug) { m_pPGCug = pCug; };
        MyPGCug* GetPGCugPointer() { return m_pPGCug; };

        void SetPGFileArchivePointer(PGFileArchive* pArchive) { m_pPGArchive = pArchive; };
        PGFileArchive* GetPGFileArchivePointer() { return m_pPGArchive; };

        void SetSimArchivePointer(SimArchive* pArchive) { m_pSimArchive = pArchive; };
        SimArchive* GetSimArchivePointer() { return m_pSimArchive; };

        void SetChartCugPointer(ChartCug* pCug) { m_pChartCug = pCug; };
        ChartCug* GetChartCugPointer() { return m_pChartCug; };

        void SetFunctionPointer(FunctionProperties* pFunc) { m_pFunc = pFunc; };
        FunctionProperties* GetFunctionPointer() { return m_pFunc; };

        void SetChartPropPointer(ChartProp* pProp) { m_pChartProp = pProp; };
        ChartProp* GetChartPropPointer() { return m_pChartProp; };

        void SetPlotInfoPointer(PlotInfo* pProp) { m_pPlotInfo = pProp; };
        PlotInfo* GetPlotInfoPointer() { return m_pPlotInfo; };

        void SetChartUtilPointer(ChartUtil* pUtil) { m_pChartUtil = pUtil; };
        ChartUtil* GetChartUtilPointer() { return m_pChartUtil; };

        void SetChartZoomPointer(ZoomTimeSerie* pZoom) { m_pZoomTimeSerie = pZoom; };
        ZoomTimeSerie* GetChartZoomPointer() { return m_pZoomTimeSerie; };

        void SetExtraChartWndPointer(CChartWnd* pExtraChart) { m_pExtraChartWnd = pExtraChart; };
        CChartWnd* GetExtraChartWndPointer() { return m_pExtraChartWnd; };

        void SetActDoc(CCoupModelDoc* pDoc) { m_pPrevDoc = pDoc; };
        bool IsActDoc(CCoupModelDoc* pDoc) { if (pDoc == m_pPrevDoc) return true; else return false; };





};


#endif //DEV_MYSINGLE_H
