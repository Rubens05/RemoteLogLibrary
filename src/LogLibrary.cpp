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
    this->idBoard = idBoard;
    this->mqttServer = mqttServer;
    this->mqttPort = mqttPort;
    this->mqttTopic = mqttTopic;
    this->timeZone = timeZone;
    this->ntpServer = ntpServer;
    this->espClient = setupWifi(ssid, password);

    setupTime();

    this->xQueue = xQueueCreate(10, sizeof(logMessage));

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

WiFiClient Logger::setupWifi(const char *ssid, const char *password)
{
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

    configTime(gmtOffset_sec, daylightOffset_sec, this->ntpServer);
}

void Logger::logINFO(const char *message)
{
    createLog("INFO", message);
}

void Logger::logWARNING(const char *message)
{
    createLog("WARNING", message);
}

void Logger::logERROR(const char *message)
{
    createLog("ERROR", message);
}

void Logger::logCRITICO(const char *message)
{
    createLog("CRITICO", message);
}

void Logger::createLog(const char *level, const char *message)
{
    logMessage log;
    log.level = level;
    log.message = message;
    log.idSender = idBoard;
    log.topic = mqttTopic;
    log.timestamp = getLocalTimeString();
    xQueueSend(xQueue, &log, portMAX_DELAY);
}

void Logger::vReceiverTask(void *pvParam)
{
    PubSubClient client(this->espClient);               // Crear cliente MQTT a partir del cliente WiFi
    client.setServer(this->mqttServer, this->mqttPort); // Setear servidor MQTT
    Serial.println("Conectando al servidor MQTT...");
    client.connect("ESP32Client");
    logMessage log;
    portBASE_TYPE xStatus;
    const portTickType xTicksToWait = 10000 / portTICK_RATE_MS;
    for (;;)
    {
        xStatus = xQueueReceive(xQueue, &log, xTicksToWait);
        if (xStatus == pdPASS)
        {
            // Llamar a funcion para que cree el mensaje con el formato deseado
            String message = buildMessage(log);
            Serial.println(message);
            if (client.publish(log.topic, message.c_str()))
            {
                Serial.println("Mensaje publicado en el servidor MQTT");
            }
            else
            {
                Serial.println("Error al publicar el mensaje en el servidor MQTT");
            };
        }
        else
        {
            Serial.println("No se ha recibido mensaje...");
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
    const char *format = this->logFormat;
    std::stringstream ss;

    std::regex pattern("\\{(.*?)\\}"); // Expresión regular para encontrar los valores dentro de los corchetes

    std::string logFormatStr(format); // Convertir la cadena en un objeto de string

    std::smatch match; // Objeto para almacenar los resultados del split

    // Iterar sobre las coincidencias encontradas
    while (std::regex_search(logFormatStr, match, pattern))
    {
        // Imprimir el valor dentro de los corchetes
        std::cout << "Valor dentro de corchetes: " << match[1] << std::endl;

        // Acualizar el stringstream con el valor encontrado
        if (strcmp(match[1].str().c_str(), "level") == 0)
        {
            ss << "[" << log.level << "]";
        }
        else if (strcmp(match[1].str().c_str(), "message") == 0)
        {
            ss << "Message: " << log.message;
        }
        else if (strcmp(match[1].str().c_str(), "idSender") == 0)
        {
            ss << "ID: " << log.idSender;
        }
        else if (strcmp(match[1].str().c_str(), "timestamp") == 0)
        {
            ss << "Timestamp: " << log.timestamp.c_str();
        }

        ss << " - "; // Separador entre los valores
        // Actualizar la cadena para buscar la siguiente coincidencia
        logFormatStr = match.suffix().str();
    }

    // ss << "[" << log.level << "] " << log.message << ", ID: " << log.idSender << ", Timestamp: " << log.timestamp.c_str();
    return ss.str().c_str();
}
