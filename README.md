# Remote Log Library (ESP)

Librería que permite a los usuarios monitorizar y auditar en tiempo real las placas utilizadas en sus proyectos de forma remota. Esta librería está acompañada de una aplicación web para poder interactuar de forma gráfica con los datos almacenados en la base de datos (en este caso MongoDB).

Se puede utilizar la librería independientemente del la aplicación web, esto es gracias a que la librería tiene en cuenta el `modo serial monitor`, que es el modo por defecto (solo se imprimen los logs por el monitor serial), y el `modo persistencia` (se imprimen los logs por el monitor serial y además se insertan en MongoDB)


## 1. Instalación

Importar en platformio.ini el repositorio de la librería. (lib_deps = https://github.com/Rubens05/RemoteLogLibrary.git)

## 2. Funciones

### 2.1 Función sendToMQTT
Esta funcion es la que permite enviar los logs a la base de datos, es decir usaría la libreria en `modo persistencia`. Para utilizarla correctamente hay que pasar por parámetro el usuario y contraseña del broker MQTT para poder enviar los logs a este y que los redireccione a la base de datos. El usuario y contraseña son necesarios ya que se requiere de autorización y autenticación para poder publicar en el broker MQTT. De esta forma podrá redirigir los datos a MongoDB y así evitar posibles ataques maliciosos.

```` c++
// Send logs to MQTT in order to be stored in a database
logger.sendToMQTT(mqttUser, mqttPassword);
````


#### 2.2 Función init
Función que se encarga de preparar el entorno para el funcionamiento correcto de la libreria. Esta función debe ser llamada posteriormente a la función sendToMQTT. Ademáws está sobrecargada para que se puede llamar con diferentes parámetros, siendo obligatorios siempre cuatro parámetros:

| Parametros Obligatorios  |/|   Parametros Opcionales  | 
| ------------------------ |-| ------------------------ |
|         redWifi          |/|         mqttTopic        |
|         pwdWifi          |/|         idBoard          |        
|         mqttServer       |/|         timeZone         |
|         mqttPort         |/|         ntpServer        |


Todos los parámetros opcionales tienen un valor `default` y la librería funcionará correctamente aunque no se les pase un valor.


```` javascript
//Serial monitor mode
void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort);

//Database mode
void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char mqttUser, const char mqttPassword);

void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char mqttUser, const char mqttPassword, const char *mqttTopic);

void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char mqttUser, const char mqttPassword, const char *mqttTopic, const char *idBoard);

void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char mqttUser, const char mqttPassword, const char *mqttTopic, const char *idBoard, const char *timeZone);

void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char mqttUser, const char mqttPassword, const char *mqttTopic, const char *idBoard, const char *timeZone, const char *ntpServer);
````


### 2.3 Función setLogFormat
Función para que el usuario pueda elegir el formato de los logs que se envían. Para el correcto uso de esta función se debe utilizar una cadena que contenga entre llaves "{}", el parámetro deseado, siendo posible los siguientes valores de parámetros:
- level
- message 
- idSender
- topic
- timestamp

Un ejemplo sería:

``` 
const char *logFormat = "{level}-{message}-{idSender}-{topic}-{timestamp}";
```

El resultado del formato anterior sería el siguiente:
```
[DEBUG] - Message: Debug en la placa del usuario - ID: defaultIdBoard - Topic: Authorization Topic - Timestamp: April 22 2024 20:27:23 -
```

Este ejemplo anterior es el `formato de log por defecto` de la librería, en caso de no especificar un formato, este será el utilizado.

#### 2.4 Función logX
Función que el usuario llama para enviar/imprimir un mensaje de log.
Funciones logX disponibles:

``` c++
logINFO(const char *message);
logWARNING(const char *message);
logCRITCAL(const char *message);
logERROR(const char *message);
logDEBUG(const char *message);
```

## 3. Uso

Una vez entendidas las funcionalidades básicas de la librería se puede empezar a utilizar.

### 3.1 Para utilizar la librería es esencial tener los siguientes elementos.
 
- Una red wifi (y la contraseña).
- Un broker mqtt (IP y Puerto).
- Una placa que conectaremos a la Red Wifi y enviará los logs al Broker mqtt.

### 3.2 Primeros pasos

1. Crear un objeto Logger 

    ```c++
    // Database mode
    Logger loggerName;
    ```

2. Utilizar la función `sendToMQTT` para guardar los logs en la base de datos MongoDB `(modo persistencia)` que se levanta con el dockercompose de la Interfaz de Usuario de la librería: 
https://github.com/Rubens05/RemoteLogLibraryUI.git.

    ```` c++
    // Send logs to MQTT in order to be stored in a database
    logger.sendToMQTT(mqttUser, mqttPassword);
    ````

2. Llamar a la función Logger.init deseada.

    ```c++
    // Simplest mode
    logger.init(redWifi, pwdWifi, mqttServer, mqttPort);
    ```

    ```c++
    // all params function
    logger.init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone, const char *ntpServer);
    ```

3. ¡Listo!

Ahora solo tendrás que implementar tu lógica y decidir cuándo utilizar las funciones `logX`. Además, opcionalmente podrás cambiar el formato de los logs con la función `setLogFormat`.
## Ejemplos:

> random.cpp

Es un script con el que se puede llamar a la función init de diferentes formas y también es posible cambiar el formato de los logs. En un bucle infinto se llamarán a las funciones logX de forma aleatoria.  


