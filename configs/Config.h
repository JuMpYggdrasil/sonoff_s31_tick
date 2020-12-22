#ifndef _CONFIG_H_
#define _CONFIG_H_

#if defined ESP8266

#include "CommonConfig.h"
#include "GpioConfig.h"
#include "WifiConfig.h"
#include "RedisConfig.h"
#include "FtpConfig.h"
#include "OtaConfig.h"
#include "TelnetConfig.h"

// Includes of ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>//for led and push button status,(build in lib)

#if USE_NTP
#include <NTPClient.h>//Fabrice Weinberg v3.2.0
#include <WiFiUdp.h>
#else
#include <time.h>
#endif


#include "CSE7766.h"//ingeniuske custom-modified
#include <PinButton.h>//Martin Poelstra v1.0.0
#include <EEPROM.h>
#include <singleLEDLibrary.h>//SethSenpai v1.0.0
#include <FS.h>

#include "../userInterfacesVar.h"//local custom file


#else
#error "The board must be ESP8266"
#endif // ESP8266

#endif
