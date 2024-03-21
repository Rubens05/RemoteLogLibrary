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
    // Generar un número aleatorio entre 0 y 20
    int numeroAleatorio = random(0, 21);

    // FUNCIONALIDAD DEL USUARIO
    funcionalidadUsuario();

    if (numeroAleatorio >= 0 && numeroAleatorio <= 5)
    {
        logger.logINFO("Error en la placa del usuario");
    }
    else if (numeroAleatorio >= 6 && numeroAleatorio <= 10)
    {
        logger.logWARNING("Advertencia en la placa del usuario");
    }
    else if (numeroAleatorio >= 11 && numeroAleatorio <= 15)
    {
        logger.logERROR("Error en la placa del usuario");
    }
    else if (numeroAleatorio >= 16 && numeroAleatorio <= 20)
    {
        logger.logINFO("Placa del usuario funcionando correctamente");
    }
}

void funcionalidadUsuario()
{
    // Funcion cualquiera de la placa del usuario
    // No se debe interrumpir su funcionamiento
    Serial.println("Funcionalidad del usuario");
    delay(500);
}
