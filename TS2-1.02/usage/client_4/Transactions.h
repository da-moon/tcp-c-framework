/// @source       Transactions.h
/// @description  Simulated POS transactions.

// -----------------------------------------------------------------------------
#ifndef __TRANSACTIONS_H__
#define __TRANSACTIONS_H__
// -----------------------------------------------------------------------------

enum HeaderOffset
{
   TRAN_CODE_OFFSET = 0 ,
   ERR_CODE_OFFSET = TRAN_CODE_OFFSET + TRAN_CODE_LEN ,
   TERM_ID_OFFSET  =  TRAN_CODE_OFFSET + TRAN_CODE_LEN ,
};

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

enum Msg000_Offset
{
   SER_NUM_OFFSET  =  TERM_ID_OFFSET + TERM_ID_LEN ,
};

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

enum Msg010_Offset
{
   SUPERV_OFFSET  =  TERM_ID_OFFSET + TERM_ID_LEN ,
};

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

enum Msg020_Offset
{
   TERM_OPER_OFFSET  =  TERM_ID_OFFSET + TERM_ID_LEN ,
};

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

enum Msg030_Offset
{
   OP_CODE_OFFSET    = TERM_OPER_OFFSET + EMPLOYEE_LEN ,
   OP_DATALEN_OFFSET = OP_CODE_OFFSET + OP_CODE_LEN ,
   OP_DATA_OFFSET    = OP_DATALEN_OFFSET + OP_DATA_LEN,
};

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

// -----------------------------------------------------------------------------
#endif // __TRANSACTIONS_H__
// -----------------------------------------------------------------------------
