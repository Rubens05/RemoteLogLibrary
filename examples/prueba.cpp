#include <Arduino.h>
#include "LogLibrary.h"

const char *ssid = "wifiNetwork";
const char *password = "wifiPassword";
const char *mqttServer = "IP_BROKER";
const int mqttPort = 1883;
const char *mqttTopic = "mqttTopic";
const char *idBoard = "idBoard";
Logger logger;

void funcionalidadUsuario();

void setup()
{
    Serial.begin(9600);
    logger.init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard);
}

void loop()
{
    // Generar un número aleatorio entre 0 y 8
    int numeroAleatorio = random(0, 4);

    // FUNCIONALIDAD DEL USUARIO
    funcionalidadUsuario();

    switch (numeroAleatorio)
    {
    // EL USUARIO DECIDE CUANDO LLAMAR AL METODO DE LOG
    case 0:
        Serial.print("INFO");
        logger.setTopic("Topic 0");
        logger.setIdBoard("Yoda");
        logger.logINFO("Mensaje de info!");
        break;
    case 1:
        Serial.print("WARNING");
        logger.setTopic("Topic 1");
        logger.setIdBoard("Oviguan");
        logger.logWARNING("Mensaje de warning!");
        break;
    case 2:
        Serial.print("ERROR");
        logger.setTopic("Topic 2");
        logger.setIdBoard("Aphelios");

        logger.logERROR("Mensaje de error!");
        break;
    case 3:
        Serial.print("CRITICO");
        logger.setTopic("Topic 3");
        logger.setIdBoard("Kratos");

        logger.logCRITICO("Mensaje crítico!");
        break;
    default:
        break;
    }
}

void funcionalidadUsuario()
{
    // Funcion cualquiera de la placa del usuario
    // No se debe interrumpir su funcionamiento
    Serial.println(" ");
    delay(500);
}
