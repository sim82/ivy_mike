PROJECT(ivymike)

ADD_LIBRARY(ivymike main.cpp src/LargePhylip.cpp )

INSTALL(TARGETS ivymike
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)



INSTALL(FILES src/LargePhylip.h
  DESTINATION include)
