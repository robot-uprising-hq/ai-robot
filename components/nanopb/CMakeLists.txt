# Edit following two lines to set component requirements (see docs)
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../nanopb/extra)
#find_package(Nanopb REQUIRED)
#include_directories(./nanopb)

set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "pb_encode.c pb_decode.c pb_common.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
#idf_component_register(SRCS "car.c"
#        INCLUDE_DIRS "."
#        REQUIRES engine)