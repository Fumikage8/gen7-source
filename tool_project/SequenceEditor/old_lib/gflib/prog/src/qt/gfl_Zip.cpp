#include <zlib.h>
#include <qt/gfl_Zip.h>

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

bool gfl::qt::Zip::DecodeFile( QByteArray* dst, const QString& fname )
{
// �t�@�C���̍Ō�S�o�C�g���𓀌�̃T�C�Y
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
	// gz �Ƃ��ăI�[�v��
	gzFile inFile = gzopen( fname.toAscii( ).data( ), GZ_MODE );
	if( inFile == NULL ){
		return false;
	}

	// �𓀌�o�b�t�@
	dst->resize( dae_size );

	// �t�@�C����ǂ�ŉ𓀂����s
	s32 ret = gzread( inFile, dst->data( ), dae_size );
	if( ret == -1 ){
		// gzerror�ŃG���[���b�Z�[�W���擾����
		const char *msg = gzerror( inFile, &ret );
		if( ret == Z_ERRNO ){
			msg = strerror( ret );
		}
//		gfl::core::Debug::PrintConsole( "gzread failed. %s\n", msg );
		return false;
	}

	// �t�@�C���N���[�Y
	if( ( ret = gzclose( inFile ) ) != Z_OK ){
		// gzclose�Ɏ��s�����ꍇ,�t�@�C���͕�����̂�gzerror�ł��Ȃ�
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

	// �f�[�^��zip�`���ň��k���ďo�͂���.
	int writeLen = gzwrite( outFile, data.data( ), data.size( ) );
	if( writeLen != data.size( ) ){
		GFL_WARNING( "failed to gzwrite\n" );
		return;
	}

	// gz�t�@�C���̃N���[�Y
	int ret;
	if( ( ret = gzclose( outFile ) ) != Z_OK ){
		// gzclose�Ɏ��s�����ꍇ,�t�@�C���͕�����̂�gzerror�ł��Ȃ�
		GFL_WARNING( "gzclose failed.\n" );
		return;
	}
#endif
}
