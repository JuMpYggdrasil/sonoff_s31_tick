// # Documentation Section
// =================================
// =====       ESP8266      ===== //
// ===== arduino ide 1.8.13 ===== //
// Generic esp8266
// CPU 80 MHz
// XTAL 26 MHZ
// flash size 4MB(FS:2MB OTA:~1019KB)
// flash mode DOUT
// =================================

// # Link Section
// Include Libraries
#include "configs/Config.h"

// # Using Directive/Defining Section
// # Class declaration/definition Section
// ## global variable declaration
// ### Timing
uint32_t mLastTime = 0;
uint32_t mTimeSeconds = 0;
// ### www
String hostNameWifi;

// ## object declaration
Ticker btnTicker;
Ticker ledTicker;

#if USE_WiFiManager
WiFiManager wm;
bool wm_reset_flag = false;
#endif

#if !USE_MDNS
IPAddress local_IP(192, 168, 1, 17);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 1);
#endif

#if USE_FTP
FtpServer ftpSrv;
#endif

#if USE_TELNET
RemoteDebug Debug;
#endif

CSE7766 cse7766;
PinButton S31_Button(PUSHBUTTON_PIN);
sllib blueLed(LED_PIN);
ESP8266WebServer server(80);
WiFiClient redisConn;

#if USE_NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.navy.mi.th", 25200);//GMT+7 =3600*7 =25200
#else
int timezone = 7 * 3600;
int dst = 0;
#endif

// ## constant variable inmemory declare
// ###  webpage structure (to access use FPSTR())
const char WEB_HEAD[] PROGMEM = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
const char WEB_LOAD_SCRIPT[] PROGMEM = "<script src=\"highcharts.js\"></script>";
const char WEB_STYLE[] PROGMEM = "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">";
const char WEB_BODY_START[] PROGMEM = "</head><body>";
const char WEB_SIDENAV[] PROGMEM = "<div class=\"sidenav\"><a href=\"/\">Home</a><a href=\"/info\">Info</a><a href=\"/graph\">Graph</a><a href=\"/update\">OTA</a></div>";
const char WEB_CONTENT_START[] PROGMEM = "<div class=\"content\"><h2><span style=\"color: maroon\">I</span>ED</h2>";
const char WEB_BODY_HTML_END[] PROGMEM = "</div></body></html>";//with end content
const char WEB_SCRIPT_START[] PROGMEM = "</div></body><script>";//with end content
const char WEB_SCRIPT_HTML_END[] PROGMEM = "</script></html>";

// ###  SSID authen
#if !USE_WiFiManager
// SSID and password
const char* ssid PROGMEM = STASSID;
const char* password PROGMEM = STAPSK;
#endif

// ### webpage server authen
const char* www_username PROGMEM = DEFAULT_WWW_USER;
const char* www_password PROGMEM = DEFAULT_WWW_PASS;

// ### FTP authen
#if USE_FTP
// FTP
const char* ftp_username PROGMEM = DEFAULT_FTP_USER;
const char* ftp_password PROGMEM = DEFAULT_FTP_PASS;
#endif

// ## prototype declaration
// ###  functional
void startupConfig(void);
void startupLog(void);
void clickbutton_action(void);
void PowerSensorDisplay(void);
void redisInterface_handle(void);
//float calculateDistance(int rssi);

// ###  web page handle
void handleRoot(void);
void handleNotFound(void);
void handleConfig(void);
void handleInfo(void);
void handleGraph(void);

// ###  EEPROM
void EEPROM_WriteString(char addr, String data);
String EEPROM_ReadString(char addr);
void EEPROM_WriteUInt(char address, unsigned int number);
unsigned int EEPROM_ReadUInt(char address);

// # Main Section
// ## Setup Section
void setup() {
    // ### Initialize
    ESP.wdtDisable();

    cse7766.setRX(1);
    cse7766.begin();// will initialize serial to 4800 bps
    //Serial.begin(115200);

    //  pinMode(PUSHBUTTON_PIN, INPUT);
    //  pinMode(LED_PIN, OUTPUT);
    //  digitalWrite(LED_PIN, LOW);
    blueLed.setOffSingle();//turn on blue led
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);//turn off red led

    btnTicker.attach(0.02, btnTick);
    ledTicker.attach(0.05, ledTick);

#if USE_REDIS
    redis_deviceKey.reserve(80);
    redis_server_addr.reserve(30);
    redis_server_pass.reserve(19);
