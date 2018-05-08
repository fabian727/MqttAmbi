#ifndef CONFIG_H
#define CONFIG_H

//define, if you want to send the data as String over MQTT, else undef. The data will be sent binary
#undef MQTT_STRING
//used for debugging purposes, like drawing an image of the screen or average calculated colours
#undef DEBUG
//define for measurement of needed time of loop
#undef TIME

//name or ip of the broker. IP should be formated as: AAA.BBB.CCC.DDD
#define MQTT_BROKER "surfer.local"
//default port is 1883, but there is other for encrypted
#define MQTT_PORT 1883
//your base topic, there will be sub-themes for "color","numleds","numcolors"
#define MQTT_BASE_TOPIC "desk/backlight"
//the name which will appear for you in the mqtt-network
#define MQTT_NAME "desktop"
//the default number of LEDs of each stripe
#define MQTT_LEDS 42
//number of colors per LED
#define MQTT_COLORS 3
//number of default stripes, which are spoken to
#define MQTT_STRIPES 1
#endif // CONFIG_H
