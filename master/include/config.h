#ifndef CONFIG_H
#define CONFIG_H

//define, if you want to send the data as String over MQTT, else undef. The data will be sent binary
#undef MQTT_STRING
//used for debugging purposes, like drawing an image of the screen or average calculated colours
#undef DEBUG
//define for measurement of needed time of loop
#undef TIME

//name or ip of the broker. IP should be formated as: AAA.BBB.CCC.DDD
#define MQTT_BROKER "openhab.local"
//default port is 1883, but there is other for encrypted
#define MQTT_PORT 1883
//your base topic, there will be sub-themes for "color","NumLeds","NumColors"
#define MQTT_BASE_TOPIC "desk/backlight"
//the name which will appear for you in the mqtt-network
#define MQTT_NAME "desktop"

#endif // CONFIG_H
