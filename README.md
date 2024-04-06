# Remote Log Library

Librería que permite a los usuarios monitorizar y auditar en tiempo real las placas utilizadas en sus proyectos de forma remota. 

## 1. Instalación

Importar en platformio.ini el repositorio de la librería. (lib_deps = https://github.com/Rubens05/RemoteLogLibrary.git)

## 2. Funciones


#### 2.1 Función init
Función que se encarga de preparar el entorno para el funcionamiento correcto de la libreria. Esta función está sobrecargada por lo que se puede llamar con diferentes parámetros, siendo obligatorios cuatro parámetros:

| Parametros Obligatorios  |/|   Parametros Opcionales   | 
| ------------------------ |-| ------------------------ |
|         redWifi          |/|         mqttTopic        |
|         pwdWifi          |/|         idBoard          |        
|         mqttServer       |/|         timeZone         |
|         mqttPort         |/|         ntpServer        |

   

```
- void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort);
- void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic);
- void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard);
- void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone);
- void init(const char *ssid, const char *password, const char *mqttServer, const int mqttPort, const char *mqttTopic, const char *idBoard, const char *timeZone, const char *ntpServer);

```

#### 2.2 Función logX
Función que el usuario llama para enviar un mensaje de log.
Funciones logX disponibles:
```
- logINFO(const char *message);
- logWARNING(const char *message);
- logCRITCAL(const char *message);
- logERROR(const char *message);
- logDEBUG(const char *message);
```
### 2.3 Función para dar formato a los logs 
Función para que el usuario pueda elegir el formato de los logs que se envían. Para el correcto uso de esta función se debe utilizar una cadena que contenga entre llaves "{}", el parámetro deseado, siendo posible los siguientes valores de parametros:
- level
- message 
- timestamp
- idSender

Un ejemplo sería:

``` 
const char *logFormat = "{level}-{message}-{timestamp}-{idSender}";
```

El resultado del formato anterior sería el siguiente:
```
[ERROR] - Message: Mensaje de error! - Timestamp: April 04 2024 17:20:21 - ID: defaultIdBoard - 
```

Este ejemplo anterior es el `formato por defecto` de la librería, en caso de no especificar un formato este será el utilizado.

## Uso

Una vez entendidas las funcionalidades básicas de la librería se puede empezar a utilizar.

### Para utilizar la librería es esencial tener los siguientes elementos.
 
- Una red wifi (y la contraseña).
- Un broker mqtt (IP y Puerto).
- Una placa que conectaremos a la Red Wifi y enviará los logs al Broker mqtt.

### Primeros pasos

1. Crear un objeto Logger 

```c++
Logger loggerName;
```

2. Llamar a la función Logger.init deseada.

```c++
// Versión simple con parámetros obligatorios
logger.init(redWifi, pwdWifi, mqttServer, mqttPort);
```
3. ¡Listo!

Ahora solo tendrás que implementar tu lógica y decidir cuándo utilizar las funciones `logX`. Además, opcionalmente podrás cambiar el formato de los logs con la función `setLogFormat`.
## Ejemplos:

> random.cpp

Es un script con el que se puede llamar a la función init de diferentes formas y también es posible cambiar el formato de los logs. En un bucle infinto se llamarán a las funciones logX de forma aleatoria.  


