cmake_minimum_required( VERSION 2.8 )

IF(WIN32)
include_directories( C:\\src\\boost_1_46_1)
ENDIF()


PROJECT(ivymike)


#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

#ADD_LIBRARY(ivymike src/main.cpp src/LargePhylip.cpp src/time.cpp )
ADD_LIBRARY(ivymike src/time.cpp src/tree_parser.cpp src/multiple_alignment.cpp src/getopt.cpp src/demangle.cpp src/sdf.cpp )


INSTALL(TARGETS ivymike
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

ADD_EXECUTABLE( treeparser_test src/tree_parser.cpp src/tree_parser_test.cpp )
# ADD_EXECUTABLE( sdf_test src/sdf.cpp )


INSTALL(FILES ivymike/LargePhylip.h
  DESTINATION include)

