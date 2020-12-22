void redisInterface_handle(void) {
    String redis_key;
    String cse7766_value;
    String redis_str_result;
    bool redis_bool_result;

    if (redisInterface_flag == true) {
        if (redisInterface_state == 0) {//connect Redis server
            if (!redisConn.connect(redis_server_addr.c_str(), redis_server_port))
            {
#if USE_TELNET
                debugE("Failed to connect to the Redis server!");
#endif
                redisInterface_state = 0;
                redisInterface_flag = false;
                redisPeriod = REDIS_PERIOD_FAIL;
                blueLed.setPatternSingle(error_pattern, 6);
                return;
            }
            redisPeriod = REDIS_PERIOD_NORM;
            redisInterface_state++;
        } else if (redisInterface_state == 1) {//authen Redis server & send data1
            Redis redis(redisConn);
            if (redis_server_pass != "") {
                auto connRet = redis.authenticate(redis_server_pass.c_str());

                if (connRet == RedisSuccess)
                {
#if USE_TELNET
                    debugD("Connected to the Redis server!");
#endif
                    blueLed.setPatternSingle(normal_pattern, 4);
                } else {
#if USE_TELNET
                    debugE("Failed to authenticate to the Redis server! Errno: %d\n", (int)connRet);
#endif
                    redisInterface_state = 0;
                    redisInterface_flag = false;
                    redisConn.stop();
                    blueLed.setPatternSingle(unAuthen_pattern, 6);
                    return;
                }
            } else {
                blueLed.setPatternSingle(noAuthen_pattern, 8);
            }

            // Voltage
            redis_key = redis_deviceKey + String(redis_voltage);
            cse7766_value = String(cse7766.getVoltage());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
                if (redis_server_pass == "") {//can connect but auth fail
                    blueLed.setPatternSingle(unAuthen_pattern, 6);
                }
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // Current
            redis_key = redis_deviceKey + String(redis_current);
            cse7766_value = String(cse7766.getCurrent());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // ActivePower
            redis_key = redis_deviceKey + String(redis_activepower);
            cse7766_value = String(cse7766.getActivePower());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // ApparentPower
            redis_key = redis_deviceKey + String(redis_apparentpower);
            cse7766_value = String(cse7766.getApparentPower());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            redisInterface_state++;
        } else if (redisInterface_state == 2) {//authen Redis server & send data2
            Redis redis(redisConn);
            if (redis_server_pass != "") {
                auto connRet = redis.authenticate(redis_server_pass.c_str());

                if (connRet == RedisSuccess)
                {
#if USE_TELNET
                    debugD("Connected to the Redis server!");
#endif
                } else {
#if USE_TELNET
                    debugE("Failed to authenticate to the Redis server! Errno: %d\n", (int)connRet);
#endif
                    redisInterface_state = 0;
                    redisInterface_flag = false;
                    redisConn.stop();
                    return;
                }
            }

            // ReactivePower
            redis_key = redis_deviceKey + String(redis_reactivepower);
            cse7766_value = String(cse7766.getReactivePower());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // PowerFactor
            redis_key = redis_deviceKey + String(redis_powerfactor);
            cse7766_value = String(cse7766.getPowerFactor());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // Energy
            redis_key = redis_deviceKey + String(redis_energy);
            cse7766_value = String(cse7766.getEnergy());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
#if REDIS_SET_TEST
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // TimeStamp
            redis_key = redis_deviceKey + String(redis_timestamp);
            //            String timeStamp = timeClient.getFormattedTime();
#if USE_TELNET
            //            debugD("SET %s %s: ", redis_key.c_str(), timeStamp.c_str());
#endif
#if REDIS_SET_TEST
            //            redis_bool_result = redis.set(redis_key.c_str(), timeStamp.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#endif
#endif
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
#if USE_TELNET
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            redisInterface_state++;
        } else if (redisInterface_state == 3) {//disconnect Redis server
            redisConn.stop();
#if USE_TELNET
            debugD("Connection closed!");
#endif

            redisInterface_state = 0;
            redisInterface_flag = false;
        } else {
            redisInterface_state = 0;
            redisInterface_flag = false;
        }
    }
}
