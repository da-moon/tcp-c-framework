/// @source       Transactions.h
/// @description  Simulated POS transactions.

// -----------------------------------------------------------------------------
#ifndef __TRANSACTIONS_H__
#define __TRANSACTIONS_H__
// -----------------------------------------------------------------------------

typedef struct _MessageHeader
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
} MessageHeader;

typedef struct _ReplyHeader
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} ReplyHeader;

// --------------------------------------

typedef struct _Message_0000
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
   char serialNumber[SER_NUM_LEN];
} Message_0000;

typedef struct _Reply_0000
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} Reply_0000;

// --------------------------------------

typedef struct _Message_0010
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
   char supervisor[EMPLOYEE_LEN];
} Message_0010;

typedef struct _Reply_0010
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} Reply_0010;

// --------------------------------------

typedef struct _Message_0020
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
   char termOperator[EMPLOYEE_LEN];
} Message_0020;

typedef struct _Reply_0020
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} Reply_0020;

// --------------------------------------

#if PLATFORM(Windows)
#pragma warning(disable:4200) // zero-sized array
#endif

typedef struct _Message_0030
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
   char termOperator[EMPLOYEE_LEN];
   char operationCode[OP_CODE_LEN];
   char operationDataLen[OP_DATA_LEN];
   char operationData[0];
} Message_0030;

typedef struct _Reply_0030
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} Reply_0030;

// --------------------------------------

typedef struct _Message_0040
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
   char termOperator[EMPLOYEE_LEN];
} Message_0040;

typedef struct _Reply_0040
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} Reply_0040;

// --------------------------------------

typedef struct _Message_0050
{
   char tranCode[TRAN_CODE_LEN];
   char terminalID[TERM_ID_LEN];
   char supervisor[EMPLOYEE_LEN];
} Message_0050;

typedef struct _Reply_0050
{
   char tranCode[TRAN_CODE_LEN];
   char errCode[ERR_CODE_LEN];
} Reply_0050;

// --------------------------------------

// transaction processing function prototype
typedef Message* TranFunc(const Message*);

// the transaction mapping to processing function
typedef struct _Transaction
{
   cchar* code;        // a transaction code
   TranFunc* tranFunc; // and its processing function
} Transaction;

// -----------------------------------------------------------------------------

// prototypes

extern TranFunc* tran_findFunc(cchar*);

// -----------------------------------------------------------------------------
#endif // __TRANSACTIONS_H__
// -----------------------------------------------------------------------------
