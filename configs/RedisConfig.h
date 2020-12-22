#ifndef _REDIS_CONFIG_H_
#define _REDIS_CONFIG_H_

#include "CommonConfig.h"

#if USE_REDIS

#include <Redis.h>//Ryan Joseph v2.1.3

#define REDIS_ADDR "siriprapawat.trueddns.com"//"192.168.1.22"
#define REDIS_PORT 14285//6379
#define REDIS_PASS "61850"
#define REDIS_PERIOD_NORM 1//send every sec if connected
#define REDIS_PERIOD_FAIL 30//try every 30 sec if fail

#define REDIS_EEPROM_ADDR_BEGIN 0//address of REDIS_DEVKEY
#define REDIS_EEPROM_SERVER_ADDR 100
#define REDIS_EEPROM_SERVER_PORT 130
#define REDIS_EEPROM_SERVER_PASS 132
//#define REDIS_EEPROM_SERVER_xx 150
#define EEPROM_INIT 501

#define REDIS_DEVKEY "ACBUSx220xengMMTR1/MMXU1$MX$"
#define REDIS_VOLTAGE "PhV$mag$f"
#define REDIS_CURRENT "A$mag$f"
#define REDIS_ACTIVEPOWER "TotW$mag$f"//P
#define REDIS_APPARENTPOWER "TotVA$mag$f"//S
#define REDIS_REACTIVEPOWER "TotVAr$mag$f"//Q
#define REDIS_POWERFACTOR "TotPF$mag$f"
#define REDIS_ENERGY "TotWh$mag$f"
//#define REDIS_FREQUENCY "Hz$mag$f"
#define REDIS_TIMESTAMP "Time$mag$f"

////Redis
bool redisInterface_flag = false;
int redisInterface_state = 0;
int redisPeriod = REDIS_PERIOD_NORM;

String redis_deviceKey = REDIS_DEVKEY;
String redis_server_addr = REDIS_ADDR;
uint16_t redis_server_port = REDIS_PORT;
String redis_server_pass = REDIS_PASS;

// ===  redis
const char* redis_voltage PROGMEM = REDIS_VOLTAGE;
const char* redis_current PROGMEM = REDIS_CURRENT;
const char* redis_activepower PROGMEM = REDIS_ACTIVEPOWER;
const char* redis_apparentpower PROGMEM = REDIS_APPARENTPOWER;
const char* redis_reactivepower PROGMEM = REDIS_REACTIVEPOWER;
const char* redis_powerfactor PROGMEM = REDIS_POWERFACTOR;
const char* redis_energy PROGMEM = REDIS_ENERGY;
const char* redis_timestamp PROGMEM = REDIS_TIMESTAMP;

#else
	
	#define EEPROM_INIT 501

#endif

#endif
