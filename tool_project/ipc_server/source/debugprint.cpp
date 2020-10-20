// debug.cpp
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
 
#include "debugprint.h"

#define _DEBUG_OUT_BUFF_SIZE_ 128
 
bool dprintf( const char *str, ...)
{
    char debugOutBuff[ _DEBUG_OUT_BUFF_SIZE_ ];
 
    va_list ap;
    va_start( ap, str );
 
    if ( !vsprintf_s( debugOutBuff, _DEBUG_OUT_BUFF_SIZE_, str, ap ) ) {
        OutputDebugStringA( "dprintf error." );
        return false;
    }
    OutputDebugStringA( debugOutBuff );
 
    return true;
}