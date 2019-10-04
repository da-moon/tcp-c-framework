/// @source       Database.h
/// @description  Simulated database access.

// -----------------------------------------------------------------------------
#ifndef __DATABASE_H__
#define __DATABASE_H__
// -----------------------------------------------------------------------------

void db_init();

// configuration
cchar* db_serialNumber();
cchar* db_terminalID();

ushort db_maxSleepTime();
ushort db_pBeginShift();
ushort db_pSendOper();

// state
cchar* db_operator();
uchar db_state();
cchar* db_supervisor();

void db_updateState(uchar);

// -----------------------------------------------------------------------------
#endif // __DATABASE_H__
// -----------------------------------------------------------------------------
