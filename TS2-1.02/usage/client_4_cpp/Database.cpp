/// @source       Database.cpp
/// @description  Simulated database access.

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes
// ...

// C language includes
#include <stdlib.h> // abort
#include <stdio.h>
#include <string.h>

// application includes
#include "Client.h"     /* client_xxx functions */
#include "Client_4.h"
#include "Database.h"

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
const char sourceID[] = "DBA";

// ----------------------------------

// configuration

// hardware serial number of this terminal
char serialNumber[SER_NUM_LEN];

// software serial number of this terminal
char terminalID[TERM_ID_LEN];

// max sleep time between transactions, in seconds
ushort maxSleepTime = 4;

// probability of sending another operation, against ending the shift
ushort pSendOper = 100; // (100/100=1 1% for ending the shift)

// probability of beginning another shift, against closing the terminal
ushort pBeginShift = 100; // (100/100=1 1% for closing the terminal)

// ----------------------------------

// state

// SWITCHED_ON, RECOGNIZED, AUTHORIZED, OPERATING, CLOSED
uchar state; // SWITCHED_ON, RECOGNIZED, AUTHORIZED, OPERATING, CLOSED

// supervisor who authorized this terminal
char supervisor[EMPLOYEE_LEN];

// operator who is manning, or last manned, this terminal
char termOperator[EMPLOYEE_LEN];

// ----------------------------------

// prototypes
void loadConfig();
void loadField(FILE*, cchar*, ushort, char*, cchar*, bool);
void restoreState();
void saveState();

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    initializes the db simulator.
*/

void db_init()
{
   loadConfig();
   restoreState();
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves max sleeping time between transactions
*/

ushort db_maxSleepTime()
{
   return maxSleepTime;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the current operator
*/

cchar* db_operator()
{
   return termOperator;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the probability of sending the next operation, against ending
    the shift
*/

ushort db_pSendOper()
{
   return pSendOper;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the probability of beginning the next shift, against closing the
    terminal
*/

ushort db_pBeginShift()
{
   return pBeginShift;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the serial number of the terminal
*/

cchar* db_serialNumber()
{
   return serialNumber;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the state of the terminal
*/

uchar db_state()
{
   return state;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the current supervisor
*/

cchar* db_supervisor()
{
   return supervisor;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the terminal id
*/

cchar* db_terminalID()
{
   return terminalID;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    updates the state of the terminal
*/

void db_updateState(uchar _state)
{
   printInfo("changing state from '%c' to '%c'", state, _state);
   state = _state;
   saveState();
}

// -----------------------------------------------------------------------------
// HELPER FUNCTIONS
// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

/** Loads the configuration of the terminal by reading a file
*/

void loadConfig()
{
   char strMaxSleepTime[10];
   char strSendOper[10];
   char strBeginShift[10];

   static cchar fileName[] = "config.txt";
   FILE* fp = fopen(fileName, "rb");
   if (!fp)
   {
      printFatal("missing file %s", fileName);
      abort();
   }

   loadField(fp, "serial number", SER_NUM_LEN, serialNumber, fileName, false);
   loadField(fp, "terminalID", TERM_ID_LEN, terminalID, fileName, false);

   loadField(fp, "max sleep time", 10, strMaxSleepTime, fileName, true); // opt
   if (!strMaxSleepTime[0])
      goto END;
   maxSleepTime = atoi(strMaxSleepTime);

   loadField(fp, "pSendOper", 10, strSendOper, fileName, true);      // opt
   if (!strSendOper[0])
      goto END;
   pSendOper = atoi(strSendOper);

   loadField(fp, "pBeginShift", 10, strBeginShift, fileName, true);  // opt
   if (!strBeginShift[0])
      goto END;
   pBeginShift = atoi(strBeginShift);

END:
   printInfo("serial number: %.*s", SER_NUM_LEN, serialNumber);
   printInfo("terminal id: %.*s", TERM_ID_LEN, terminalID);
   printInfo("max sleep time: %d", maxSleepTime);
   printInfo("pSendOper: %d", pSendOper);
   printInfo("pBeginShift: %d", pBeginShift);

   fclose(fp);
}

// -----------------------------------------------------------------------------

/** Load a field from a file
*/

void loadField(FILE* fp, cchar* fieldName, ushort fieldLen, char* dest,
   cchar* fileName, bool opt)
{
   int n;
   char buf[80];
   char field[80];

   memset(dest, 0, fieldLen);

   if (!fgets(buf, 80, fp))
   {
      if (opt)
         return;
      printFatal("could not read %s in %s", fieldName, fileName);
      abort();
   }

   n = sscanf(buf,"%s", field);
   if (n != 1)
   {
      printFatal("missing %s in %s", fieldName, fileName);
      abort();
   }

   if (!opt && (strlen(field) != fieldLen))
   {
      printFatal("invalid %s in %s", fieldName, fileName);
      abort();
   }
   else
      fieldLen = strlen(field);

   memcpy(dest, field, fieldLen);
}

// -----------------------------------------------------------------------------

/** Restores the terminal state by reading a file
*/

void restoreState()
{
   static cchar fileName[] = "state.txt";
   FILE* fp = fopen(fileName, "rb");
   if (!fp)
   {
      printFatal("missing file %s", fileName);
      abort();
   }

   loadField(fp, "terminal state", 1, (char*)&state, fileName, false);
   loadField(fp, "operator", EMPLOYEE_LEN, termOperator, fileName, false);
   loadField(fp, "supervisor", EMPLOYEE_LEN, supervisor, fileName, false);

   fclose(fp);
}

// -----------------------------------------------------------------------------

/** Saves the terminal state by writing a file
*/

void saveState()
{
   int stateLen =
      1              // state
    + 1              // \n
    + EMPLOYEE_LEN   // operator
    + 1              // \n
    + EMPLOYEE_LEN   // supervisor
    + 1;

   int n;
   FILE* fp = fopen("state.txt", "wb");
   if (!fp)
   {
      printFatal("could not create file state.txt");
      abort();
   }

   n = fprintf(fp,"%c\n%.*s\n%.*s\n", state, EMPLOYEE_LEN, termOperator,
      EMPLOYEE_LEN, supervisor);

   if (n != stateLen)
   {
      printFatal("could not write state.txt");
   }

   fclose(fp);
}

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
// the end
