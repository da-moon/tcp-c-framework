/// @source       Database.h
/// @description  Simulated database access.

// -----------------------------------------------------------------------------
#ifndef __DATABASE_H__
#define __DATABASE_H__
// -----------------------------------------------------------------------------

void db_init(void);

// configuration
cchar* db_serialNumber(void);
cchar* db_terminalID(void);

ushort db_maxSleepTime();
ushort db_pBeginShift();
ushort db_pSendOper();

// state
cchar* db_operator(void);
uchar db_state(void);
cchar* db_supervisor(void);

void db_updateState(uchar);

// -----------------------------------------------------------------------------
#endif // __DATABASE_H__
// -----------------------------------------------------------------------------
