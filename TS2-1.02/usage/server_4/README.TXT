1. Server_4 - A simulated server for a POS terminal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A small TCP server that simulates the operation of a POS terminal.

2. Configuration
~~~~~~~~~~~~~~~~

There are two settings that can be configured by changing the source code: the
TCP service port, and the number of worker threads. Also, the tracing of
received and sent messages are commented out in the code.

In the working directory there are some files that simulates the programs's
database. These are simple text files that can be edited manually.

serial_number.txt - hardware serial numbers of the allowed terminals
terminal_id.txt   - software identification numbers of the allowed terminals
supervisot.tx     - employee number of the allowed supervisors
operator.tx       - employee number of the allowed operators

3. Running
~~~~~~~~~~

4. Running in Linux
-------------------

Open a terminal shell in the build directory:
.../TS2/usage/server_4/build/linux.

Run the script prepareEnv.sh with the shell dot command, to adjust the
LD_LIBRARY_PATH nvironment variable.

Then run the program by issuing the command ./Server_4.

Stop the program using Ctrl-C.

The programs displays messages on the standard output, and writes to a log
file named YYMMDD_HHMMSS_PPP.tse (ppp is program's pid).

5. Running in Windows
---------------------

Open a command prompt window in the build output directory:
...\TS2\usage\server_4\build\windows\VS2008/debug.

Run the script prepareEnv.cmd to adjust the PATH enviroment variable. Then run
the program by issuing the command Server_4.

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

7. Application protocol and business rules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Please see the README file for the Client_4 client application.

oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO



























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

7) The server checks the terminal identification and the operator
   credentials, and if all is well acknowledges the transaction "0040".

   The server then goes back to the state AUTHORIZED.

8) Alternatively, when the terminal is in the AUTHORIZED state a
   supervisor can log in and send the transation "0050", preventing the
   terminal from being used again until the next day.
   Both the terminal and the server then enter the state CLOSED.
