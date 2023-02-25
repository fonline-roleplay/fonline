
#ifndef _FONLINE_MD5_
#define _FONLINE_MD5_

#include <tchar.h>
#include <iostream>
#include <Windows.h>

typedef struct
{
    char          hash[ 33 ]; // 32 chars for MD5, +1 for terminating zero
    unsigned char digest[ 16 ];
} TMD5;

typedef struct
{
    ULONG         i[ 2 ];
    ULONG         buf[ 4 ];
    unsigned char in[ 64 ];
    unsigned char digest[ 16 ];
} MD5_CTX;

extern LPVOID MapFile_ReadOnly( LPCTSTR lpFileName, DWORD& dwSize );
extern bool InitMD5( );
extern TMD5 GetMD5( PUCHAR Buffer, DWORD dwSize );

#endif _FONLINE_MD5_