#ifndef _FTP_CONFIG_H_
#define _FTP_CONFIG_H_

#include "CommonConfig.h"

#if USE_FTP
#include <ESP8266FtpServer.h>//nailbuster

#define DEFAULT_FTP_USER DEFAULT_USER_PHRASE
#define DEFAULT_FTP_PASS DEFAULT_PASS_PHRASE

#endif

#endif