#include <stdio.h>
#include <fstream>
#include <windows.h>
#include "sub.h"

#undef  MessageBox
#define MessageBox		//

#pragma warning ( disable : 4996 )


#define			HELP_MAX		(8)

char *help[HELP_MAX] = {
	"Binary2Bmp		ver 2.0\n",
	"Binary2Bmp *.* OutFile Byte\n",
	"バイナリコードをアスキーコードに変換します。\n",
	"第一引数 入力ファイル\n",
	"第二引数 出力ファイル\n",
	"第三引数 出力Byte単位(1or2or4)\n",
	"第四引数 暗号化オプション:-secure or -nosecure(デフォルトは-nosecure)\n",
	"第五引数 パディング無効化:-nopadding\n",
};



//"C:/home/niji/git_gflib2/gfx/Sample/data/DrawUtilPixelShader.bfsh" "C:/home/niji/git_gflib2/gfx/Sample/data/DrawUtilPixelShader.bfsh.txt" 1 -nosecure -nopadding
//"C:\home\xy_tools\QtEx\Tools\Bin2Ascii\test.dat" "C:\home\xy_tools\QtEx\Tools\Bin2Ascii\main.txt" 1 -secure
bool BinaryToChangeAscii( char *pSrcFile, char *pOutFile, int OutByte, bool fSecure, int padding );
unsigned char GetSecureCode( unsigned char code, short seed );
unsigned char DeCode( unsigned char code, short seed );

void main( int argc, char* argv[] )
{
	int			i;
	char		file_name[256];
	char		out_name[256];
	char		last_mess[256];
	char		option[256];
	char		padding_option[256];
	int			out_byte = 1;
	bool		fSecure = false;
	int			padding = 1;				// 2014.07.01追加. デフォルトではパディングを有効化して後方互換性を保つ.

	memset( file_name, 0, sizeof(file_name) );
	memset( out_name, 0, sizeof(out_name) );
	memset( last_mess, 0, sizeof(last_mess) );
	memset( option, 0, sizeof(option) );
	memset( padding_option, 0, sizeof(padding_option) );

	switch( argc ){
	case 6:
		strcat( padding_option, argv[5] );
		MessageBox( NULL, padding, NULL, MB_OK );
		if ( strcmp( padding_option, "-nopadding" ) == 0 )
		{
			padding = 0;			// パディングを無効化
		}
	case 5:
		strcat( option, argv[4] );
		MessageBox( NULL, option, NULL, MB_OK );

		if ( strcmp( option, "-secure" ) == 0 )
		{
			fSecure = true;
		}
	case 4:
		out_byte = atoi( argv[3] );
		MessageBox( NULL, argv[3], NULL, MB_OK );
	case 3:
		strcat( out_name, argv[2] );
		MessageBox( NULL, argv[2], NULL, MB_OK );
	case 2:
		strcat( file_name, argv[1] );
		MessageBox( NULL, argv[1], NULL, MB_OK );

		if ( argc == 2 ){
			ChangeExtension( file_name, out_name, ".txt" );
		}

		MessageBox( NULL, file_name, out_name, MB_OK );

		if ( BinaryToChangeAscii( file_name, out_name, out_byte, fSecure, padding ) ){
			sprintf( last_mess, "%sを%sに変換しました。\n", file_name, out_name );
			MessageBox( NULL, last_mess, NULL, MB_OK );
			printf( last_mess );
		}else{
			//何かの原因で失敗
			exit( -1 );
		}

		break;
	default:
		for( i = 0; i < HELP_MAX; i ++ ){
			MessageBox( NULL, help[i], out_name, MB_OK );
			printf( help[i] );
		}
		break;
	}

	//getchar();
	exit( 0 );
}

bool BinaryToChangeAscii( char *pSrcFileName, char *pOutFileName, int OutByte, bool fSecure, int padding )
{
	int					i, i2;
	char				*pFile;
	int					FileSize;
	int					srt_num;
	int					srt_num_last;
	char				tmpbuf[256];
	char				tmpbuf2[256];
	unsigned char		*p1byte;
	unsigned short		*p2byte;
	unsigned int		*p4byte;
	FILE				*outfile;
	const int			align = 16;

	pFile = ReadFile( pSrcFileName, &FileSize, align );
	if ( pFile == NULL )		return false;
	srt_num = FileSize / OutByte;
	srt_num_last = FileSize % align;

	outfile = fopen( pOutFileName, "wb");
	if ( outfile == NULL ){
		char	mess[256];	
		memset( mess, 0, 256 );
		sprintf( mess, "%s can not file open.\n", pOutFileName );
		printf( mess );
		MessageBox( NULL, mess, NULL, MB_OK );
		return false;
	}

	Random			rand;

	for( i = 0; i < srt_num; i += ( 16 / OutByte ) )
	{
		memset( tmpbuf, 0, sizeof(tmpbuf) );

		if ( OutByte == 1 ){
			p1byte = (unsigned char*)&pFile[i];

			for( i2 = 0; i2 < 16; i2 ++ ){
				memset( tmpbuf2, 0, sizeof(tmpbuf2) );

				unsigned char		code = p1byte[i2];

				if ( fSecure )
				{
					short		seed = rand.Get();
					code = GetSecureCode( code, seed );
					//code = DeCode( code, seed );
				}

				if( (i + ( 16 / OutByte ) >= srt_num ) &&		// 最後の行の書き込みで,
						(padding == 0) &&												// パディングが無効で,
						(i2 >= srt_num_last)										// 最後の値まで書き込んでいたら
					)
				{
					break;
				}
				sprintf( tmpbuf2, "0x%02X, ", code );
				strcat( tmpbuf, tmpbuf2 );
			}

		}else if ( OutByte == 2 ){
			p2byte = (unsigned short*)&pFile[i];

			for( i2 = 0; i2 < 8; i2 ++ ){
				memset( tmpbuf2, 0, sizeof(tmpbuf2) );
				sprintf( tmpbuf2, "0x%04X, ", p2byte[i2] );
				strcat( tmpbuf, tmpbuf2 );
			}

		}else if ( OutByte == 4 ){
			p4byte = (unsigned int*)&pFile[i];

			for( i2 = 0; i2 < 4; i2 ++ ){
				memset( tmpbuf2, 0, sizeof(tmpbuf2) );
				sprintf( tmpbuf2, "0x%08X, ", p4byte[i2] );
				strcat( tmpbuf, tmpbuf2 );
			}

		}

		strcat( tmpbuf, "\r\n" );
		
		fwrite( tmpbuf, strlen(tmpbuf), 1, outfile );

	}

	fclose( outfile );
	delete ( pFile );

	return true;
}

unsigned char GetSecureCode( unsigned char code, short seed )
{
	int			shift = seed % 8;

	unsigned char lCode;
	unsigned char rCode;

	lCode = ((code << shift) & 0xFF);

	shift = 8 - shift;
	rCode = ((code >> shift) & 0xFF);

	code = ( lCode + rCode );

	return code;
}

unsigned char DeCode( unsigned char code, short seed )
{
	int			shift = seed % 8;

	unsigned char lCode;
	unsigned char rCode;

	rCode = ((code >> shift) & 0xFF);

	shift = 8 - shift;
	lCode = ((code << shift) & 0xFF);

	code = ( lCode + rCode );

	return code;
}