#endif

    EEPROM.begin(512);

    if (EEPROM.read(EEPROM_INIT) != 1) {
        EEPROM.write(EEPROM_INIT, 1);
        delay(1);
        EEPROM.commit();
        delay(1);

#if USE_REDIS
        EEPROM_WriteString(REDIS_EEPROM_ADDR_BEGIN, REDIS_DEVKEY);
        EEPROM_WriteString(REDIS_EEPROM_SERVER_ADDR, REDIS_ADDR);
        EEPROM_WriteUInt(REDIS_EEPROM_SERVER_PORT, REDIS_PORT);
        EEPROM_WriteString(REDIS_EEPROM_SERVER_PASS, REDIS_PASS);
#endif
    }

    // ### Wait for user click
    blueLed.setPatternSingle(waitClick_pattern, 4);//one long & one short blink
    delay(10000);

    if (singleClick_flag) {
        //restore to default
#if USE_REDIS
        EEPROM_WriteString(REDIS_EEPROM_ADDR_BEGIN, REDIS_DEVKEY);
        EEPROM_WriteString(REDIS_EEPROM_SERVER_ADDR, REDIS_ADDR);
        EEPROM_WriteUInt(REDIS_EEPROM_SERVER_PORT, REDIS_PORT);
        EEPROM_WriteString(REDIS_EEPROM_SERVER_PASS, REDIS_PASS);

        redis_deviceKey = EEPROM_ReadString(REDIS_EEPROM_ADDR_BEGIN);
        redis_server_addr = EEPROM_ReadString(REDIS_EEPROM_SERVER_ADDR);
        redis_server_port = EEPROM_ReadUInt(REDIS_EEPROM_SERVER_PORT);
        redis_server_pass = EEPROM_ReadString(REDIS_EEPROM_SERVER_PASS);
#endif

#if USE_WiFiManager
        wm_reset_flag = true;
#endif

        singleClick_flag = false;
    }
    if (doubleClick_flag) {
#if USE_WiFiManager
        wm_reset_flag = true;
#endif

        doubleClick_flag = false;
    }
    if (longClick_flag) {
#if USE_WiFiManager
        wm_reset_flag = true;
#endif

        longClick_flag = false;
    }

    // ### Config wifi
    blueLed.setOffSingle();//turn on blue led

#if !USE_MDNS
    WiFi.config(local_IP, primaryDNS, gateway, subnet);
#endif

    // WiFi connection
    WiFi.mode(WIFI_STA);

#if USE_WiFiManager
    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    //wm.setConnectTimeout(20);     // how long to try to connect for before continuing

    if (wm_reset_flag) {
        wm.resetSettings();
    }

    wm.setAPCallback(configModeCallback);
    wm.setWebServerCallback(configWebServerCallback);
    wm.setSaveConfigCallback(configSaveConfigCallback);
    wm.setConfigPortalTimeout(300); //second
    wm.setDebugOutput(false);
    bool res = wm.autoConnect();    // password protected ap
    if (!res) {
        blueLed.setOnSingle();//turn off blue led
        delay(3000);
        ESP.reset();
        delay(5000);
    }
#else
    WiFi.begin(ssid, password);
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        yield();
    }

#endif

    // ### Wifi already connect, try to config device
    //blueLed.setPatternSingle(config_pattern, 2);//very fast blink

    // Register host name in WiFi and mDNS
    hostNameWifi = HOST_NAME;
    if (!hostNameWifi.endsWith(".local") ) {
        hostNameWifi.concat(".local");
    }

#ifdef ESP8266 // Only for it
    WiFi.hostname(hostNameWifi);
#endif

#if USE_MDNS
    if (MDNS.begin(HOST_NAME)) {
        //Serial.print("* MDNS responder started. Hostname -> ");
        //Serial.println(HOST_NAME);
    }
#if USE_FTP
    MDNS.addService("ftp", "tcp", 21);
#endif
#if USE_TELNET
    MDNS.addService("telnet", "tcp", 23);
#endif

    MDNS.addService("http", "tcp", 80);


