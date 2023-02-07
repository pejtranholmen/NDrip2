set(LIBSOILPF_NAME libsoilpf)

set(LIBSOILPF_SOURCES
        PlotPF/GausKreuger.cpp
        PlotPF/GausKreuger.h
        PlotPF/Geodesi.cpp
        PlotPF/GetPFCurve.cpp
        PlotPF/GetPFCurve.h
        PlotPF/PFCurve.cpp
        PlotPF/PFCurve.h
        PlotPF/PFStorage.cpp
        PlotPF/PFStorage.h
        GetSoilStorage.cpp
        GetSoilStorage.h
        SoilStorage.cpp
        SoilStorage.h
)
add_library(${LIBSOILPF_NAME} STATIC ${LIBSOILPF_SOURCES})
target_link_libraries(${LIBSOILPF_NAME} INTERFACE ${LIBUTIL_NAME})
