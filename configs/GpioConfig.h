#ifndef _GPIO_CONFIG_H_
#define _GPIO_CONFIG_H_

#include "CommonConfig.h"

//// Hardware config
/// GPIOs
#define PUSHBUTTON_PIN   0
#define RELAY_PIN       12//relay(active high) include red led(active high)
#define LED_PIN         13//blue led(active high/->>low???)
/// UART0
//#define esp8266_TX_PIN 1//(S31)connect GPIO_CSE7766_RX PIN8(RI)
//#define esp8266_RX_PIN 3//(S31)connect GPIO_CSE7766_TX PIN6(TI)
/// i2c
//#define SDA 4//GPIO4 as D RX
//#define SCL 5//GPIO5 as D TX


#endif
