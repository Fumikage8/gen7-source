//==============================================================================
/**
 * @file	gfl_File.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/19, 11:50
 */
// =============================================================================

#ifdef GFL_PLATFORM_PC
QString gfl::core::File::s_RootDir = "";//"../../../../../../romfiles/";
#endif

//==============================================================================
//==============================================================================
u8* gfl::core::File::Load( const c8 * const fname_org, gfl::heap::HeapBase* heap, size_t* read_size, const s32 mode )
{
#ifdef GFL_PLATFORM_3DS
	GFL_AVOID_WARNING( mode );
	c8 fname[0x100];
	memset( fname, 0x00, 0x80 );
	strcpy( fname, "rom:/" );
	strcat( fname, fname_org );
	nn::fs::FileReader motReader( fname );
	size_t size = motReader.GetSize( );
	//	u8* buffer = new u8 [size];
	u8* buffer = GFL_NEW_ARRAY( heap ) u8 [size];
	size_t dat_size = motReader.Read( buffer, size );
	motReader.Finalize( );
#elif defined(GFL_PLATFORM_PC)
	QByteArray dat = gfl::qt::File::Load( ( fname_org[0] != '/' && fname_org[1] != ':' ) ? s_RootDir + fname_org : fname_org,
										 ( mode == MODE_TEXT ) ? QIODevice::Text : static_cast < QIODevice::OpenModeFlag >( 0 ) );

	// just for test
	size_t dat_size = dat.size( );
	if( !dat_size ){
		gfl::core::Debug::PrintConsole( "Could not read %s\n", fname_org );
		return 0;
	}
	u8* buffer = GFL_NEW_ARRAY( heap ) u8 [dat_size+1];
	memcpy( buffer, dat.data( ), dat_size );
buffer[dat_size] = 0x00;
#else // 
	size_t dat_size = 0;
#endif
	if( read_size ){
		*read_size = dat_size;
	}
	return buffer;
}

//==============================================================================
//==============================================================================
gfl::core::ByteArrayPointer gfl::core::File::LoadSmart( const c8* fname_org, gfl::heap::HeapBase* heap, const s32 mode )
{
	size_t size;
	u8* buf = Load( fname_org, heap, &size, mode );
	return ByteArrayPointer( GFL_NEW( heap ) ByteArray( buf, size ) );
}
