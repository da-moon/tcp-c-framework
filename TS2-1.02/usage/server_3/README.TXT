1. SERVER_3
~~~~~~~~~~~

A minimum TCP server running 2 types of threads.

The communications threads do a fake processing of the received messages and
send back the replies to the client applications.

The other threads from time to time write a message on the console.

The server is configurable from the command-line.

The threads also writes information to the log file.

(Aside from some bells and whistles, this program is the same as Server_2.)

Test with Client_1.

2. Configuration
~~~~~~~~~~~~~~~~

There are several configuration tha can be passed on the command line when
running the program.

The amount of detail that will be written in the log file:
   -loglevel N
where N is 1 to 5 (the higher the level, more information is written).

The maximum size of the messages accepted by the program:
   -msgsize: N

The port used by the server:
   -port: N

The number of "type 1" threads that will be started by the program.
   -threads1: N

The number of "type 2" threads that will be started by the program.
   -threads2: N

"Type 1" threads echo back to the clients the messages received by the
framework.

"Type 2" threads only display a message on the standard output  from time
to time.

("Type 1" and "type 2" threads are artifacts of this program, for the framework
all threads are the same).

3. Running
~~~~~~~~~~

4. Running in Linux
-------------------

Open a terminal shell in the build directory:
.../TS2/usage/server_3/build/linux.

Run the script prepareEnv.sh with the shell dot command, to adjust the
LD_LIBRARY_PATH nvironment variable.

Then run the program by issuing the command ./Server_3.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

5. Running in Windows
---------------------

Open a command prompt window in the build output directory:
...\TS2\usage\server_3\build\windows\VS2008/debug.

Run the script prepareEnv.cmd to adjust the PATH enviroment variable. Then run
the program by issuing the command Server_3.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

6. Log files
------------

Be aware that the log files can easily grow to huge sizes if you are using debug
mode or tracing the messages exchange between server and clients. Delete them as
soon as possible if you are concerned about disk space.

If you are a user of the vim editor there's a syntax file for the log file in
the package.

oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO
