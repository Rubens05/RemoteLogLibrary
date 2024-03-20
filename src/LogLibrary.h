#ifndef LogLibrary_h
#define LogLibrary_h

#include "Arduino.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <queue>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

class Logger
{
public:
    Logger();
    ~Logger();
    const char *mqttTopic;
    const char *idBoard;
    const char *mqttServer;
    int mqttPort;

    void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard);
    void setTopic(const char *topic);
    void setMqttServer(const char *mqttServer);
    void setMqttPort(const int mqttPort);
    void setIdBoard(const char *idBoard);
    void logINFO(const char *message);
    void logWARNING(const char *message);
    void logERROR(const char *message);
    void logCRITICO(const char *message);

private:
    struct logMessage
    {
        const char *level;
        const char *message;
        const char *idSender;
        const char *topic;
        std::string timestamp; // Cambiando a std::string para almacenar el timestamp
    };
    std::queue<logMessage> queue;
    WiFiClient espClient;
    // PubSubClient client;
    WiFiClient setupWifi(const char *ssid, const char *password);
    // PubSubClient setupMqtt(const char *mqttServer, const int mqttPort, WiFiClient espClient);
    void publishMqtt();
};

#endif
