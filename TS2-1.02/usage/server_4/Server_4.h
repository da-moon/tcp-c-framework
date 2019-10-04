/// @source       Server_4.h
/// @description   A simulated POS terminal server.

// -----------------------------------------------------------------------------
#ifndef __SERVER_4_H__
#define __SERVER_4_H__
// -----------------------------------------------------------------------------

// the server TCP service port
enum { SERVER_PORT = 4000 };

// the server IP address
#define SERVER_ADDR "127.1"

// -----------------------------------------------------------------------------

// configuration constants

enum
{
   // size of the supervisor and operator fields
   EMPLOYEE_LEN   = 10 ,

   // size of the error code field
   ERR_CODE_LEN   = 3 ,

   // maximum number of terminals supported by the application
   N_MAX_TERMS    = 50 ,

   // size of the operation code field
   OP_CODE_LEN    = 3  ,

   // size of the operation data length field
   OP_DATA_LEN    = 3  ,

   // size of the serial number field
   SER_NUM_LEN    = 20 ,

   // size of the terminal identification field
   TERM_ID_LEN    = 3  ,

   // size of the transaction code field
   TRAN_CODE_LEN  = 4 ,
};

// -----------------------------------------------------------------------------

// terminal states

enum
{
   TRM_SWITCHED_OFF , // initial state of the terminal
// TRM_SWITCHED_ON  , // terminal sent transaction "0000"
   TRM_RECOGNIZED   , // server acknowledged transation "0000"
   TRM_AUTHORIZED   , // server acknowledged transaction "0010" or "0040"
   TRM_OPERATING    , // server acknowledged transaction "0020"
   TRM_CLOSED       , // server acknowledged transaction "0050"
};

// -----------------------------------------------------------------------------

// errors

enum
{
   E_OK             = 0 ,
   E_INS_TERM       = 1 ,  // error when inserting terminal in terminal table
   E_INV_OP         = 2 ,  // invalid operator
   E_INV_OPCODE     = 3 ,  // invalid operation
   E_INV_SUP        = 4 ,  // invalid supervisor
   E_INV_TERM_STATE = 5 ,  // invalid terminal state
};

// -----------------------------------------------------------------------------

// some utility functions

void printError(cchar* fmt, ...);
void printFatal(cchar* fmt, ...);
void printInfo(cchar* fmt, ...);
void printWarn(cchar* fmt, ...);

// -----------------------------------------------------------------------------
#endif // __SERVER_4_H__
// -----------------------------------------------------------------------------
