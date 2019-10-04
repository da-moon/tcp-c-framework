/// @source       Database.cpp
/// @description  Simulated database access.

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes

// C language includes
#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // abort
#include <string.h>

#if PLATFORM(Windows)
#define strdup _strdup
#pragma warning(disable: 4800) // conversion (void*) --> bool
#endif

// framework includes
#include "Server.h"     /* server_xxx functions */
#include "util/List.h"  /* list_xxx functions   */

// application includes
#include "Server_4.h"   /* printXxx functions   */
#include "Database.h"   /* db_xxx functions     */

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
const char sourceID[] = "DBA";

mutex_t* dbMutex;

// the terminal structure
typedef struct _Terminal
{
   uchar state;
   char terminalID[TERM_ID_LEN+1];
   char serialNumber[SER_NUM_LEN+1];
   char supervisor[EMPLOYEE_LEN+1];
   char termOperator[EMPLOYEE_LEN+1];
   uint ipAddr;
   ushort connectionSeqNo;
} Terminal;

// the table of terminals
Terminal terminals[N_MAX_TERMS];

// helper functions
void operatorTable_init();
void serialNumberTable_init();
void supervisorTable_init();
void terminalId_init();

bool checkEmployee(void*, void*);
bool checkSerialNumber(void*, void*);
bool checkTerminalID(void*, void*);

void loadTable(cchar*, ushort, List*);

// -----------------------------------------------------------------------------

static List* operatorTable;
static List* serialNumberTable;
static List* supervisorTable;
static List* terminalIdTable;

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
   checks if connection information is consistent with the terminal id
*/

bool db_checkTerminalConnectionID(short termIndex, uint ipAddr,
   ushort connectionSeqNo)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);

   if (terminals[termIndex].ipAddr != ipAddr)
      return false;

   if (terminals[termIndex].connectionSeqNo != connectionSeqNo)
      return false;

   return true;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    checks if a operator is using a terminal

    @pre
    the terminal must be checked to be in the TRM_OPERATING state.
*/

bool db_checkTerminalOperator(short termIndex, cchar* termOperator)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   return
      (!(memcmp(terminals[termIndex].termOperator, termOperator,
         EMPLOYEE_LEN)));
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    checks the serial number of a terminal.
*/

bool db_checkTerminalSerialNumber(short termIndex, cchar* serialNumber)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   return
      (!(memcmp(terminals[termIndex].serialNumber, serialNumber,
         SER_NUM_LEN)));
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    finds an operator in the operator table
*/

