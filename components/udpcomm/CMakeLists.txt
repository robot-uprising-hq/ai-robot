
set(COMPONENT_REQUIRES nanopb)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "udpserver.c proto/RobotSystemCommunication.pb.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
