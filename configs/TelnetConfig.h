#ifndef _TELNET_CONFIG_H_
#define _TELNET_CONFIG_H_

#include "CommonConfig.h"

#if USE_TELNET


// Remote debug over WiFi - not recommended for production, only for development
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug

// Error: Critical errors
// Always: Important messages
// Another levels (showed if level is equal or higher that actual one):
// Warning: Error conditions but not critical
// Info: Information messages
// Debug: Extra information
// Verbose: More information than the usual

/*
   (V p:3065 loop C1) * Run time: 00:41:23 (VERBOSE)

    Where:  V: is the level
            p: is a profiler time, elased, between this and previous debug
            loop: is a function name, that executed this debug
            C1: is a core that executed this debug (and a function of this) (only for ESP32)
            The remai ning is the message formatted (printf)
*/

#endif

#endif
