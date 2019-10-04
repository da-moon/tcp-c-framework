/// @source       Operations.h
/// @description  Simulated operations for POS terminal.

// -----------------------------------------------------------------------------
#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__
// -----------------------------------------------------------------------------

typedef void OpFunc(const Message*, Message*, short);

// the operation mapping to processing function
typedef struct _Operation
{
   char* code;     // an operation code
   OpFunc* opFunc; // and its processing function
} Operation;

OpFunc* oper_findFunc(cchar*);

// -----------------------------------------------------------------------------
#endif // __OPERATIONS_H__
// -----------------------------------------------------------------------------
