/// @source       Client_4.h
/// @description  A simulated POS terminal.

// -----------------------------------------------------------------------------
#ifndef __CLIENT_4_H__
#define __CLIENT_4_H__
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

   // maximum operation code
   MAX_OP_CODE   = 4 ,
};

// -----------------------------------------------------------------------------

// terminal states

enum
{
   TRM_SWITCHED_ON = '0' , // initial state of the terminal
   TRM_RECOGNIZED        , // server acknowledged transation "0000"
   TRM_AUTHORIZED        , // server acknowledged transaction "0010" or "0040"
   TRM_OPERATING         , // server acknowledged transaction "0020"
   TRM_CLOSED            , // server acknowledged transaction "0050"
};

// -----------------------------------------------------------------------------

// some utility functions
void printError(cchar* fmt, ...);
void printFatal(cchar* fmt, ...);
void printInfo(cchar* fmt, ...);
void printWarn(cchar* fmt, ...);

// -----------------------------------------------------------------------------
#endif // __CLIENT_4_H__
// -----------------------------------------------------------------------------
