#include <Arduino.h>
#include "LogLibrary.h"

const char *ssid = "wifiNetwork";
const char *password = "wifiPassword";
const char *mqttServer = "IP_BROKER";
const int mqttPort = 1883;
const char *mqttTopic = "mqttTopic";
const char *idBoard = "idBoard";

// Change TimeZone https://remotemonitoringsystems.ca/time-zone-abbreviations.php
// Default timezone is Madrid
// const char *timeZone = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";

// Change ntpServer https://www.ntppool.org/es/zone/es
// Default ntpServer is pool.ntp.org
// const char *ntpServer = "pool.ntp.org";

// Change log format optionally
// Default format: "{level}-{message}-{idSender}-{topic}-{timestamp}"
// const char *logFormat1 = "{level} - {message}";
// const char *logFormat2 = "{message} - {timestamp} - {idSender}";
// const char *logFormat3 = "{level} - {message} - {idSender}";

// Init logger with different configurations
// Logger logger(true); -> Save logs in database
// Logger logger(false); -> Do not save logs in database
Logger logger(false);

void userFunctionality();

void setup()
{
    Serial.begin(9600);

    // Initialize logger with different configurations
    logger.init(ssid, password, mqttServer, mqttPort);
    // logger.init(ssid, password, mqttServer, mqttPort, mqttTopic);
    // logger.init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard);
    // logger.init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard, timeZone);
    // logger.init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard, timeZone, ntpServer);

    // Set log format
    // logger.setLogFormat(logFormat2);
    // logger.setLogFormat(logFormat3);
    // logger.setLogFormat(logFormat4);
}

void loop()
{
    // Random number between 0 and 25
    int number = random(0, 26);

    // User functionality
    userFunctionality();

    if (number >= 0 && number <= 5)
    {
        logger.logDEBUG("Debug in the user's board");
    }
    else if (number >= 6 && number <= 10)
    {
        logger.logWARNING("Warning in the user's board");
    }
    else if (number >= 11 && number <= 15)
    {
        logger.logERROR("Error in the user's board");
    }
    else if (number >= 16 && number <= 20)
    {
        logger.logINFO("Information in the user's board");
    }
    else if (number >= 21 && number <= 25)
    {
        logger.logCRITICAL("Critical in the user's board");
    }
}

void userFunctionality()
{
    // Functionality of the user's board
    // Its operation should not be interrupted
    Serial.println("User functionality running...");
    delay(500);
}