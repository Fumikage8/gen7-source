// ---------------------------------------------------------------------------

#include "qt/gfl_QtFile.h"
#include "core/gfl_Utility.h"

//
// ---------------------------------------------------------------------------

QByteArray gfl::qt::File::Load( const QString& fname, const QIODevice::OpenModeFlag flag )
{
	QByteArray txt;
	// ファイルポインタ FILE みたいなもの？
	QFile file( fname );
	// ファイルオープン
	if( file.open( QIODevice::ReadOnly | flag ) )
	{
		txt = file.readAll( );
		// ファイルクローズ
		file.close( );
	}
	// 驚くことに実体返しでOK！
	return txt;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

QStringList gfl::qt::File::LoadLines(const QString& fname, const bool remove_crlf)
{
	QStringList txt;
	// ファイルポインタ FILE みたいなもの？
	QFile file( fname );
	// ファイルオープン
	if( file.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		c8 buf[LINE_BUFFER_SIZE];
		while(-1 != file.readLine( buf,LINE_BUFFER_SIZE )){
			if(remove_crlf){
				size_t len = strlen( buf );
				for(;len;){
					c8 c = buf[--len];
					if(c == '\r' || c == '\n'){
						buf[len] = 0x00;
					}else{
						break;
					}
				}
			}
			txt.append( buf );
		}
		// ファイルクローズ
		file.close( );
	}
	// 驚くことに実体返しでOK！
	return txt;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

bool gfl::qt::File::Save( const QString& fname, const QByteArray& dat, const QIODevice::OpenModeFlag flag )
{
	// ファイルポインタ FILE みたいなもの？
	QFile file( fname );
	// ファイルオープン
	bool ret = true;
	if( file.open( QIODevice::WriteOnly | flag ) ){
		if( -1 == file.write( dat ) ){
			ret = false;
		}
		// ファイルクローズ
		file.close( );
	}
	return ret;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
u8* gfl::qt::File::MemCatEndianSwap( u8* dst, const void* src, const s32 size )
{
	switch( size ){
	case 4:
		*dst++ = ( ( u8* )src )[3];
	case 3:
		*dst++ = ( ( u8* )src )[2];
	case 2:
		*dst++ = ( ( u8* )src )[1];
	case 1:
		*dst++ = ( ( u8* )src )[0];
	}
	return dst;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
u8* gfl::qt::File::MemCatNoEndianSwap( u8* dst, const void* src, const s32 size )
{
	u8* u8_val = ( u8* )src;

	*dst++ = *u8_val++;
	if( size == 1 ){
		return dst;
	}
	*dst++ = *u8_val++;
	if( size == 2 ){
		return dst;
	}
	*dst++ = *u8_val++;
	if( size == 3 ){
		return dst;
	}
	*dst++ = *u8_val++;
	return dst;
}
// -----------------------------------------------------------------
// Link
// -----------------------------------------------------------------
bool gfl::qt::File::LinkBinaryFiles( const QString& save_fname, const QStringList& fnames, const s32 endian, const u32 align_size, bool header )
{
	// アラインメントのマスクを求める get alignment mask
	const u32 align_mask = align_size - 1; // align size must be 2^

	u8 *buf, *buf_org;
	// 一度に読み書きするサイズ
	static const u32 GFL_BINARY_BUFFER_SIZE = 0xf000;
	buf = buf_org = static_cast < u8* >( malloc( GFL_BINARY_BUFFER_SIZE ) );
	if( !buf ){
//		_ErrorMessage = QString( "リンク時のメモリー確保(%1 byte)に失敗しました。" ).arg( GFL_BINARY_BUFFER_SIZE );
		return false;
	}

	// エンディアンにしたがってメモリに配置
	MemCatByEndian cat = ( endian == MODE_LITTLE_ENDIAN ) ? MemCatNoEndianSwap : MemCatEndianSwap;
	// ファイルの数を格納 Store file count
	const u32 file_max = fnames.size( );
	buf = ( *cat )( buf, ( void* )&file_max, sizeof(u32 ) );

	// 実データのオフセットを求める
	u32 ofs = ( file_max + 2 ) << 2; // +2 for file count and file tail size <<2 == *sizeof(u32)
	u32 pad = ofs & align_mask;
	if( pad ){
		ofs += align_size - pad;
	}
	buf = ( *cat )( buf, ( void* )&ofs, sizeof(u32 ) );

	for( u32 i = 0; i < file_max; i++ ){
		u32 file_size;
		file_size = fnames.at( i ).size( );

		pad = file_size & align_mask;
		if( pad ){
			file_size += align_size - pad;
		}
		ofs += file_size;

		buf = ( *cat )( buf, &ofs, sizeof(u32 ) );
	}

	// ヘッダ部分をアランメントする
	pad = static_cast < u32 >( buf - buf_org ) & align_mask;
	if( pad ){
		pad = align_size - pad;
		memset( buf, 0x00, pad );
		buf += pad;
	}

	// ヘッダファイル
	QFile hfp( save_fname.section('.',0,-2) + ".h" );
#if OUTPUT_CSV
	QFile cfp( _CharName + ".csv" );
#endif
	if( header && endian == MODE_LITTLE_ENDIAN ){ // little のときだけヘッダ作成
		if( hfp.open( QIODevice::WriteOnly ) ){
			hfp.write( "#ifdef "LINK_KEYWORD"\n\n" );
		} else {
//			_ErrorMessage = QString( "リンク時のヘッダーオープン(%1)に失敗しました。" ).arg( _CharName + HEADER_EXT );
			return false;
		}
#if OUTPUT_CSV
		cfp.open( QIODevice::WriteOnly );
#endif
	} else {
		header = false;
	}

	QFile wfp( save_fname );
	if(!wfp.open( QIODevice::WriteOnly )){
//		_ErrorMessage = QString("リンク時のデータファイルオープン(%1)に失敗しました。").arg(save_fname);
		return false;
	}
	wfp.write( ( const c8* )buf_org, buf - buf_org );
	buf = buf_org; // 書き込んだので再利用

	for( u32 i = 0; i < file_max; ++i ){
		if( header ){ // little のときだけヘッダ作成
			//			fprintf(hfp,"GR_FILE_LINKED( %s , %s )\n", header_fname.toupper().data(), header_fname.data());
			const QString name( fnames.at( i ).section( '.', 0, 0 ) );
			hfp.write( QString( LINK_KEYWORD"( %1 )\n" ).arg( name ).toAscii( ).data( ) );
#if OUTPUT_CSV
			cfp.write( QString( "%1," ).arg( name ).toAscii( ).data( ) );
#endif
		}

		QFile fp( fnames.at( i ) );
		if( fp.open( QIODevice::ReadOnly ) ){
			u32 file_size = fp.size( );
			for(;; ){
				u32 read_size = static_cast < u32 >( fp.read( ( c8* )buf, GFL_BINARY_BUFFER_SIZE ) );
				if( read_size != GFL_BINARY_BUFFER_SIZE ){ // GFL_BINARY_BUFFER_SIZE 分読み込めなかった
					pad = read_size & align_mask;
					if( pad ){
						pad = align_size - pad;
						memset( &buf[read_size], 0x00, pad );
						read_size += pad;
					}
				}
				wfp.write( ( const c8* )buf, read_size );
				if( file_size <= read_size ){
					break;
				} else {
					file_size -= read_size;
				}
			}

			fp.close( );
		}
	}

	wfp.close( );

	if( header ){ // little のときだけヘッダ作成
		hfp.write( "\n#undef "LINK_KEYWORD"\n#endif // "LINK_KEYWORD"\n" );
		hfp.close( );
#if OUTPUT_CSV
		cfp.close( );
#endif
	}

	free( buf_org );
	return true;
}

//==============================================================================
//==============================================================================
bool gfl::qt::File::CheckModified(const QString& orgfname, const QString& tagfname)
{
	QFileInfo fi_org( orgfname );
	if( fi_org.exists( ) ){
		QFileInfo fi_tag( tagfname );
		if( fi_tag.exists( ) ){
			if( fi_tag.lastModified( ) < fi_org.lastModified( ) ){ // 元ファイルの方が新しい
				return true;
			}
		}else{ // 先ファイルがない
			return true;
		}
	}
	return false;
}
//==============================================================================
//==============================================================================
bool gfl::qt::File::CheckModified(const QDateTime& orgtime, const QString& fname)
{
	QFileInfo fi_org( fname );
	if( fi_org.exists( ) ){
		return ( orgtime < fi_org.lastModified( ));
	}
	return false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
QDateTime gfl::qt::File::GetModifiedTime( const QString& fname )
{
	QFileInfo fi_org( fname );
	if( fi_org.exists( ) ){
		return fi_org.lastModified( );
	}
	return QDateTime();
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
QString gfl::qt::File::ToAbsoluteFilePath(const QString& fname)
{
	QDir dir(QDir::current());
	return dir.cleanPath(dir.absoluteFilePath(QDir::fromNativeSeparators(fname) ) );
}
