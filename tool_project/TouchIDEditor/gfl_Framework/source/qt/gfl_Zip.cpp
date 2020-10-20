#include <zlib.h>
#include <qt/gfl_Zip.h>

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

bool gfl::qt::Zip::DecodeFile( QByteArray* dst, const QString& fname )
{
// ファイルの最後４バイトが解凍後のサイズ
	u32 dae_size;
	FILE* fp = fopen( fname.toAscii( ).data( ), "rb" );

	if( !fp ){
//		gfl::core::Debug::PrintConsole( "failed to gzopen %s\n", fname.toAscii( ).data( ) );
		return false;
	}

	fseek( fp, -( s32 )sizeof(u32 ), SEEK_END );
	fread( &dae_size, 1, sizeof(u32 ), fp );
	fclose( fp );

#define GZ_MODE "rb6f"
	// gz としてオープン
	gzFile inFile = gzopen( fname.toAscii( ).data( ), GZ_MODE );
	if( inFile == NULL ){
		return false;
	}

	// 解凍後バッファ
	dst->resize( dae_size );

	// ファイルを読んで解凍を実行
	s32 ret = gzread( inFile, dst->data( ), dae_size );
	if( ret == -1 ){
		// gzerrorでエラーメッセージを取得する
		const char *msg = gzerror( inFile, &ret );
		if( ret == Z_ERRNO ){
			msg = strerror( ret );
		}
//		gfl::core::Debug::PrintConsole( "gzread failed. %s\n", msg );
		return false;
	}

	// ファイルクローズ
	if( ( ret = gzclose( inFile ) ) != Z_OK ){
		// gzcloseに失敗した場合,ファイルは閉じられるのでgzerrorできない
//		gfl::core::Debug::PrintConsole( "gzclose failed.\n" );
		return false;
	}
//	gfl::core::Debug::PrintConsole( "gz ok.\n" );
	return true;
}

// -----------------------------------------------------------------
// Encode File
// -----------------------------------------------------------------
void gfl::qt::Zip::EncodeFile( const QString& rfname )
{
#ifdef GFL_PLATFORM_PC
	#undef GZ_MODE
	#define GZ_MODE "wb6f"

	QByteArray data = gfl::qt::File::Load( rfname );

	if( !data.size( ) )
	{
		GFL_WARNING( "Failed to open %s or file size 0\n", rfname.toAscii( ).data( ) );
		return;
	}

	gzFile outFile = gzopen( (rfname + ".gz").toAscii( ).data( ), GZ_MODE );
	if( outFile == NULL )
	{
		GFL_WARNING( "Failed to gzopen\n" );
		return;
	}

	// データをzip形式で圧縮して出力する.
	int writeLen = gzwrite( outFile, data.data( ), data.size( ) );
	if( writeLen != data.size( ) ){
		GFL_WARNING( "failed to gzwrite\n" );
		return;
	}

	// gzファイルのクローズ
	int ret;
	if( ( ret = gzclose( outFile ) ) != Z_OK ){
		// gzcloseに失敗した場合,ファイルは閉じられるのでgzerrorできない
		GFL_WARNING( "gzclose failed.\n" );
		return;
	}
#endif
}
