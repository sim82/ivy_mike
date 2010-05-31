PROJECT(ivymike)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

ADD_LIBRARY(ivymike main.cpp src/LargePhylip.cpp ) 
#src/TreeParser.cpp )

INSTALL(TARGETS ivymike
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)



INSTALL(FILES src/LargePhylip.h
  DESTINATION include)