#endif //USE_MDNS

    // #### webpage assign section
    // ##### client web access
    server.on("/", HTTP_GET, handleRoot);
    server.on("/info", HTTP_GET, handleInfo);
    server.on("/graph", HTTP_GET, handleGraph);
    server.onNotFound(handleNotFound);
    // ##### spiffs file support
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.serveStatic("/log.txt", SPIFFS, "/log.txt");
    server.serveStatic("/test.html", SPIFFS, "/test.html");
    server.serveStatic("/highcharts.js", SPIFFS, "/highcharts.js");
    // ##### http get/post
    server.on("/config", HTTP_POST, handleConfig);
    server.on("/on", HTTP_POST, []() {
        digitalWrite(RELAY_PIN, HIGH);
        server.send(204);
    });
    server.on("/off", HTTP_POST, []() {
        digitalWrite(RELAY_PIN, LOW);
        server.send(204);
    });
    // ##### ajax xmlHttpRequest
    server.on("/xVal", HTTP_GET, []() {//using AJAX
        String xValue = "";
        if (digitalRead(RELAY_PIN)) {
            xValue.concat("on");
        } else {
            xValue.concat("off");
        }
        xValue.concat("," + String(cse7766.getVoltage()));
        xValue.concat("," + String(cse7766.getCurrent()));
        xValue.concat("," + String(cse7766.getActivePower()));
        xValue.concat("," + String(cse7766.getApparentPower()));
        xValue.concat("," + String(cse7766.getReactivePower()));
        xValue.concat("," + String(cse7766.getPowerFactor()));
        xValue.concat("," + String(cse7766.getEnergy()));
        xValue.concat("," + WiFi.SSID() + " " + String(WiFi.RSSI()));

        int n = WiFi.scanNetworks();
        String ssni = ",";
        for (int i = 0; i < n; i++)
        {
            if (WiFi.SSID(i).startsWith("ESP")) {
                ssni.concat(WiFi.SSID(i) + " " + String(WiFi.RSSI(i)) + "  ");
                //ssni.concat(WiFi.SSID(i) + " " + String(calculateDistance(WiFi.RSSI(i))) + "  ");
            }
            if (WiFi.SSID(i) == "JUMP") {
                ssni.concat(WiFi.SSID(i) + " " + String(WiFi.RSSI(i)) + "  ");
                //ssni.concat(WiFi.SSID(i) + " " + String(calculateDistance(WiFi.RSSI(i))) + "  ");
            }
        }
        xValue.concat(ssni);
        server.send(200, "text/plain", xValue);//(comma format)
    });



#if USE_OTA
    ElegantOTA.begin(&server);    // Start ElegantOTA
#endif

    server.begin();
    
#if USE_NTP
    timeClient.begin();
#endif

    bool spiffsResult = SPIFFS.begin();
    if (spiffsResult) {
#if USE_FTP
        // FTP Setup, ensure SPIFFS is started before ftp;
        ftpSrv.begin(ftp_username, ftp_password);// Then start FTP server when WiFi connection in On
#endif

        //startupConfig();
        //startupLog();
    }

#if USE_TELNET
    // Initialize RemoteDebug
    Debug.begin(HOST_NAME); // Initialize the WiFi server
    Debug.setResetCmdEnabled(true); // Enable the reset command
    Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)
    Debug.showColors(true); // Colors

    // Debug levels
    debugA("* This is a message of debug level ANY");//always show
    debugV("* This is a message of debug level VERBOSE");
    debugD("* This is a message of debug level DEBUG");
    debugI("* This is a message of debug level INFO");
    debugW("* This is a message of debug level WARNING");
    debugE("* This is a message of debug level ERROR");
#endif

#if USE_REDIS
    redis_deviceKey = EEPROM_ReadString(REDIS_EEPROM_ADDR_BEGIN);
    redis_server_addr = EEPROM_ReadString(REDIS_EEPROM_SERVER_ADDR);
    redis_server_port = EEPROM_ReadUInt(REDIS_EEPROM_SERVER_PORT);
    redis_server_pass = EEPROM_ReadString(REDIS_EEPROM_SERVER_PASS);
#endif

#if USE_TELNET
    debugI("IP address: %s", WiFi.localIP().toString().c_str());
#if USE_REDIS
    debugI("redis_deviceKey: %s", redis_deviceKey.c_str());
    debugI("redis_server_addr: %s", redis_server_addr.c_str());
    debugI("redis_server_port: %d", redis_server_port);
    debugI("redis_server_pass: %s", redis_server_pass.c_str());
#endif

#endif

    blueLed.setOnSingle();//turn off blue led
    digitalWrite(RELAY_PIN, HIGH);//turn on red led

    // ### Device inital complete
    blueLed.setPatternSingle(initPass_pattern, 2);//one short blink
    ESP.wdtEnable(WDTO_8S);

#if !USE_NTP
    configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
    while (!time(nullptr)) {
        delay(1000);
    }
#endif
}


// ## Loop Section
unsigned long checkTime1, checkTime2, checkTime3, checkTime4, checkTime5, checkTime6, checkTime7, checkTime8, checkTime9;
unsigned long checkDiffTime1, checkDiffTime2, checkDiffTime3, checkDiffTime4, checkDiffTime5, checkDiffTime6, checkDiffTime7,checkDiffTime8, checkDiffTime9;

void loop()
{
    checkTime1 = millis();
    // Each second
    if ((millis() - mLastTime) >= 1000) {
        // Time
        mLastTime = millis();
        mTimeSeconds++;

        if (mTimeSeconds % 5 == 0) { // Each 5 seconds
            //PowerSensorDisplay();
        }

#if USE_REDIS
        if (mTimeSeconds % redisPeriod == 1) {
            redisInterface_flag = true;
        }
#endif

#if !USE_NTP
        if (mTimeSeconds % 60 == 2) { // Each minute
            configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
            //need some time delay slot****
        }

        checkTime6 = millis();
        time_t now = time(nullptr);
        struct tm* p_tm = localtime(&now);
        debugD("t: %s", asctime (p_tm));//human readable
        debugD("epoch: %ld", now);//epoch:unix timestamp
        checkDiffTime6 = millis() - checkTime6;
#endif

    }
    checkDiffTime1 = millis() - checkTime1;

    checkTime2 = millis();
    cse7766.handle();// CSE7766 handle
    checkDiffTime2 = millis() - checkTime2;

    checkTime3 = millis();
    clickbutton_action();
    checkDiffTime3 = millis() - checkTime3;

#if USE_FTP
    checkTime4 = millis();
    ftpSrv.handleFTP();
    checkDiffTime4 = millis() - checkTime4;
#endif

    checkTime5 = millis();
    server.handleClient();
    checkDiffTime5 = millis() - checkTime5;

    //    checkTime6 = millis();
#if USE_NTP
    checkTime6 = millis();
    timeClient.update();
    checkDiffTime6 = millis() - checkTime6;
#endif

#if USE_REDIS
checkTime7 = millis();
    redisInterface_handle();
    checkDiffTime7 = millis() - checkTime7;
#endif

#if USE_MDNS
    checkTime8 = millis();
    MDNS.update();
    checkDiffTime8 = millis() - checkTime8;
#endif




#if USE_TELNET
    checkTime9 = millis();
    Debug.handle();// RemoteDebug handle
    checkDiffTime9 = millis() - checkTime9;
#endif


    ESP.wdtFeed();

    // Give a time for ESP
    yield();

    if (checkDiffTime1 >= 10) {
        debugW("Each second: %lu", checkDiffTime1);
    }
    if (checkDiffTime2 >= 10) {
        debugW("CSE7766: %lu", checkDiffTime2);
    }
    if (checkDiffTime3 >= 10) {
        debugW("clickbutton: %lu", checkDiffTime3);
    }
    if (checkDiffTime4 >= 10) {
        debugW("FTP: %lu", checkDiffTime4);
    }
    if (checkDiffTime5 >= 10) {
        debugW("handleClient: %lu", checkDiffTime5);
    }
    if (checkDiffTime6 >= 10) {
        debugW("NTP: %lu", checkDiffTime6);
    }
    if (checkDiffTime7 >= 10) {
        debugW("REDIS: %lu", checkDiffTime7);
    }
    if (checkDiffTime8 >= 10) {
        debugW("MDNS: %lu", checkDiffTime8);
    }
    if (checkDiffTime9 >= 10) {
        debugW("TELNET: %lu", checkDiffTime9);
    }

}

