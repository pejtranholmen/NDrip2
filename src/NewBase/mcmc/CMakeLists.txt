add_library(mcmclib )

target_sources(mcmclib
  PRIVATE
        Density.cpp
        intervals.cpp
        lot.cpp
        ratio.cpp
        statistics.cpp
        util.cpp
  PUBLIC
         Density.h
         intervals.h
         lot.h
         ratio.h
         statistics.h
         util.h
  )

target_include_directories(mcmclib
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )
