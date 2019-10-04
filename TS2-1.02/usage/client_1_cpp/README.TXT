1. CLIENT_1
~~~~~~~~~~~

An echo client program used for the purposes of testing the server framework.

A minimum TCP echo client that generates messages with random contents, sends
these messages to a server and then awaits the replies.

The client functions are not really part of the framework, but are provided as
a convenience for testing the framework.

2. Configuration
~~~~~~~~~~~~~~~~

The program expects the server to be running at the IP address 127.1 (the local
host), using the port 12345. These values are not configurable, but are easy to
change in the source code.

Also, the client waits for about 5 seconds for the replies sent back by the
server, before accusing timeout. This timeout is easily configurable in the
source code.

The interval between messages is between 0 and 3 seconds, by default. The
size of the messages are between 0 ans 256 bytes by default.

It's also possible to enable tracing of the messages on the wire, and to
configure the amount of information written in the log file. These settings are
commented out in the source code.

The reason for having the configuration settings only in the source code was to
keep the source program small enough to be understood even by a cursory reading.

3. Running
~~~~~~~~~~

For running this program it is necessary to also run the echo server, either
Server_2 or Server_3. Please refer to the README files of those programs.

4. Running in Linux
-------------------

5. Running one instance
.......................

To run one instance of the program, open a terminal shell in the build
directory: .../TS2/usage/client_1_cpp/build/linux.

Run the script prepareEnv.sh with the shell dot command, to adjust the
LD_LIBRARY_PATH nvironment variable. Then run the program by issuing the
command ./Client_1.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

6. Running several instances
............................

Open a terminal shell in the build directory, and run the prepareEnv.sh script.
Issue the command ./run_clients.sh N, where N is the number of instances. Each
instance will run in its own directory: ./clients/1 ./clients/2 etc.

To stop the instances, run the script ./kill_clients.sh.

As before, each instance will write to a log file in its directory, and display
messages will be redirected to files 1.txt 2.txt etc.

To see the instances running, use the script ./see_clients.sh.

To clean up the log and display files use the script ./reset_all.sh.

7. Running in Windows
---------------------

8. Running one instance
.......................

To run one instance of the program, open a command prompt window in the build
output directory: ...\TS2\usage\client_1_cpp\build\windows\VS2008/debug.

Run the script prepareEnv.cmd to adjust the PATH enviroment variable. Then run
the program by issuing the command Client_1.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

9. Running several instances
............................

Open a command prompt window in the build output directory, and run the
script prepareEnv.cmd. Issue the command run_clients.cmd N, where N is the
number of instances. Each instance will run in its own directory: clients\1
clients\2 etc. Also be aware that each instance will run in its own window!
So be advised that you will have lots of command windows open.

To stop the instances, run the script kill_clients.cmd.

As before, each instance will write to a log file in its directory, and display
messages will be redirected to files 1.txt 2.txt etc.

To see the instances running, use the script see_clients.cmd.

To clean up the log and display files use the script ./reset_all.cmd.

10. Log files
~~~~~~~~~~~~~

Please be aware that the log file (*.tse) can easily grow to huge sizes if you
use debug mode, or enable tracing of the messages exchanged between server and
clients. If you are concerned with disk space you must delete them as soon as
possible.

If you are a user of the vim editor there's a syntax file for the log file in
the package.

oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO
