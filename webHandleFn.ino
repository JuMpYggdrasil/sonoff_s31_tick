void handleRoot(void) {
    String rootPage = "";
    //authentication
    if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
    }

#if USE_REDIS
    redis_deviceKey = EEPROM_ReadString(REDIS_EEPROM_ADDR_BEGIN);
    redis_server_addr = EEPROM_ReadString(REDIS_EEPROM_SERVER_ADDR);
    redis_server_port = EEPROM_ReadUInt(REDIS_EEPROM_SERVER_PORT);
    redis_server_pass = EEPROM_ReadString(REDIS_EEPROM_SERVER_PASS);
#endif

    rootPage.concat(FPSTR(WEB_HEAD));
    rootPage.concat(FPSTR(WEB_STYLE));
    rootPage.concat(FPSTR(WEB_BODY_START));
    rootPage.concat(FPSTR(WEB_SIDENAV));
    rootPage.concat(FPSTR(WEB_CONTENT_START));
#if USE_REDIS
    rootPage.concat("<div>" + hostNameWifi + " : " + WiFi.localIP().toString() + "</div><br>");
    rootPage.concat(F("<form action=\"/config\" method=\"POST\">"));
    rootPage.concat(F("<label for=\"name1\">Device key:  </label><br>"));
    rootPage.concat("<input type=\"text\" name=\"name1\" placeholder=\"" + redis_deviceKey + "\"><br>");
    rootPage.concat(F("<label for=\"name2\">Redis addr:  </label><br>"));
    rootPage.concat("<input type=\"text\" name=\"name2\" placeholder=\"" + redis_server_addr + "\"><br>");
    rootPage.concat(F("<label for=\"name3\">Redis port:  </label><br>"));
    rootPage.concat("<input type=\"text\" name=\"name3\" placeholder=\"" + String(redis_server_port) + "\"><br>");
    rootPage.concat(F("<label for=\"name4\">Redis pass:  </label><br>"));
    rootPage.concat("<input type=\"password\" name=\"name4\" placeholder=\"" + redis_server_pass + "\"><br>");
    rootPage.concat(F("<input type=\"submit\" value=\"Save\">"));
    rootPage.concat(F("</form></br>"));
    rootPage.concat(F("<form action=\"/on\" method=\"POST\">"));
    rootPage.concat(F("<input type=\"submit\" value=\"relay on\">"));
    rootPage.concat(F("</form>"));
    rootPage.concat(F("<form action=\"/off\" method=\"POST\">"));
    rootPage.concat(F("<input type=\"submit\" value=\"relay off\">"));
    rootPage.concat(F("</form>"));
    rootPage.concat(F("<div id='x0Val'></div>"));
    rootPage.concat(F("<div id='x1Val'></div>"));
    rootPage.concat(FPSTR(WEB_SCRIPT_START));
    rootPage.concat("setInterval(function xmlDataRequest(){");
    rootPage.concat("var xhttp = new XMLHttpRequest();");
    rootPage.concat("xhttp.onreadystatechange = function() {");
    rootPage.concat("if (this.readyState == 4 && this.status == 200) {");
    rootPage.concat("var resultText = this.responseText.split(',');");
    rootPage.concat("document.getElementById('x0Val').innerHTML = resultText[8];");
    rootPage.concat("document.getElementById('x1Val').innerHTML = resultText[9];}");
    rootPage.concat("};");//function()
    rootPage.concat("xhttp.open('GET', '/xVal', true);");
    rootPage.concat("xhttp.send();");
    rootPage.concat("}, 2000 ) ; ");
    rootPage.concat(FPSTR(WEB_SCRIPT_HTML_END));
#else
    rootPage.concat(FPSTR(WEB_BODY_HTML_END));
