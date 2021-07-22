#ifndef __CRYPTO_DEFS_H__
#define __CRYPTO_DEFS_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <openssl/err.h>

#ifdef _WIN32
# ifdef WIN_EXPORT
#   define EXPORTED  __declspec( dllexport )
# else
#   define EXPORTED  __declspec( dllimport )
# endif
#else
# define EXPORTED
#endif

typedef unsigned int uint;
typedef unsigned long ulong;

const  uint  NO_ELEMENT = 0;
const  int   INT_ERROR_RET = -1;
const  uint  MAX_LEN_BUF = 10000;
extern char  charBuff[MAX_LEN_BUF];

#define POINTER_ERROR_RET (nullptr);

enum error_code {
    NO_MEMORY = 0,
    POINTER_NULL,
    PARAMETER_INVALID,
    NOT_FOUND_ELEMENT,
    ELEMENT_INVALID,
    ID_INVALID,
    CALC_FAILED,
    NOT_DEFINED_ACTION,
    CONTAINER_OVERFLOW,
    UNKNOWN_ERROR,
    NO_MESSAGE
};

EXPORTED const std::string errorDescription(const uint errorCode);
EXPORTED const std::string toHex(void* p);
extern "C" EXPORTED char* getCharBuff();
extern "C" EXPORTED void handleError(const uint errorCode);

#endif