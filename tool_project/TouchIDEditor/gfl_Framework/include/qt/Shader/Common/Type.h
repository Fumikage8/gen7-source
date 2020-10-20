#ifndef TYPE_H_
#define TYPE_H_

#include "Platform.h"

typedef	unsigned char		UChar;
typedef	unsigned short		UShort;
typedef	unsigned int		UInt;
typedef	unsigned long		ULong;
typedef	char				Int8, int8;
typedef	unsigned char		UInt8, uint8;
typedef	short				Int16, int16;
typedef	unsigned short		UInt16, uint16;
typedef	int					Int32, int32;
typedef	unsigned int		UInt32, uint32;
typedef	float				Float32;
typedef unsigned long long int ulong128;

typedef unsigned long long int ulong128;
typedef	long long			Int64, int64;
typedef	unsigned long long	UInt64, uint64;

#define SAFE_DELETE( p )       { if( p ) { delete ( p );		(p)=0; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );		(p)=0; } }

#endif
