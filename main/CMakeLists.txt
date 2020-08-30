
set(COMPONENT_REQUIRES motor_control udpcomm wifi)
set(COMPONENT_PRIV_REQUIRES nvs_flash)

set(COMPONENT_SRCS "main.c readline.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
