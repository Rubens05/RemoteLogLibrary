#include "LogLibrary.h"

Logger::Logger()
{
    // Constructor
}

Logger::~Logger()
{
    // Destructor
}

void Logger::init(const char *ssid, const char *password, const char *mqttServer, int mqttPort)
{
    init(ssid, password, mqttServer, mqttPort, this->mqttTopic, this->idBoard, this->timeZone, this->ntpServer);
}
void Logger::init(const char *ssid, const char *password, const char *mqttServer, int mqttPort, const char *mqttTopic)
{
    init(ssid, password, mqttServer, mqttPort, mqttTopic, this->idBoard, this->timeZone, this->ntpServer);
}
void Logger::init(const char *ssid, const char *password, const char *mqttServer, int mqttPort, const char *mqttTopic, const char *idBoard)
{
    init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard, this->timeZone, this->ntpServer);
}
void Logger::init(const char *ssid, const char *password, const char *mqttServer, int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone)
{
    init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard, timeZone, this->ntpServer);
}
void Logger::init(const char *ssid, const char *password, const char *mqttServer, int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone, const char *ntpServer)
{
    if (!ssid || !password || !mqttServer || !mqttPort)
    {
        Serial.println("Error: SSID, password, MQTT server or MQTT port missing.");
        return;
    }
    this->idBoard = idBoard;
    this->mqttServer = mqttServer;
    this->mqttPort = mqttPort;
    this->mqttTopic = mqttTopic;
    this->timeZone = timeZone;
    this->ntpServer = ntpServer;
    this->espClient = setupWifi(ssid, password);

    setupTime();

    this->xQueue = xQueueCreate(10, sizeof(logMessage));

    if (!this->xQueue)
    {
        Serial.println("Error: Failed to create queue.");
        return;
    }

    xTaskCreate(
        [](void *pvParameters)
        {
            Logger *logger = (Logger *)pvParameters;
            logger->vReceiverTask(pvParameters);
            vTaskDelete(NULL);
        },
        "vReceiverTask",
        10000,
        this,
        5,
        NULL);
}

void Logger::setLogFormat(const char *format)
{
    if (!format)
    {
        Serial.println("Error: Log format missing. Using default format.");
        delay(3000);
        return;
    }
    this->logFormat = format;
}

WiFiClient Logger::setupWifi(const char *ssid, const char *password)
{
    if (!ssid || !password)
    {
        Serial.println("Error: SSID or password missing for WiFi setup.");
        return WiFiClient();
    }
    WiFiClient espClient;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");
    return espClient;
}

void Logger::setupTime()
{
    setenv("TZ", this->timeZone, 1);
    const long gmtOffset_sec = 3600;
    const int daylightOffset_sec = 3600;

    try
    {
        configTime(gmtOffset_sec, daylightOffset_sec, this->ntpServer);
    }
    catch (std::exception &e)
    {
        Serial.println("Error: Failed to configure time.");
    }
}

void Logger::logINFO(const char *message)
{
    if (!message || message == "")
    {
        Serial.println("Error: Log message invalid.");
        return;
    }

    createLog("INFO", message);
}

void Logger::logWARNING(const char *message)
{
    if (!message || message == "")
    {
        Serial.println("Error: Log message invalid.");
        return;
    }
    createLog("WARNING", message);
}

void Logger::logERROR(const char *message)
{
    if (!message || message == "")
    {
        Serial.println("Error: Log message invalid.");
        return;
    }
    createLog("ERROR", message);
}

void Logger::logCRITICAL(const char *message)
{
    if (!message || message == "")
    {
        Serial.println("Error: Log message invalid.");
        return;
    }
    createLog("CRITICAL", message);
}

void Logger::logDEBUG(const char *message)
{
    if (!message || message == "")
    {
        Serial.println("Error: Log message invalid.");
        return;
    }
    createLog("DEBUG", message);
}

void Logger::createLog(const char *level, const char *message)
{
    logMessage log;
    log.level = level;
    log.message = message;
    log.idSender = idBoard;
    log.topic = mqttTopic;
    log.timestamp = getLocalTimeString();
    if (!xQueueSend(xQueue, &log, portMAX_DELAY))
    {
        Serial.println("Error: Failed to send log message to the queue.");
    }
}

void Logger::vReceiverTask(void *pvParam)
{
    PubSubClient client(this->espClient);
    client.setServer(this->mqttServer, this->mqttPort);
    Serial.println("Connecting to the MQTT server...");
    if (!client.connect("ESP32Client"))
    {
        Serial.println("Error: Failed to connect to MQTT server.");
        vTaskDelete(NULL);
        return;
    }
    logMessage log;
    portBASE_TYPE xStatus;
    const portTickType xTicksToWait = 10000 / portTICK_RATE_MS;
    for (;;)
    {
        xStatus = xQueueReceive(xQueue, &log, xTicksToWait);
        if (xStatus == pdPASS)
        {
            String message = buildMessage(log);
            Serial.println(message);
            if (client.publish(log.topic, message.c_str()))
            {
                Serial.println("Message posted on MQTT server");
            }
            else
            {
                Serial.println("Error posting the message on the MQTT server");
            };
        }
        else
        {
            Serial.println("No message received...");
        }
    }
    vTaskDelete(NULL);
}

String Logger::getLocalTimeString()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "";
    }
    char timeString[33];
    strftime(timeString, sizeof(timeString), "%B %d %Y %H:%M:%S", &timeinfo);
    return String(timeString);
}

String Logger::buildMessage(logMessage log)
{
    // Flags to avoid repeating values
    bool level = false;
    bool message = false;
    bool idSender = false;
    bool timestamp = false;

    const char *format = this->logFormat;
    std::stringstream ss;

    // Regular expression to find the values inside the square brackets
    std::regex pattern("\\{(.*?)\\}");

    // Convert string to string object
    std::string logFormatStr(format);

    // Object to store the split results
    std::smatch match;

    // Iterate on the coincidences found.
    while (std::regex_search(logFormatStr, match, pattern))
    {
        // Check if a value was found
        if (match.size() == 0)
        {
            Serial.println("Error: Invalid input. Must be a string with at least one value in curly braces like {level}, {message}, {idSender} or {timestamp}");
            break;
        }
        // Update the stringstream with the found value
        if (strcmp(match[1].str().c_str(), "level") == 0 && !level)
        {
            ss << "[" << log.level << "]";
            ss << " - "; // Separator between values
            level = true;
        }
        else if (strcmp(match[1].str().c_str(), "message") == 0 && !message)
        {
            ss << "Message: " << log.message;
            ss << " - "; // Separator between values
            message = true;
        }
        else if (strcmp(match[1].str().c_str(), "idSender") == 0 && !idSender)
        {
            ss << "ID: " << log.idSender;
            ss << " - "; // Separator between values
            idSender = true;
        }
        else if (strcmp(match[1].str().c_str(), "timestamp") == 0 && !timestamp)
        {
            ss << "Timestamp: " << log.timestamp.c_str();
            ss << " - "; // Separator between values
            timestamp = true;
        }

        // Update the string to look for the next match
        logFormatStr = match.suffix().str();
    }

    return ss.str().c_str();
}

// TODO method to send the json to mongo

// TODO create jSON
