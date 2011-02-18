PROJECT(ivymike)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

ADD_LIBRARY(ivymike src/main.cpp src/LargePhylip.cpp src/time.cpp )

INSTALL(TARGETS ivymike
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

ADD_EXECUTABLE( treeparser_test src/TreeParser.cpp )

INSTALL(FILES ivymike/LargePhylip.h
  DESTINATION include)

