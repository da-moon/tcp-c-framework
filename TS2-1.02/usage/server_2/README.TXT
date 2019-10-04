1. SERVER_2
~~~~~~~~~~~

A minimum TCP server running 5 threads of the same code. The threads simply echo
back the received messages.

Test with Client_1.

2. Configuration
~~~~~~~~~~~~~~~~

The number of threads is fixed in the source code, but it can be easily changed.
The same holds for the IP port (12345) used by the server.

Also, the debug mode of the log file and the tracing option are disabled in the
source code, but can be easily enabled.

3. Running
~~~~~~~~~~

4. Running in Linux
-------------------

Open a terminal shell in the build directory:
.../TS2/usage/server_2/build/linux.

Run the script prepareEnv.sh with the shell dot command, to adjust the
LD_LIBRARY_PATH nvironment variable.

Then run the program by issuing the command ./Server_2.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

5. Running in Windows
---------------------

Open a command prompt window in the build output directory:
...\TS2\usage\server_2\build\windows\VS2008/debug.

Run the script prepareEnv.cmd to adjust the PATH enviroment variable. Then run
the program by issuing the command Server_2.

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
