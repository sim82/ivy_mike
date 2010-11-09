PROJECT(barrier)

ADD_EXECUTABLE(barrier barrier.cpp)
#target_link_libraries( pwosali ivymike )
#SET(CMAKE_CXX_FLAGS "--std=c++0x -Wall")
#get_target_property( ivy_dir ivymike LOCATION )
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++0x -I${ivy_dir}")
target_link_libraries( barrier boost_thread-mt pthread ivymike )
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/space2/c_tools/barrier -lstupid" )