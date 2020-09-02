ESP-IDF template app
====================

This is a template application to be used with [Espressif IoT Development Framework](https://github.com/espressif/esp-idf).

Please check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.

*Code in this repository is in the Public Domain (or CC0 licensed, at your option.)
Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.*

---

# Docker in development
## Docker compile
docker run --rm -v $PWD:/project -w /project espressif/idf:release-v4.2 idf.py build

## Docker flash
docker run --rm --privileged -v /dev:/dev -v $PWD:/project -w /project espressif/idf:release-v4.2 idf.py -p /dev/ttyUSB0 flash

---

# Serial communication with the ESP32
Setup serial communication to ESP32 with Putty.

<img src="docs/images/putty-config-serial.png" width="500">

Configure Putty to show input user types.

<img src="docs/images/putty-show-input.png" width="500">

### Available commands for AI Robot ESP32

|Command| Action  |
|---|---|
|  help | Show this help  |
|  scan | Do a quick Wifi AP scan. |
|  query |Show configuration. |
|  query all | Show all configuration, including passwords.  |
|  set ssid SSID | Set WiFi AP to use to SSID.  |
|  set passwd PASSWD | Set Wifi password to PASSWD. |
|  udpsrvstart | Start UDP server.  |
|  wifistart | (Attempt to) start WiFi.  |
|  wifistop | Disconnect from WiFi.  |
|  motortest | Run simple test of the motors.  |
|  quit | Exit command-loop (mainly for debugging).  |
