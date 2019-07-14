
# MPU9250 on ESP32

Measure the orientation of a MPU9250 IMU and display the result on a web page served by an ESP32.

The project must be compiled with *PlarformIO Core 4.0*.

The web files must be uploaded on the ESP32 with the following command:

    platformio run --target uploadfs


<p align="center">
<img height=400px alt="MPU9250 on ESP32" src="doc/mpu9250-esp32.jpg" />
</p>



# Credits

The MPU0250 driver comes from the M5Stack project:

<https://github.com/m5stack/M5Stack/blob/master/src/utility/MPU9250.cpp>


The *threejs* stuff comes form this project:

<https://github.com/DominikN/ESP32-MPU9250-web-view>

