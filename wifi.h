#ifdef __cplusplus
extern "C" {
#endif

#ifndef _HTTP_H_
#define _HTTP_H_

#include "main.h"

///////////
// Wi-Fi //
///////////
#define WIFI_BLINK_ACCESSPOINT    1000
#define WIFI_BLINK_STATION        500

#define WIFI_ACCESSPOINT_TIMEOUT  30000 // 30 sec

#define SSIDST   "MER1976"
#define PSKST    "M@rioElvir@Ruiz1976"

#define SSIDAP    "EFIS8266"
#define PSKAP     "topsecret"

#define WIFI_START_ACCESSPOINT     0
#define WIFI_ON_ACCESSPOINT        1
#define WIFI_START_STATION         2
#define WIFI_ON_STATION            3
#define WIFI_STATION_CONNECTING    4
#define WIFI_STATION_CONNECTED     5

#endif // _HTTP_H_

#ifdef __cplusplus
} // extern "C"
#endif
