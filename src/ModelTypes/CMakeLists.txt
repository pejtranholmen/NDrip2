add_library(modeltypeslib)
target_sources(
	modeltypeslib
	PRIVATE
	CDB.cpp CDB.h
	D.cpp D.h
    Ds.cpp Ds.h
	F.cpp F.h
	FCSV.cpp FCSV.h
	FixedInteger_PlantComp.h
	Func.cpp Func.h
	FunctFactory.cpp FunctFactory.h
	G.cpp G.h
	Gs.cpp Gs.h
	NE.cpp NE.h
	NewModelType.h
	OutBase.cpp OutBase.h
	OutSingle.cpp OutSingle.h
	OutVector.cpp OutVector.h
	P.cpp P.h
	Par.cpp Par.h
	Ps.cpp Ps.h
	CRunInfo.cpp RunInfo.h
	Sw.cpp Sw.h
	T.cpp T.h
	Tab.cpp Tab.h
	Ts.cpp Ts.h
	X.cpp X.h
	Xs.cpp Xs.h
	SimB.cpp SimB.h
	../std.h
	../NewBase/CommonModelInfo.cpp
	../NewBase/CommonModelInfo.h
)
#target_include_directories(
#	modeltypeslib
#	PRIVATE src/modeltypes)
#target_link_directories(modeltypeslib PRIVATE ${CMAKE_CURRENT_LIST_DIR})