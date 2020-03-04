# practica1

Primera práctica de la asignatura REDES II. Creación de un servidor web HTTP en el lenguaje C. 
Autores: Alejandro Bravo de la Serna y Miguel González González.

## Generación del ejecutable

El ejecutable consta de varios objetos y librerías cuyo código fuente se incluye en el repositorio. Para generar el ejecutable, basta con ejecutar el comando _make_ desde la raíz. Es necesario tener instalada la librería _confuse_ en el sistema, que corresponde al paquete _libconfuse-dev_. Se generará un ejecutable _server_ y los objetos y librerías se limpiarán posteriormente.

## Configuración del servidor

El fichero _server.conf_ permite al usuario configurar el servidor. Las configuraciones posibles son las siguientes:

1. *server-root* : La ruta, relativa a la ubicacion del ejecutable, donde se almacenarán los objetos de la web. El servidor nunca devolverá objetos que se encuentren por encima de esta ruta.
2. *max-clients* : Numero máximo de clientes que se pueden conectar simultáneamente en un momento dado. Si se conectan más, la conexión se rechazará o se mantendrá en espera hasta que finalicen las existentes.
3. *listen-port* : Número de puerto en el que escuchará el servidor. Debe ser un puerto que no se encuentre en uso. Por defecto, será el 8080.
4. *server-signature* : Nombre del servidor que se incluirá en las cabeceras de respuesta.
5. *daemon-mode* : Determina si el servidor se ejecuta en la terminal (0) o como demonio (1). En cualquier caso, la salida se imprimirá en el _syslog_ del sistema.
6. *listen-queue* : Numero de conexiones entrantes que se mantendrán en espera en caso de que se haya alcanzado el valor _max-clients_. El resto se rechazarán.
7. *read-timeout* : Si una conexión con un cliente que ha sido aceptada no responde durante el número de segundos que se indican aquí, se devolverá una respuesta _408 request timeout_.

## Ejecución del servidor

Para ejecutar el servidor, basta con incluir en la ruta indicada en _server-root_ los objetos de la web deseada, y correr el ejecutable generado. El fichero raíz debe llamarse _index.html_.

Para parar el servidor, se envía una señal SIGINT al proceso en ejecución. Esto puede hacerse presionando _Ctrl+C_ en la terminal, si no está en modo demonio, o bien usando _kill_. Se incluye un script _killServer.sh_ que manda una señal SIGINT al proceso _server_ que se esté ejecutando como demonio. (No obstante, se recomienda precaución por si ya existe otro proceso con ese nombre).
