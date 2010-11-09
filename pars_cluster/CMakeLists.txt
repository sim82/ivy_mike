project(pars_cluster)

add_executable( pars_cluster main.cpp )

target_link_libraries( pars_cluster boost_serialization jpeg png )

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
