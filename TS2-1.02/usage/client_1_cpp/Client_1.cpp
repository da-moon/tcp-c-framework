/// @source       Client_1.cpp
/// @description  An echo client used for the purposes of testing the server
/// framework.

// -----------------------------------------------------------------------------

/** A minimum TCP echo client that generates messages with random contents,
    sends these messages to a server and then awaits the replies. @par

    The client functions are not really part of the framework, but are provided
    as a convenience for the testing of the framework. @par

    Test with Server_2 or Server_3
*/

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes
// ...

// C language includes
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h> // srand
#include <string.h> // memcpy
#include <time.h>

// framework includes
#include "Client.h" /* client_xxx functions */

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
cchar sourceID[] = "CCC";

// variables
static uint maxMsgSize;

// prototypes
void checkConnection();
uint createMessage(const Message*);
cchar* format(cchar*, va_list);
void printError(cchar*, ...);
void printInfo(cchar*, ...);
uint recvReply(Message*);
bool sendMessage(Message*);

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------

// the main program

int main()
{
   Message* msg;
   char*savOrgBuf;
   uint orgSize, replySize;

   // configures the client
   // ~~~~~~~~~~~~~~~~~~~~~

   // how long to wait for server replies
   client_setReplyTimeout(5);

   // the port on which ther server waits for connection requests
   client_setServicePort(12345);

   // the IP address on which the server is running
   client_setServerAddress("127.1");

   // how much detail is written to the log file
   // client_setLogLevel(LOG_LEVEL_DEBUG);

   // starts the client
   // ~~~~~~~~~~~~~~~~~

   // does whatever initialization it's needed
   client_init();

   // prepares to run the client
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~

   // requests a message buffer from the framework
   msg = client_createMessage();

   // retrieves the maximum message size supported by the framework
   maxMsgSize = client_maxMessageSize();

   // for convenience
   savOrgBuf = (char*)malloc(maxMsgSize);
   assert(savOrgBuf);

   // seeds the C library algorithm for generating pseudo-random numbers
   srand((uint)time(NULL)); // for createMessage

   // at last, runs the client
   // ~~~~~~~~~~~~~~~~~~~~~~~~

   for (;;)
   {
      // fills the message buffer with a random number of random characters
      orgSize = createMessage(msg);

      // saves the contents of the message just created
      client_copyFromMessage(savOrgBuf, msg, 0, orgSize);

      // ensures the connection with the server is alive
      checkConnection();

      // sends the message to the server
      if (!sendMessage(msg)) // error ?
         continue;

      // waits for the reply from the server
      if ((replySize = recvReply(msg)) == 0)
         continue; // there was an error

      // checks if the sizes match
      if (orgSize != replySize)
         printError("sizes differ: msgSize=%d replySize=%d", orgSize,
            replySize);
      else
         // checks if the reply matches the original message
         if (client_compareToMessage(savOrgBuf, msg, 0, orgSize))
            printError("reply differs from original!");
   }

   return 0;
}

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// ensures the connection with the server is alive

void checkConnection()
{
   uint cnt = 0;

   if (client_isConnected())
      return;

   client_createClientSocket();

   // client_setTrace(true); // do we want tracing ?

   printInfo("trying to connect to the server");

   while (!client_connect())
   {
      printError("connection failed, waiting 5 secs to try again (%d)", cnt++);
      client_sleep(5);
   }
}

// -----------------------------------------------------------------------------

// fills a message buffer with a random number of random characters

uint createMessage(const Message* msg)
{
   uint size;  // the size of the created message
   uint size1; // for convenience

   // waits some time between message creation (between 0 and 3 seconds)
   int secs = rand() % 4;
   if (secs)
   {
      printInfo("waiting %d seconds before creating the message", secs);
//    client_sleep(secs);
   }

   // calculates size of new message
   size = size1 = rand() % maxMsgSize;
   if (!size)
      size = size1 = 1;
   // size = 6; // forces size for testing purposes
   printInfo("generating %d bytes", size);

   // stamps the message buffer with the message size
   client_setMessageSize(msg, size);

   // generates the message bytes
   while (size1--)
   {
      // generates random characters between 0x20 e 0x7E (inclusive)
      // (only visible characteres and space)
      char ch = (char)(0x20 + (rand() % (0x7F - 0x20)));
      client_setMessageByte(msg, size1, ch);
   }

   // returns with the size of the created message
   return size;
}

// -----------------------------------------------------------------------------

// convenience printf-like function for formatting purposes

cchar* format(cchar* fmt, va_list ap)
{
   static char buf[256];

   buf[0] = '*';                    // (1 byte)  00
   buf[1] = ' ';                    // (1 byte)  01
   client_formatCurrentTime(buf+2); // (8 bytes) 02 03 04 05 06 07 08 09
   buf[10] = ' ';                   // (1 byte)  10

   vsnprintf(buf+11, sizeof(buf)-11, fmt, ap);

   return buf;
}

// -----------------------------------------------------------------------------

// printf-like function that writes on stdout and the log file

void printError(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   puts(buf);
   fflush(stdout);

   client_logError(buf+11);
}

// -----------------------------------------------------------------------------

// printf-like function that writes on stdout and the log file

void printInfo(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   puts(buf);
   fflush(stdout);

   client_logInfo(buf+11);
}

// -----------------------------------------------------------------------------

// receives the reply from the server

uint recvReply(Message* msg)
{
   printInfo("receiving reply from the server");

   if (client_recvMessage(msg)) // true is good
   {
      printInfo("received reply with orgId:%d and orgSeq:%d",
         client_messageOrgId(msg), client_messageOrgSeqNo(msg));
      return message_size(msg);
   }

   // if timeout occurs, connection is not automatically closed, so closes it
   if (client_timeout()) // timeout ?
   {
      printError("timeout receiving reply from the server, closing connection");
      client_closeConnection();
      return 0;
   }

   // from here on, the connection is sure to have been closed

   if (client_connectionClosed()) // connection closed by server ?
   {
      printError("connection closed when receiving reply from the server");
      return 0;
   }

   if (client_formatError()) // format error on the wire ?
   {
      printError("invalid format in reply received from the server");
      return 0;
   }

   // other errors
   printError("error %d while receiving reply from the server", client_error());

   return 0;
}

// -----------------------------------------------------------------------------

// sends a message to the server (the size is embedded in the message buffer)

bool sendMessage(Message* msg)
{
   printInfo("sending message to the server");

   if (client_sendMessage(msg)) // true is good
   {
      printInfo("ok, the message was sent");
      return true;
   }

   if (client_timeout()) // probably server application is clogged, or stuck,
   {                     // or crashed, or whatever
      printError("timeout when sending a message to the server");
      client_closeConnection();
      return false;
   }

   // other errors, connection is automatically closed
   printError("error %d when sending a message to the server", client_error());

   return false;
}

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
// the end
