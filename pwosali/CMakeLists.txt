PROJECT(pwosali)

ADD_EXECUTABLE(pwosali main.cpp)
target_link_libraries( pwosali ivymike )
#SET(CMAKE_CXX_FLAGS "--std=c++0x -Wall")
#get_target_property( ivy_dir ivymike LOCATION )
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++0x -I${ivy_dir}")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++0x")