bool db_findOperator(cchar* termOperator)
{
   return (bool) list_find(operatorTable, checkEmployee, (void*)termOperator);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    finds a serial number in the serial number table
*/

bool db_findSerialNumber(cchar* serialNumber)
{
   return (bool)
      list_find(serialNumberTable, checkSerialNumber, (void*)serialNumber);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    finds a supervisor in the supervisor table
*/

bool db_findSupervisor(cchar* supervisor)
{
   return (bool) list_find(supervisorTable, checkEmployee, (void*)supervisor);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    finds a terminal in the terminal table
*/

short db_findTerminal(cchar* terminalID)
{
   short i;

   for (i = 0; i < N_MAX_TERMS; i++)
   {
      if (terminals[i].state == TRM_SWITCHED_OFF)
         continue;

      if (!memcmp(terminals[i].terminalID, terminalID, TERM_ID_LEN))
         return i; // found
   }

   // didn't find the terminal id
   return -1;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    finds a terminal id  in the terminal id table
*/

bool db_findTerminalID(cchar* terminalID)
{
   return (bool)
      list_find(terminalIdTable, checkTerminalID, (void*)terminalID);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    initializes the db simulator.
*/

void db_init()
{
   dbMutex = server_createMutex();

   operatorTable_init();
   supervisorTable_init();
   serialNumberTable_init();
   terminalId_init();
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    inserts a terminal in the terminal table.
*/

short db_insertTerminal(cchar* serialNumber, cchar* terminalID, uint ipAddr,
   ushort connectionSeqNo)
{
   short i;

   for (i = 0; i < N_MAX_TERMS; i++)
      if (terminals[i].state == TRM_SWITCHED_OFF)
      {
         // found an empty slot
         // for now we have only the serial number and the state
         printInfo("changing state for terminal %.*s from %d to %d",
            TERM_ID_LEN, terminalID, TRM_SWITCHED_OFF, TRM_RECOGNIZED);
         terminals[i].state = TRM_RECOGNIZED;
         memcpy(terminals[i].terminalID, terminalID, TERM_ID_LEN);
         memcpy(terminals[i].serialNumber, serialNumber, SER_NUM_LEN);
         memset(terminals[i].supervisor, 0, EMPLOYEE_LEN);
         memset(terminals[i].termOperator, 0, EMPLOYEE_LEN);
         terminals[i].ipAddr = ipAddr;
         terminals[i].connectionSeqNo = connectionSeqNo;
         return i;
      }

   // couldn't find an empty slot

   printError("coudn't find empty slot in terminal table sn:%.*s id:%.*s",
      SER_NUM_LEN, serialNumber, TERM_ID_LEN, terminalID);

   return -1;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    locks the database.
*/

void db_lockDatabase()
{
   server_lockMutex(dbMutex);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    sets the operator number of a terminal.
*/

void db_setTerminalOperator(short termIndex, cchar* termOperator)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   memcpy(terminals[termIndex].termOperator, termOperator, EMPLOYEE_LEN);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    sets the state of a terminal.
*/

void db_setTerminalState(short termIndex, uchar state)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   printInfo("changing state for terminal %.*s from %d to %d", TERM_ID_LEN,
      terminals[termIndex].terminalID, terminals[termIndex].state, state);
   terminals[termIndex].state = state;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    sets the supervisor number of a terminal.
*/

void db_setTerminalSupervisor(short termIndex, cchar* supervisor)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   memcpy(terminals[termIndex].supervisor, supervisor, EMPLOYEE_LEN);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the id of a terminal.
*/

cchar* db_terminalID(short termIndex)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   return terminals[termIndex].terminalID;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    retrieves the state of a terminal.
*/

uchar db_terminalState(short termIndex)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   return terminals[termIndex].state;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    updates the connection information of a terminal.
*/

void db_updateTerminalConnectionID(short termIndex, uint ipAddr,
   ushort connectionSeqNo)
{
   assert(termIndex >= 0 && termIndex < N_MAX_TERMS);
   terminals[termIndex].ipAddr = ipAddr;
   terminals[termIndex].connectionSeqNo = connectionSeqNo;
}
// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Public function:
    unlocks the database.
*/

void db_unlockDatabase()
{
   server_unlockMutex(dbMutex);
}

// -----------------------------------------------------------------------------
// HELPER FUNCTIONS
// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// compares operators and supervisors

bool checkEmployee(void* entry, void* arg)
{
   cchar* employee = (cchar*)entry;
   cchar* otherEmployee = (cchar*)arg;
   return (!(memcmp(employee, otherEmployee, EMPLOYEE_LEN)));
}

// -----------------------------------------------------------------------------

// compares serial numbers

bool checkSerialNumber(void* entry, void* arg)
{
   cchar* sn = (cchar*)entry;
   cchar* otherSN = (cchar*)arg;
   return (!(memcmp(sn, otherSN, SER_NUM_LEN)));
}

// -----------------------------------------------------------------------------

// compares terminal ids

bool checkTerminalID(void* entry, void* arg)
{
   cchar* ti = (cchar*)entry;
   cchar* otherti = (cchar*)arg;
   return (!(memcmp(ti, otherti, TERM_ID_LEN)));
}

// -----------------------------------------------------------------------------

void loadTable(cchar* fileName, ushort fieldLen, List* list)
{
   char buf[80];
   char field[80];
   int n, nRecords = 0;

   FILE* fp = fopen(fileName, "rb");
   if (!fp)
   {
      printFatal("missing file %s", fileName);
      abort();
   }

   // loads table
   // format: one field per line, only first string of line is considered
   // list_addTail(list, strdup(field));

   for (;;)
   {
      nRecords++;
      if (!fgets(buf, 80, fp))
         break;
      n = strlen(buf);
      if (n && (buf[n-1] == '\n'))  // chomps new line
         buf[n-1] = 0;
      if (!strlen(buf)) // skips empty line
         continue;
      n = sscanf(buf,"%s", field);
      if (n != 1)
      {
         printError("invalid %s (1): %d", fileName, nRecords);
         continue;
      }
      if (strlen(field) != fieldLen)
      {
         printError("invalid %s record (2): %d", fileName, nRecords);
         continue;
      }
      list_addTail(list, strdup(field));
   }

   if (!nRecords)
   {
      printFatal("invalid %s file", fileName);
      abort();
   }
}

// -----------------------------------------------------------------------------

void operatorTable_init()
{
   operatorTable = list_create();
   loadTable("operator.txt", EMPLOYEE_LEN, operatorTable);
}

// -----------------------------------------------------------------------------

void serialNumberTable_init()
{
   serialNumberTable = list_create();
   loadTable("serial_number.txt", SER_NUM_LEN, serialNumberTable);
}

// -----------------------------------------------------------------------------

void supervisorTable_init()
{
   supervisorTable = list_create();
   loadTable("supervisor.txt", EMPLOYEE_LEN, supervisorTable);
}

// -----------------------------------------------------------------------------

void terminalId_init()
{
   terminalIdTable = list_create();
   loadTable("terminal_id.txt", TERM_ID_LEN, terminalIdTable);
}

// -----------------------------------------------------------------------------
} // close anonymous anmespace
// -----------------------------------------------------------------------------
// the end