#endif
    // Root web page
    server.send(200, "text / html", rootPage);
}
void handleInfo(void) {
    String infoPage = "";
    //authentication
    if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
    }

    infoPage.concat(FPSTR(WEB_HEAD));
    infoPage.concat(FPSTR(WEB_STYLE));
    infoPage.concat(FPSTR(WEB_BODY_START));
    infoPage.concat(FPSTR(WEB_SIDENAV));
    infoPage.concat(FPSTR(WEB_CONTENT_START));

    infoPage.concat(F("<div style=\"font-weight:bold\">Network</div>"));
    infoPage.concat("<div>IP: " + WiFi.localIP().toString() + "</div>");
    infoPage.concat("<div>SSID: " + WiFi.SSID() + "</div>");
    infoPage.concat("<div>RSSI: " + String(WiFi.RSSI()) + "</div>");

    infoPage.concat(F("<br><div style=\"font-weight:bold\">Hardware</div>"));
    infoPage.concat(F("<div>Relay Status: <span id='x0Val'></span></div>"));
    infoPage.concat(F("<div>Voltage: <span id='x1Val'></span></div>"));
    infoPage.concat(F("<div>Current: <span id='x2Val'></span></div>"));
    infoPage.concat(F("<div>ActivePower: <span id='x3Val'></span></div>"));
    infoPage.concat(F("<div>ApparentPower: <span id='x4Val'></span></div>"));
    infoPage.concat(F("<div>ReactivePower: <span id='x5Val'></span></div>"));
    infoPage.concat(F("<div>PowerFactor: <span id='x6Val'></span></div>"));
    infoPage.concat(F("<div>Energy: <span id='x7Val'></span></div>"));

    FSInfo fs_info;
    SPIFFS.info(fs_info);

    infoPage.concat(F("<br><div style=\"font-weight:bold\">Filesystem information</div>"));
    infoPage.concat("<div>totalBytes " + String(fs_info.totalBytes) + "</div>");
    infoPage.concat("<div>usedBytes " + String(fs_info.usedBytes) + "</div>");

    Dir dir = SPIFFS.openDir ("");
    while (dir.next ()) {
        infoPage.concat("<div>" + String(dir.fileName ()) + "," + dir.fileSize () + "</div>");
    }

    infoPage.concat(F("<form action='/upload' method='post' enctype='multipart/form-data'>"));
    infoPage.concat(F("<input type='file' name='uploadBtn'>"));
    infoPage.concat(F("<input class='button' type='submit'>"));
    infoPage.concat(F("</form>"));
    infoPage.concat(F("<div>*note: FTP supported</div>"));

    infoPage.concat(FPSTR(WEB_SCRIPT_START));
    infoPage.concat("setInterval(function xmlDataRequest(){");
    infoPage.concat("var xhttp = new XMLHttpRequest();");
    infoPage.concat("xhttp.onreadystatechange = function() {");
    infoPage.concat("if (this.readyState == 4 && this.status == 200) {");
    infoPage.concat("var resultText = this.responseText.split(',');");
    infoPage.concat("document.getElementById('x0Val').innerHTML = resultText[0];");
    infoPage.concat("document.getElementById('x1Val').innerHTML = resultText[1];");
    infoPage.concat("document.getElementById('x2Val').innerHTML = resultText[2];");
    infoPage.concat("document.getElementById('x3Val').innerHTML = resultText[3];");
    infoPage.concat("document.getElementById('x4Val').innerHTML = resultText[4];");
    infoPage.concat("document.getElementById('x5Val').innerHTML = resultText[5];");
    infoPage.concat("document.getElementById('x6Val').innerHTML = resultText[6];");
    infoPage.concat("document.getElementById('x7Val').innerHTML = resultText[7];}");
    infoPage.concat("};");//function()
    infoPage.concat("xhttp.open('GET', '/xVal', true);");
    infoPage.concat("xhttp.send();");
    infoPage.concat("}, 2000 ) ; ");
    infoPage.concat(FPSTR(WEB_SCRIPT_HTML_END));
    //infoPage.concat(FPSTR(WEB_BODY_HTML_END));
    // Info web page
    server.send(200, "text/html", infoPage);
}

