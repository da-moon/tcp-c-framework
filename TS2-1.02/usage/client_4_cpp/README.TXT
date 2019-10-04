1. Client_4 - A simulated POS terminal client
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An example client program used for the purposes of testing the server framework.

The client functions are not really part of the framework, but are provided as
a convenience for testing the framework.

2. Configuration
~~~~~~~~~~~~~~~~

All configuration is done by changing the source code.

The program expects the server to be running at the IP address 127.1 (the local
host), using the port 12345.

Also, the client waits for about 5 seconds for the replies sent back by the
server, before accusing timeout.

The interval between messages is between 0 and 3 seconds, by default. The
size of the messages are between 0 ans 256 bytes by default.

It's also possible to enable tracing of the messages on the wire, and to
configure the amount of information written in the log file. These settings are
commented out in the source code.

3. Running
~~~~~~~~~~

For running this program it is necessary to also run the server Server_4.
Please refer to the README files for that program.

4. Running in Linux
-------------------

5. Running one instance
.......................

To run one instance of the program, open a terminal shell in the build
directory: .../TS2/usage/client_4_cpp/build/linux.

Run the script prepareEnv.sh with the shell dot command, to adjust the
LD_LIBRARY_PATH nvironment variable. Then run the program by issuing the
command ./Client_4.

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
output directory: ...\TS2\usage\client_4_cpp\build\windows\VS2008/debug.

Run the script prepareEnv.cmd to adjust the PATH enviroment variable. Then run
the program by issuing the command Client_4.

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


11. Application protocol and business rules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The application protocol and program logic are as follows:

1) When the terminal is switched on, it enters the state SWITCHED-ON and
   automatically sends a transaction with code "0000", telling the server its
   hardware serial number and software-configured terminal id.

   The server checks if both the serial number and the terminal id are valid.
   If so, the server then replies to the terminal.

   The terminal then enters the state RECOGNIZED.

   In the server, if the terminal is already in the terminal table (a table
   of the terminals that have already contacted the server) then the server
   does nothing more: the terminal was switched off and is being switched on
   again. Otherwise the server puts the terminal in the state RECOGNIZED.

2) Next, a supervisor logs in to the terminal and sends a transaction "0010"
   telling the server that this terminal is authorized to work with the
   server.

   The server checks the terminal identification and the supervisor
   credentials, and if all is well acknowledges the transaction "0010".

   The terminal and the server then enter the state AUTHORIZED.

3) Some time later, an operator log in to the terminal to begin her work shift.
   The terminal sends the transaction "0020" to the server with the
   operator credentials and the terminal identification.

4) The server checks the terminal identification and the operator
   credentials, and if all is well acknowledges the transaction "0020".

   The terminal and the server then enter the state OPERATING.

5) The terminal then enters a loop sending business transactions to the
   server (say, "produt sale" transactions, or "slip payment" transactions,
   etc), and receiving the server's authorization for committing these
   transactions. Let's say these operational transactions have code "0030".

6) Later on the operator's work shift ends. The operator then sends the
   transaction "0040".

   The terminal goes back to the state AUTHORIZED.

7) Another operator then may log in to the terminal to begin another work shift.
   The terminal sends the transaction "0020" to the server with the
   operator credentials and the terminal identification.

8) The server checks the terminal identification and the operator
   credentials, and if all is well acknowledges the transaction "0040".

   The terminal and the server then enter the state OPERATING.

9) Alternatively, when the terminal is in the AUTHORIZED state a
   supervisor can log in and send the transation "0050", preventing the
   terminal from being used again until the next day.

   Both the terminal and the server then enter the state CLOSED.

12. Terminal state and configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Terminal configuration is maintained in a per-terminal file called config.txt.

The first line constains the serial number of the terminal, 20 characters.

The second line constains the terminal identification, 3 chaarcters.

The third line constains the approximate time interval, in seconds, between
transactions sent by the terminal to the server.

The fourth line constains the probability that the terminal sends a business
instead of ending the current shift. This is a very rough estimation, the
rule of thumb is that the bigger this number, the longer the current shift
will be.

The fifth line constains the probability that the begins a new shift when it
is in the AUTHORIZED state, instead of closing down. This is a very rough
estimation, the rule of thumb is that the bigger this number, the longer it
will take for the terminal close down operations for the day.

There are some scripts to change the last 3 values for all terminals at once:
change_begin_shift_p, change_send_oper_p, and change_sleep_time.

The terminal state is mainained in a per-terminal file called state.txt.

The first line is the state the terminal is currently in:
o for SWITCHED_ON, 1 for RECOGNIZED, 2 for AUTHORIZED, 3 FOR OPERATING, and
4 for CLOSED.

The second line is the supervisor who authorized the terminal, if any.

The third line is the operator who is operating the terminal, if any.

The script reset_state resets the state of all terminals to 0 (SWITCHED_ON).
This is necessary because the server Server_4 doesn't maintain state between
successive runs.

oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO
