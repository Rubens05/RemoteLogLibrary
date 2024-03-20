#include "LogLibrary.h"

Logger::Logger()
{
    // Constructor
}

Logger::~Logger()
{
    // Destructor
}

void Logger::init(const char *ssid, const char *password, const char *mqttServer, int mqttPort, const char *mqttTopic, const char *idBoard)
{
    this->idBoard = idBoard;
    this->mqttServer = mqttServer;
    this->mqttPort = mqttPort;
    this->mqttTopic = mqttTopic;
    this->espClient = setupWifi(ssid, password);
    //  Lanzar la tarea paralela publishMqtt
    xTaskCreate(
        [](void *pvParameters)
        {
            Logger *logger = (Logger *)pvParameters;
            logger->publishMqtt();
            vTaskDelete(NULL);
        },
        "publishMqtt",
        10000,
        this,
        1,
        NULL);
}

WiFiClient Logger::setupWifi(const char *ssid, const char *password) // FUNCIONA
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

// PubSubClient Logger::setupMqtt(const char *mqttServer, const int mqttPort, WiFiClient espClient)
// {
//     PubSubClient client(espClient);
//     client.setServer(mqttServer, mqttPort);
//     while (!client.connected())
//     {
//         Serial.println("Connecting to MQTT...");

//         if (client.connect("ESP32Client"))
//         {
//             Serial.println("Connected to MQTT");
//         }
//         else
//         {
//             Serial.print("Failed with state ");
//             Serial.print(client.state());
//             delay(2000);
//         }
//     }

//     return client;
// }

void Logger::setTopic(const char *mqttTopic) // FUNCIONA
{
    this->mqttTopic = mqttTopic;
}

void Logger::setMqttServer(const char *mqttServer) // FUNCIONA
{
    this->mqttServer = mqttServer;
}

void Logger::setMqttPort(const int mqttPort) // FUNCIONA
{
    this->mqttPort = mqttPort;
}

void Logger::setIdBoard(const char *idBoard) // FUNCIONA
{
    this->idBoard = idBoard;
}

void Logger::logINFO(const char *message) // FUNCIONA, HACE BIEN EL PUSH
{
    logMessage log;
    log.level = "INFO";
    log.message = message;
    log.idSender = idBoard;
    log.topic = mqttTopic;
    log.timestamp = "2021-09-01 12:00:00";

    queue.push(log);
}

void Logger::logWARNING(const char *message) // FUNCIONA, HACE BIEN EL PUSH
{
    logMessage log;
    log.level = "WARNING";
    log.message = message;
    log.idSender = idBoard;
    log.topic = mqttTopic;
    log.timestamp = "2021-09-01 12:00:00";

    queue.push(log);
}

void Logger::logERROR(const char *message) // FUNCIONA, HACE BIEN EL PUSH
{
    logMessage log;
    log.level = "ERROR";
    log.message = message;
    log.idSender = idBoard;
    log.topic = mqttTopic;
    log.timestamp = "2021-09-01 12:00:00";

    queue.push(log);
}

void Logger::logCRITICO(const char *message) // FUNCIONA, HACE BIEN EL PUSH
{
    /////////////
    // char fecha[25]; // ctime devuelve 26 caracteres pero tambien se podría usar un puntero de char
    // time_t current_time;
    // current_time = time(NULL);
    // ctime(&current_time);
    // strcpy(fecha, ctime(&current_time));
    // printf("%s", fecha);
    ////////////////
    // time_t now;
    // Serial.println("Obteniendo la fecha y hora actual...");
    // time(&now);
    // char buffer[80];
    // strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now));
    // std::string timestamp(buffer);
    ////////////////
    // const char *date = __DATE__;
    // const char *time = __TIME__;

    logMessage log;
    log.level = "CRITICO";
    log.message = message;
    log.idSender = idBoard;
    log.topic = mqttTopic;
    log.timestamp = "2021-09-01 12:00:00";

    queue.push(log);
}

void Logger::publishMqtt() // LEE DE LA COLA, PERO NO ESTÁ EN SEGUNDO PLANO
{
    // NO SE PIERDE NINGUN MENSAJE
    PubSubClient client(this->espClient);               // Crear cliente MQTT a partir del cliente WiFi
    client.setServer(this->mqttServer, this->mqttPort); // Setear servidor MQTT
    Serial.println("Tarea paralela publishMqtt lanzada...");
    while (true)
    {
        if (!queue.empty())
        {
            // Si la cola tiene datos, conectar al servidor MQTT
            Serial.println("Conectando al servidor MQTT...");
            client.connect("ESP32Client");

            // Si se ha conectado al servidor MQTT publicar los mensajes de la cola
            if (client.connected())
            {
                Serial.println("Conectado al servidor MQTT");
                while (!queue.empty())
                {
                    logMessage log = queue.front();
                    std::string message =
                        "[" + std::string(log.level) + "] "    /**/
                        + std::string(log.message)             /**/
                        + ", ID: " + std::string(log.idSender) /**/
                        + ", Timestamp: " + log.timestamp;     /**/
                    client.publish(log.topic, message.c_str());
                    queue.pop();
                    delay(100);
                }
                // Desconectar del servidor MQTT
                client.disconnect();
            }
            else // Si no se ha podido conectar al servidor MQTT
            {
                Serial.println("No se ha podido conectar al servidor MQTT");
                delay(2000);

                ////// Opcional vaciar la cola para que no colapse
                while (!queue.empty())
                {
                    queue.pop();
                }
                ///////
            }
        }
        else
        {
            delay(5000); // Si la cola esta vacía esperar x tiempo
        }
    }
}