void handleConfig(void) {
    String configPage = "";

    if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
    }

    configPage.concat(FPSTR(WEB_HEAD));
    configPage.concat(FPSTR(WEB_STYLE));
    configPage.concat(FPSTR(WEB_BODY_START));
    configPage.concat(FPSTR(WEB_SIDENAV));
    configPage.concat(FPSTR(WEB_CONTENT_START));

    configPage.concat(F("<div>config ok</div>"));
    configPage.concat(FPSTR(WEB_BODY_HTML_END));
    if ((server.hasArg("name1")) && (server.hasArg("name2")) && (server.hasArg("name3")) && (server.hasArg("name4"))) {
#if USE_REDIS
        EEPROM_WriteString(REDIS_EEPROM_ADDR_BEGIN, server.arg("name1"));
        EEPROM_WriteString(REDIS_EEPROM_SERVER_ADDR, server.arg("name2"));
        EEPROM_WriteUInt(REDIS_EEPROM_SERVER_PORT, server.arg("name3").toInt());
        EEPROM_WriteString(REDIS_EEPROM_SERVER_PASS, server.arg("name4"));

        redis_deviceKey = EEPROM_ReadString(REDIS_EEPROM_ADDR_BEGIN);
        redis_server_addr = EEPROM_ReadString(REDIS_EEPROM_SERVER_ADDR);
        redis_server_port = EEPROM_ReadUInt(REDIS_EEPROM_SERVER_PORT);
        redis_server_pass = EEPROM_ReadString(REDIS_EEPROM_SERVER_PASS);
#endif

        server.send(200, "text/html", configPage);
#if USE_TELNET
        debugD("config ok");
#endif
    } else {
        server.send(200, "text/plain", "config error");
#if USE_TELNET
        debugE("config error");
#endif
    }
}

void handleGraph(void) {
    String graphPage = "";
    //authentication
    if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
    }

    graphPage.concat(FPSTR(WEB_HEAD));
    graphPage.concat(FPSTR(WEB_LOAD_SCRIPT));
    graphPage.concat(FPSTR(WEB_STYLE));
    graphPage.concat(FPSTR(WEB_BODY_START));
    graphPage.concat(FPSTR(WEB_SIDENAV));
    graphPage.concat(FPSTR(WEB_CONTENT_START));

    graphPage.concat(F("<div id=\"chart-temperature\" class=\"container\"></div>"));

    graphPage.concat(FPSTR(WEB_SCRIPT_START));

    graphPage.concat("var chartT=new Highcharts.Chart({chart:{renderTo:\"chart-temperature\"},title:{text:\"CSE7766 Power\"},series:[{showInLegend:!1,data:[]}],plotOptions:{line:{animation:!1,dataLabels:{enabled:!0}},series:{color:\"#059e8a\"}},xAxis:{type:\"datetime\",dateTimeLabelFormats:{second:\"%H:%M:%S\"}},yAxis:{title:{text:\"Power (watt)\"}},credits:{enabled:!1}});setInterval(function(){var e=new XMLHttpRequest;e.onreadystatechange=function(){if(4==this.readyState&&200==this.status){var e=this.responseText.split(\",\"),t=(new Date).getTime(),a=parseFloat(e[3]);chartT.series[0].data.length>60?chartT.series[0].addPoint([t,a],!0,!0,!0):chartT.series[0].addPoint([t,a],!0,!1,!0)}},e.open(\"GET\",\"/xVal\",!0),e.send()},1e3);");

    graphPage.concat(FPSTR(WEB_SCRIPT_HTML_END));
    //graphPage.concat(FPSTR(WEB_BODY_HTML_END));
    // Info web page
    server.send(200, "text/html", graphPage);
}

void handleFileUpload() {
    //if (server.uri() != "/info") return;
    
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        //DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
        fsUploadFile = SPIFFS.open(filename, "w");
        filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile)
            fsUploadFile.close();
        //DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
    }

    String uploadPage = "";
    uploadPage.concat(FPSTR(WEB_HEAD));
    uploadPage.concat(FPSTR(WEB_STYLE));
    uploadPage.concat(FPSTR(WEB_BODY_START));
    uploadPage.concat(FPSTR(WEB_SIDENAV));
    uploadPage.concat(FPSTR(WEB_CONTENT_START));

    uploadPage.concat(F("<div>file upload ok</div>"));
    uploadPage.concat(FPSTR(WEB_BODY_HTML_END));
    
    server.send(200, "text/html", uploadPage);
    
}

void handleNotFound() {
    server.send(404, "text / plain", "404: Not found");// Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
