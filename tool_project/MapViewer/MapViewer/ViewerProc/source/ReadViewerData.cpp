
/**
* @brief  フィールドの地形リソースの生成を担うクラス
* @file   FieldTerrainFactory.cpp
* @author ikuta_junya
* @data   2015.04.19
*/

// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include "ViewerProc/include/ReadViewerData.h"


//----------------------------------------------------------------------------
/**
*	@brief  ファイル読み込み
*	@param  利用するアロケータ
*	@param  ファイルパス
*	@return Rawデータ
*/
//-----------------------------------------------------------------------------
void* ReadData( const char* pFilePath,GLResourceAllocator *pAllocator )
{
#if defined(GF_PLATFORM_CTR)
  nn::fs::FileReader file( pFilePath );
  u32 fileSize = file.GetSize();
  void* pShaderBinary = pAllocator->SystemMemoryMalloc(fileSize, 128);
  file.Read(pShaderBinary, fileSize);
  return pShaderBinary;
#else
  FILE		*hFile;
  int			file_size;
  char		*read_buff;

  fopen_s( &hFile, pFilePath, "rb" );

  if ( hFile == NULL )
  {
    ///assert(0);
    return NULL;
  }

  fseek( hFile, 0, SEEK_END );
  file_size = ftell( hFile );
  fseek( hFile, 0, SEEK_SET );

  u32		align = 16;
  file_size += align;//必ずNULLが入るようにしたい。
  u32 buf_size = file_size + ((-file_size) & (align-1));

  read_buff = reinterpret_cast<c8*>( pAllocator->SystemMemoryMalloc(buf_size,128) );
  memset( read_buff, 0, buf_size );
  fread( read_buff, file_size, 1, hFile );
  fclose( hFile );

  return read_buff;
#endif
}