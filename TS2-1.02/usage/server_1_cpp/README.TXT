1. SERVER_1
~~~~~~~~~~~

A minimum server running 3 threads of the same code. The threads are in fact
unrelated to the framework, and don't do any communications at all.

Each thread simply displays a "Hello" message from time to time.

The test standalone, no client program is needed.

2. Configuration
~~~~~~~~~~~~~~~~

The number of threads is fixed in the source code, but it can be easily changed.
The same holds for the interval time between the messages.

3. Running
~~~~~~~~~~

4. Running in Linux
-------------------

Open a terminal shell in the build directory:
.../TS2/usage/server_1_cpp/build/linux.

Run the script prepareEnv.sh with the shell dot command, to adjust the
LD_LIBRARY_PATH nvironment variable.

Then run the program by issuing the command ./Server_1.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

5. Running in Windows
---------------------

Open a command prompt window in the build output directory:
...\TS2\usage\server_1_cpp\build\windows\VS2008/debug.

Run the script prepareEnv.cmd to adjust the PATH enviroment variable. Then run
the program by issuing the command Server_1.

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
