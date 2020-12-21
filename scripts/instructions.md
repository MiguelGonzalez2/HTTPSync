## Script usage instructions

In this directory, several CGI testing scripts are included. They can be tested as follows:

1. Assign the scripts/ directory as server root in the config file.
2. Run the server.
3. With a browser, GET requests can be issued, for example: localhost:port/prueba1.py?user=foo
4. With curl, POST requests can be easily issued: curl localhost:port/prueba1.py -X POST -d user=foo

This is the specification for each script:

- Prueba1 (tanto .php como .py): Receives a _user_ parameter and outputs "Hola user!". For example, if user=foo, it will output "Hola foo!".

- Prueba2 (tanto .php como .py): Receives a _celsius_ parameter with a temperature and prints it's value in Fahrenheit degrees. For example, if celsius=37, it will output 98.6
