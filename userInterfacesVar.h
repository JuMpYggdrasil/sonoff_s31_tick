#ifndef _USER_INTERFACES_VARIABLE_H_
#define _USER_INTERFACES_VARIABLE_H_

// #  Led patterns
// ## operating mode
int initPass_pattern[] = {1900, 100};//one short blink
int normal_pattern[] = {1500, 100, 300, 100};//twice short blink
int error_pattern[] = {1100, 100, 300, 100, 300, 100};//three times short blink
int noAuthen_pattern[] = {700, 100, 300, 100, 300, 100, 300, 100};//four times short blink
int unAuthen_pattern[] = {300, 100, 300, 100, 300, 900};//one long & two short blink
// ##  setting mode
int waitClick_pattern[] = {100, 200, 100, 800};//one short & one long blink
int wifiManager_pattern[] = {100, 100};//fast constant continue blink
int config_pattern[] = {50, 50};//fast constant continue blink

// # button click state
boolean singleClick_flag = false;
boolean doubleClick_flag = false;
boolean longClick_flag = false;

#endif
