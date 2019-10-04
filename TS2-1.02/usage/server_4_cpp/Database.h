/// @source       Database.h
/// @description  Simulated database access.

// -----------------------------------------------------------------------------
#ifndef __DATABASE_H__
#define __DATABASE_H__
// -----------------------------------------------------------------------------

void db_init(void);

bool db_findOperator(cchar*);
bool db_findSerialNumber(cchar*);
bool db_findSupervisor(cchar*);
short db_findTerminal(cchar*);
bool db_findTerminalID(cchar*);

bool db_checkTerminalConnectionID(short, uint, ushort);
bool db_checkTerminalOperator(short, cchar*);
bool db_checkTerminalSerialNumber(short, cchar*);

short db_insertTerminal(cchar*, cchar*, uint, ushort);

cchar* db_terminalID(short);
uchar db_terminalState(short);

void db_setTerminalState(short, uchar);
void db_setTerminalOperator(short, cchar*);
void db_setTerminalSupervisor(short, cchar*);

void db_updateTerminalConnectionID(short, uint ipAddr, ushort);

void db_lockDatabase();
void db_unlockDatabase();

// -----------------------------------------------------------------------------
#endif // __DATABASE_H__
// -----------------------------------------------------------------------------
