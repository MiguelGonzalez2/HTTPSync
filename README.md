# HTTPSync - A simple, fast web server in C.

This project is a simple webserver written in C that supports simple HTTP features and CGI. It was developed as part of the Communication Networks II subject at college.
Authors: [Alejandro Bravo de la Serna](https://github.com/zombor00) and [Miguel González González](https://github.com/MiguelGonzalez2).

## Building the program

To build the server's executable, just run _make_ from the project's root. Library _confuse_, which corresponds to package _libconfuse-dev_, needs to be installed. This will result in the file _server_.

## Configuration

The _server.conf_ file allows to tweak certain parameters:

1. *server-root* : Path, relative to the program's file, that will store the web contents. Files above this path will never be returned by the server.
2. *max-clients* : Maximum number of clients that can connect to the server in a given moment. More connections will result in waits or refusings, as specified in _listen-queue_
3. *listen-port* : Must be an unusued port in which the server will start listening.
4. *server-signature* : Server name that will be included in response header files.
5. *daemon-mode* : Determines whether the server shall run "as is" (0) or should launch itself as a daemon (1). In any case, server output will be produced in the system log (_syslog_).
6. *listen-queue* : Number of incoming connections that will be put on hold, in case _max-clients_ has been attained, before starting to refuse.
7. *read-timeout* : Number of seconds that will be waited before sending a _408 request timeout_ on nonresponding clients.

## Running the server

It suffices to include the website objects in the _server-root_ path and run the server. The website root shall be named _index.html_.

In order to properly stop the server, it must be signaled with a SIGINT. If running on an active terminal, this can be achieved by pressing _Ctrl+C_. Otherwise, use the _kill_ command (_kill -INT <pid>_). A shell script, _killServer.sh_, is included to automatically send the signal to the server. (CAUTION. Be aware that if other processes have the same name as the server one, they might be signaled as well. Script was included for debugging convenience, and shall never be used to stop the server in serious environments).

## Features and future improvements

Kindly refer to the [project's wiki](https://github.com/MiguelGonzalez2/HTTPSync/wiki/) to find out about what is supported, some details about the design, and some possible future roadmap.
