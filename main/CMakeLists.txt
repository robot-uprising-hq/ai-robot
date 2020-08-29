
set(COMPONENT_REQUIRES motor_control udpcomm)
set(COMPONENT_PRIV_REQUIRES nvs_flash)

set(COMPONENT_SRCS "main.c console.c wifiscan.c wificonnect.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
