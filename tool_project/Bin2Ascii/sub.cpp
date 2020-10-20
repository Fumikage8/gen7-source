#include "sub.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

#undef  MessageBox
#define MessageBox		//

#pragma warning ( disable : 4996 )

/*#################################################################################
##
##			リードファイル関数
##
##################################################################################*/
char* ReadFile( char* filename, int* p_size, int align )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	hFile = fopen( filename, "rb" );

	if ( hFile == NULL ){
		char	mess[256];	
		memset( mess, 0, 256 );
		sprintf( mess, "%s can not file open.\n", filename );
		printf( mess );
		MessageBox( NULL, mess, NULL, MB_OK );
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	fseek( hFile, 0, SEEK_SET );

	size_t buf_size = file_size + ((-file_size) & (align-1));
	{
		char	mess[256];	
		memset( mess, 0, 256 );
		sprintf( mess, "%d %d %d\n", buf_size, file_size, align );
		printf( mess );
		MessageBox( NULL, mess, NULL, MB_OK );
	}

	read_buff = new char[buf_size];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );

	*p_size = file_size;
	return( read_buff );
}

void ChangeExtension( char *pSrc, char *pDst, char *pExt )
{
	int			size;

	size = (int)strlen( pSrc );

	while( size ){
		if ( pSrc[size] == '.' )		break;
		size --;
	}

	if ( size ){
		memcpy( pDst, pSrc, size );
	}else{
		memcpy( pDst, pSrc, (int)strlen( pSrc ) );//たぶん拡張子の指定がなかった
	}

	strcat( pDst, pExt );//拡張子追加

}