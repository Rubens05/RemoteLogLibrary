#ifndef RemoteLogLibrary_h
#define RemoteLogLibrary_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <thread>
#include <queue>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <regex>

class Logger
{
public:
    Logger();
    ~Logger();

    bool saveInDB;

    const char *mqttTopic = "defaultTopic";
    const char *idBoard = "defaultIdBoard";
    const char *mqttServer;
    int mqttPort;
    const char *mqttUser;
    const char *mqttPassword;
    const char *timeZone = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"; // Change "timeZone" according to time zone https://remotemonitoringsystems.ca/time-zone-abbreviations.php
    const char *ntpServer = "pool.ntp.org";                                // Change to the NTP server of your region https://www.ntppool.org/es/zone/es
    const char *logFormat = "{level}-{message}-{idSender}-{topic}-{timestamp}";

    void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort);
    void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic);
    void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard);
    void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone);
    void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone, const char *ntpServer);

    void sendToMQTT(const char *mqttUser, const char *mqttPassword);

    void setLogFormat(const char *format);
    void logINFO(const char *message);
    void logWARNING(const char *message);
    void logERROR(const char *message);
    void logCRITICAL(const char *message);
    void logDEBUG(const char *message);

    // Functions to simulate multiple devices
    void setBoardId(const char *id);
    void setMqttTopic(const char *topic);

private:
    struct logMessage
    {
        const char *level;
        const char *message;
        const char *idSender;
        const char *topic;
        String timestamp;
    };
    bool authenticated;
    QueueHandle_t xQueue;

    WiFiClient espClient;
    WiFiClient setupWifi(const char *ssid, const char *password);
    void setupTime();
    void createLog(const char *level, const char *message);
    void vReceiverTask(void *pvParam);
    String getLocalTimeString();
    String buildMessage(logMessage log);
    JsonDocument buildJson(logMessage log);
};

#endif
