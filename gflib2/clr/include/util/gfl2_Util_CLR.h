#if !defined(GFLIB2_UTIL_UTIL_CLR_H_INCLUDED)
#define GFLIB2_UTIL_UTIL_CLR_H_INCLUDED
#pragma once

#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace clr { namespace util { 

public class Util
{
public:
  static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
  {
    FILE *hFile;
    int file_size;
    char *read_buff;

    fopen_s( &hFile, filename, "rb" );

    if ( hFile == NULL )
    {
      assert(0);
      return NULL;
    }

    fseek( hFile, 0, SEEK_END );
    file_size = ftell( hFile );
    *p_size = file_size;
    fseek( hFile, 0, SEEK_SET );

    file_size += align;//必ずNULLが入るようにしたい。
    u32 buf_size = file_size + ((-file_size) & (align-1));

    read_buff = reinterpret_cast<c8*>( gfl2::gfx::GLMemory::Malloc(buf_size) );
    memset( read_buff, 0, buf_size );
    fread( read_buff, file_size, 1, hFile );
    fclose( hFile );

    return( read_buff );
  }
};

}}}

#endif
