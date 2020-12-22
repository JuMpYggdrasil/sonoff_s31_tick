#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_

#include "CommonConfig.h"

#if USE_WiFiManager

#include <WiFiManager.h>//tzapu v2.0.3-alpha

#else

#define STASSID "JUMP"//DEFAULT_USER_PHRASE
#define STAPSK "025260652"//DEFAULT_PASS_PHRASE



#endif

#if USE_MDNS
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#endif

// webpage server authen
#define DEFAULT_WWW_USER DEFAULT_USER_PHRASE
#define DEFAULT_WWW_PASS DEFAULT_PASS_PHRASE

#endif
