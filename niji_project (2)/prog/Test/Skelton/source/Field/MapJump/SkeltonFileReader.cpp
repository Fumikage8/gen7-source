#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file file_reader.cpp
 @brief データファイル読み込み
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************

#include <stdio.h>
#include <string.h>
#include "Test/Skelton/include/Field/MapJump/SkeltonFileReader.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

FileReader::FileReader()
{
}

FileReader::~FileReader()
{
}

c8* FileReader::ReadFile( const c8* pFilePath, s32* pSize, s32 align)
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	fopen_s( &hFile, pFilePath, "rb" );

	if ( hFile == NULL ){
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*pSize = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = new c8[ buf_size ];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );
	
	return( read_buff );
}

c8* FileReader::ReadFile( const c16* pFilePath, s32* pSize, s32 align )
{
	FILE		*hFile;
	int			file_size;
	c8*    read_buff;

	_wfopen_s( &hFile, pFilePath, L"rb" );

	if ( hFile == NULL ){
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*pSize = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = new c8[ buf_size ];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );
	
	return( read_buff );
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