// # Member Function Definition Section
void clickbutton_action(void) {
    if ( singleClick_flag) {

#if USE_TELNET
        debugD("SingleClick");
        if (digitalRead(RELAY_PIN)) {
            debugI("status on\n");
        } else {
            debugI("status off\n");
        }
#if USE_REDIS
        debugI("redis_deviceKey: %s", redis_deviceKey.c_str());
        debugI("redis_server_addr: %s", redis_server_addr.c_str());
        debugI("redis_server_port: %d", redis_server_port);
        debugI("redis_server_pass: %s", redis_server_pass.c_str());
        //debugI("cse7766: %s", cse7766.description().c_str());
#endif
        debugW("ssid %s", WiFi.SSID().c_str());

        //WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);

        if (WiFi.status() == WL_CONNECTED) {
            debugW("connected");
        } else if (WiFi.status() == WL_NO_SHIELD) {
            debugW("no WiFi shield is present");
        } else if (WiFi.status() == WL_IDLE_STATUS) {
            debugW("idle");
        } else if (WiFi.status() == WL_NO_SSID_AVAIL) {
            debugW("no SSID are available");
        } else if (WiFi.status() == WL_SCAN_COMPLETED) {
            debugW("scan networks is completed");
        } else if (WiFi.status() == WL_CONNECT_FAILED) {
            debugW("connection fails for all the attempts");
        } else if (WiFi.status() == WL_CONNECTION_LOST) {
            debugW("connection is lost");
        } else if (WiFi.status() == WL_DISCONNECTED) {
            debugW("disconnected from a network");
        } else {
            debugW("status error");
        }
#endif
        blueLed.setPatternSingle(initPass_pattern, 2);

        File configFile = SPIFFS.open("/config.txt", "r");
        if (configFile)
        {
            while (configFile.available())
            {
                //read line by line from the file
                String line = configFile.readStringUntil('\n');
#if USE_TELNET
                debugI("%s", line.c_str());
#endif
                //        String resultstr;
                //        if (line.startsWith("ssid")) {
                //          resultstr = line.substring(line.indexOf(",") + 1);
                //          resultstr.trim();
                //          resultstr.toCharArray(ssid, resultstr.length() + 1);
                //          debugI("-> %s", resultstr.c_str());
                //          debugI("--> %s", ssid);
                //        } else if (line.startsWith("pass")) {
                //          resultstr = line.substring(line.indexOf(",") + 1);
                //          resultstr.trim();
                //          resultstr.toCharArray(password, resultstr.length() + 1);
                //          debugI("-> %s", resultstr.c_str());
                //          debugI("--> %s", password);
                //        } else {
                //          debugI("%s", line.c_str());
                //        }
            }
        }
        configFile.close();

        singleClick_flag = false;
    }
    if ( doubleClick_flag) {
        digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
#if USE_TELNET
        debugD("DoubleClick");
#endif

        doubleClick_flag = false;
    }

    //    if (S31_Button.isLongClick()) {
    //        //restore to default
    //        EEPROM_WriteString(REDIS_EEPROM_ADDR_BEGIN, REDIS_DEVKEY);
    //        EEPROM_WriteString(REDIS_EEPROM_SERVER_ADDR, REDIS_ADDR);
    //        EEPROM_WriteUInt(REDIS_EEPROM_SERVER_PORT, REDIS_PORT);
    //        EEPROM_WriteString(REDIS_EEPROM_SERVER_PASS, REDIS_PASS);
    //
    //        redis_deviceKey = EEPROM_ReadString(REDIS_EEPROM_ADDR_BEGIN);
    //        redis_server_addr = EEPROM_ReadString(REDIS_EEPROM_SERVER_ADDR);
    //        redis_server_port = EEPROM_ReadUInt(REDIS_EEPROM_SERVER_PORT);
    //        redis_server_pass = EEPROM_ReadString(REDIS_EEPROM_SERVER_PASS);
    //
    //#if USE_TELNET
    //        debugD("LongClick");
    //#endif
    //
    //#if USE_WiFiManager
    //        wm.resetSettings();
    //        ESP.restart();
    //#endif
    //    }
}
void PowerSensorDisplay(void) {
#if USE_TELNET
    debugV("Voltage %.4f V\n", cse7766.getVoltage());
    debugV("Current %.4f A\n", cse7766.getCurrent());
    debugV("ActivePower %.4f W\n", cse7766.getActivePower());
    debugV("ApparentPower %.4f VA\n", cse7766.getApparentPower());
    debugV("ReactivePower %.4f VAR\n", cse7766.getReactivePower());
    debugV("PowerFactor %.4f %%\n", cse7766.getPowerFactor());
    debugV("Energy %.4f Ws\n", cse7766.getEnergy());
#endif
}
void startupConfig(void) {
    //  //timeClient.update();
    //  timeClient.forceUpdate();
    //
    //  File configFile = SPIFFS.open("/config.txt", "r");
    //  if (!configFile)
    //  {
    //    return;
    //  }
    //  while (configFile.available())
    //  {
    //    String line = configFile.readStringUntil('\n');
    //    //      String resultstr;
    //    //      if (line.startsWith("xxxx")) {
    //    //        resultstr = line.substring(line.indexOf(",") + 1);
    //    //        resultstr.trim();
    //    //        resultstr.toCharArray(XXXX, resultstr.length() + 1);//global char* XXXX = "initial";
    //    //      } else if (line.startsWith("yyyy")) {
    //    //        resultstr = line.substring(line.indexOf(",") + 1);
    //    //        resultstr.trim();
    //    //        resultstr.toCharArray(YYYY, resultstr.length() + 1);//global char* YYYY = "initial";
    //    //      }
    //  }
    //  configFile.close();
}
void startupLog(void) {
    //    //timeClient.update();
    //    timeClient.forceUpdate();
    //    timeClient.forceUpdate();
    //
    //    //a+ -> Open for reading and appending (writing at end of file).
    //    //The file is created if it does not exist.
    //    File logFile = SPIFFS.open("/log.txt", "a+");
    //    if (!logFile) {
    //        return;
    //    }
    //    if (logFile.size() < 2000) {
    //        int bytesWritten = logFile.print(timeClient.getEpochTime());
    //        bytesWritten = logFile.print(",");
    //        bytesWritten = logFile.println( ESP.getResetReason());
    //        //bytesWritten = logFile.println(timeClient.getFormattedTime());
    //
    //    }
    //    logFile.close();
}
#if USE_REDIS
void redisInterface_handle(void) {
    String redis_key;
    String cse7766_value;
    String redis_str_result;
    bool redis_bool_result;

    if (redisInterface_flag == true) {
        if (redisInterface_state == 0) {//WIFI CLIENT
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
        } else if (redisInterface_state == 1) {
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
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // Current
            redis_key = redis_deviceKey + String(redis_current);
            cse7766_value = String(cse7766.getCurrent());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // ActivePower
            redis_key = redis_deviceKey + String(redis_activepower);
            cse7766_value = String(cse7766.getActivePower());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // ApparentPower
            redis_key = redis_deviceKey + String(redis_apparentpower);
            cse7766_value = String(cse7766.getApparentPower());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            redisInterface_state++;
        } else if (redisInterface_state == 2) {
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
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // PowerFactor
            redis_key = redis_deviceKey + String(redis_powerfactor);
            cse7766_value = String(cse7766.getPowerFactor());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // Energy
            redis_key = redis_deviceKey + String(redis_energy);
            cse7766_value = String(cse7766.getEnergy());
#if USE_TELNET
            debugD("SET %s %s: ", redis_key.c_str(), cse7766_value.c_str());
#endif
            redis_bool_result = redis.set(redis_key.c_str(), cse7766_value.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            // TimeStamp
            redis_key = redis_deviceKey + String(redis_timestamp);
            //            String timeStamp = timeClient.getFormattedTime();
#if USE_TELNET
            //            debugD("SET %s %s: ", redis_key.c_str(), timeStamp.c_str());
#endif
            //            redis_bool_result = redis.set(redis_key.c_str(), timeStamp.c_str());
#if USE_TELNET
            if (redis_bool_result) {
                debugD("ok!");
            } else {
                debugE("err");
            }
#if REDIS_GET_TEST
            redis_str_result = redis.get(redis_key.c_str());
            debugD("GET %s: %s", redis_key.c_str(), redis_str_result.c_str());
#endif
#endif

            redisInterface_state++;
        } else if (redisInterface_state == 3) {
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
#endif

#if USE_WiFiManager
void configModeCallback (WiFiManager *myWiFiManager) {//start AP mode
    blueLed.setPatternSingle(wifiManager_pattern, 2);//fast continue blink
}
void configWebServerCallback() {//called after webserver has started

}
void configSaveConfigCallback() {//called when wifi settings have been changed and connection was successful ( or setBreakAfterConfig(true) )
    blueLed.setPatternSingle(config_pattern, 2);//very fast blink
}

#endif
//float calculateDistance(int rssi) {
//  if (rssi == 0) {
//    return -1.0;
//  }
//
//  float pl = (rssi + 2.031862) / (-3.9955);
//  float distance =  pow( 10, pl);
//  return distance;
//}
