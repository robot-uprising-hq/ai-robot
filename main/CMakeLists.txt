
set(COMPONENT_REQUIRES motor_control udpcomm wifi readline led_control battery_monitor)
set(COMPONENT_PRIV_REQUIRES nvs_flash)

set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
