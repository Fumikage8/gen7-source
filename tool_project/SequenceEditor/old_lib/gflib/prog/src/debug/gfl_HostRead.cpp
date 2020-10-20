#include <debug/gfl_Host.h>

#if GFL_HIO_ENABLE
//==============================================================================
/**
 * @file	HostRead.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/24, 18:48
 */
// =============================================================================
gfl::hio::HostRead::~HostRead( void )
{
	Clear( );
	if( m_pHostWrite ){
		if(!m_pHostWrite->IsAlive( ) ){
			nn::hio::CTR::Finalize( );
		}
		m_pHostWrite = 0;
	}
}

//==============================================================================
//==============================================================================
void gfl::hio::HostRead::Main( void )
{
	gfl::core::Time::SleepMilliSecond( HIO_WAIT_TIME );
	switch( m_Step ){
	case WAIT_FOR_CONNECT:
		if(!WaitForConnect( )){
			break;
		}
		m_Step = CONNECTED;

	case CONNECTED:
	{
		m_Channel.GetReadableSize(&m_Size,nn::hio::CTR::SerialChannel::ATTRIBUTE_IN_PC);
		const size_t size = m_Size;
		if(size <= 0){
//			Wait();
			return;
		}
		
		if(m_pHeapBase->GetTotalAllocatableSize() < size){
			gfl::core::Debug::PrintConsole( "hio too much memory alloc 0x%x\n",size );
			return;
		}
		
		bool aloc;
		if(m_pData){
			if(m_AllocSize < size){
//				GFL_DELETE_ARRAY m_pData;
				GflHeapFreeMemory(m_pData);
				aloc = true;
			}else{
				aloc = false;
			}
		}else{
			aloc = true;
		}
		
		if(aloc){
			const size_t aloc_size = (size+1);
#if USE_GFL_NEW
//			m_pData = GFL_NEW_ARRAY( m_pHeapBase ) u8 [aloc_size];
			m_pData = (u8*)GflHeapAllocMemoryAlign(m_pHeapBase,aloc_size,GFL_HIO_ALIGN);
#    else // 
			m_pData = (u8*)_aligned_malloc(aloc_size, GFL_HIO_ALIGN );
#    endif
//			gfl::core::Debug::PrintConsole( "aloc %d %d\n",aloc_size,size );
			m_AllocSize = size;
		}
//		gfl::core::Debug::PrintConsole( "read size %d %d\n",m_ChannelId,size );
		const s32 r_size = Read( m_pData, size );
//		const s32 r_size = 0;
		
		if(r_size<=0){
			gfl::core::Debug::PrintConsole( "read err %d\n", m_Size );
			m_Channel.Disconnect();
			m_Step = WAIT_FOR_CONNECT;
			return;
		}
		
//		gfl::core::Debug::PrintConsole( "read (0x%p)%s %d\n",m_pData,m_pData, r_size);
		
		if( 0 == strcmp(RECEIVED_STRING,( c8* )m_pData )){
			if( m_pHostWrite ){
				m_pHostWrite->SetData( RECEIVED_STRING );
			}
		}
			//				m_pHostWrite = 0;
			//			}
//			m_Step = TERMINATE;
//			break;
//		}

		m_DataSize = r_size;
		// 読みこんだので処理されるのを待つ
		Wait( );
		m_DataSize = 0;
	}
		break;
	case TERMINATE:
//		gfl::core::Debug::PrintConsole( "term r %d\n",m_ChannelId);
		
		Close( );
		Kill( );
	}
}

#    if 0 // 
//==============================================================================
//==============================================================================
s32 gfl::hio::HostRead::ReadSize( void )
{
#    if 0 // これじゃ駄目っす
	// PCからのデータを受信します。
	size_t readsize;
	nn::Result result = m_Channel.GetReadableSize( &readsize );
	if( result.IsFailure( ) ){
		NN_LOG( "m_Channel.ReadSize() ERROR(code:%08x)\n", result.GetPrintableBits( ) );
		m_Channel.Disconnect( );
		m_Channel.Close( );
	}
	return readsize;
#    else //
	return Read( static_cast < void* >( m_pSize ), sizeof(size_t ) );
//#    else // 
//	size_t size;
//	return m_Channel.Read( static_cast < void* >( &size ), sizeof(size ), nn::hio::CTR::SerialChannel::ATTRIBUTE_NO_WAIT );
#    endif
}

//==============================================================================
//==============================================================================
s32 gfl::hio::HostRead::Read( void* buffer, const size_t size )
{
#    if 0 // 
	// PCからのデータを受信します。
	size_t readsize;
	gfl::core::Debug::PrintConsole( "readptr %p %d\n",buffer,size );
	nn::Result result = m_Channel.Read( &readsize, buffer, size );
	if( result.IsFailure( ) ){
		NN_LOG( "m_Channel.Read() ERROR(code:%08x)\n", result.GetPrintableBits( ) );
		m_Channel.Disconnect( );
		m_Channel.Close( );
	}
#    else // 
	// PCからのデータを受信します。
//	gfl::core::Debug::PrintConsole( "readptr %p %d\n",buffer,size );
	return m_Channel.Read( buffer, size );
#        if 0 // 
	if( ret != size ){
		GFL_PRINT( "m_Channel.Read() ERROR ret %d size %d\n", ret, size );
		m_Channel.Disconnect( );
		m_Channel.Close( );
	}
#        endif
#    endif
}
#    endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
s32 gfl::hio::HostRead::Read( void* buffer, const size_t size ){
//	GFL_ASSERT_MSG( !(((size_t)buffer) & 0x1f), "read align error %p", buffer );
	
#if 0 // 
	s32 ret = m_Channel.Read( buffer, size, nn::hio::CTR::SerialChannel::ATTRIBUTE_NONE );	
#else // 
	s32 ret;
	nn::Result result = m_Channel.Read( (size_t*)&ret, buffer, size, nn::hio::CTR::SerialChannel::ATTRIBUTE_NONE );
	if(result.IsFailure()){
		ret = 0;
	}
#endif
#if 0 // 
	if(ret == size){
		u8* ensure = (u8*)GflHeapAllocMemoryAlign(m_pHeapBase,1,GFL_HIO_ALIGN);
		*ensure = 1;
		m_Channel.Write( ensure, 1, nn::hio::CTR::SerialChannel::ATTRIBUTE_NONE );
		GflHeapFreeMemory(ensure);
	}
#endif
	return ret;
//	return m_Channel.Read( buffer, size );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::hio::HostRead::Clear( void )
{
#if USE_GFL_NEW
	if(m_pData){
//		GFL_DELETE_ARRAY m_pData;
		GflHeapFreeMemory(m_pData);
		m_pData = 0;
	}
#else
	_aligned_free( m_pData );
	m_pData = 0;
#endif
	m_DataSize = 0;
}
#endif //GFL_HIO_ENABLE
