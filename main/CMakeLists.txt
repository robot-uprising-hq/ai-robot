# Edit following two lines to set component requirements (see docs)
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../nanopb/extra)
#find_package(Nanopb REQUIRED)
#include_directories(./nanopb)

set(COMPONENT_REQUIRES nanopb)
set(COMPONENT_PRIV_REQUIRES nvs_flash)

set(COMPONENT_SRCS "main.c console.c wifiscan.c wificonnect.c udpserver.c RobotSystemCommunication.pb.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
