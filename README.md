# Remote Log Library

Librería que permite a los usuarios monitorizar y auditar en tiempo real las placas utilizadas en sus proyectos de forma remota. 

## Instalación

1. Importar en platformio.ini el repositorio de la librería. (lib_deps = https://github.com/Rubens05/RemoteLogLibrary.git)

## Funciones

### Función init
Función que se encarga de preparar el contexto para el funcionamiento correcto de la libreria. Recibe los parámetros: redWifi, pwdWifi, mqttServer, mqttPort, mqttTopic e idBoard.
A esta función esta sobrecargada por lo que se puede llamar con diferentes parametros, siendo obligatorios los parametros redWifi, pwdWifi, mqttServer y mqttPort.

### Función logX
Función que el usuario llama para enviar un mensaje al broker.
Esta función 

### Función para formatear logs //TODO
Función para que el usuario pueda elegir el formato de los logs que se envían.


## Uso


### Para utilizar la librería es esencial tener los siguientes elementos.
 
- Una red wifi (y la contraseña) para conectar la placa.
- Un broker mqtt (IP y Puerto) donde enviar los logs.
- Un Topic al que publicar.
- Una placa que haga sus tareas.

### Primeros pasos

1. Crear un objeto Logger 

```c++
    Logger loggerName;
```

2. Llamar a la función Logger.init, con los parametros de red (wifi y contraseña), de broker (servidor y puerto), un topic, y un id de placa.

```c++
    logger.init(redWifi, pwdWifi, mqttServer, mqttPort, mqttTopic, idBoard);
```
3. ¡Listo!

Ahora solo tendrás que implementar tu lógica y decidir cuando enviar los mensajes de logs. 
## Ejemplos:

> random.cpp

```c++
    const char *ssid = "wifiNetwork";
    const char *password = "wifiPassword";
    const char *mqttServer = "IP_BROKER";
    const int mqttPort = 1883;
    const char *mqttTopic = "topicUsuario";
    const char *idBoard = "placaUsuario";
    Logger logger;

    void funcionalidadUsuario();

    void setup()
    {
        Serial.begin(9600);
        logger.init(ssid, password, mqttServer, mqttPort, mqttTopic, idBoard);
    }


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
```
El resultado del codigo anterior sería:
```
topicUsuario [INFO] Placa del usuario funcionando correctamente, ID: PlacaUsuario, Timestamp: 2021-09-01 12:00:00
topicUsuario [INFO] Error en la placa del usuario, ID: placaUsuario, Timestamp: 2021-09-01 12:00:00
topicUsuario [WARNING] Advertencia en la placa del usuario, ID: placaUsuario, Timestamp: 2021-09-01 12:00:00
topicUsuario [ERROR] Error en la placa del usuario, ID: placaUsuario, Timestamp: 2021-09-01 12:00:00
topicUsuario [ERROR] Error en la placa del usuario, ID: placaUsuario, Timestamp: 2021-09-01 12:00:00
topicUsuario [INFO] Error en la placa del usuario, ID: placaUsuario, Timestamp: 2021-09-01 12:00:00
```





