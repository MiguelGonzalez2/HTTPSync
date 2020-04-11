## Instrucciones de uso de los scripts

En este directorio se incluyen unos sencillos scripts de prueba para probar la funcionalidad CGI del servidor. Para probarlos:

1. Asignar el directorio scripts/ como directorio raiz del servidor en el fichero de configuracion.
2. Ejecutar el servidor.
3. Desde un navegador se pueden realizar las peticiones GET, por ejemplo, localhost:puerto/prueba1.py?user=foo
4. Desde curl se pueden realizar facilmente peticiones POST, por ejemplo: curl localhost:puerto/prueba1.py -X POST -d user=foo

Los scripts que se incluyen se han de ejecutar como sigue:

- Prueba1 (tanto .php como .py): Reciben un parametro user e imprimen "Hola user!". Por ejemplo, ante una ejecución con user=foo, imprimirán "Hola foo!".

- Prueba2 (tanto .php como .py): Reciben un parametro celsius con un valor de temperatura en celsius e imprimen su valor en farenheit. Por ejemplo, se puede ejecutar con celsius=37 e imprimirá el valor correspondiente, que es 98.6
