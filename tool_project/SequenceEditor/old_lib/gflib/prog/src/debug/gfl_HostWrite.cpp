#include <debug/gfl_Host.h>

#if GFL_HIO_ENABLE
//==============================================================================
/**
 * @file	HostWrite.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/28, 11:24
 */
// =============================================================================

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
gfl::hio::HostWrite::HostWrite( gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap, const s32 channel_id, const u32 buffer_size, const u32 stackSize )
: HostBase( heap, dev_heap, channel_id, stackSize ), m_pHostRead( 0 ), m_BufferSize( buffer_size )
{
#    if USE_GFL_NEW
	//		m_pWriteBuffer = GFL_NEW_ARRAY( heap ) c8 [buffer_size];
	m_pWriteBuffer = ( c8* )GflHeapAllocMemoryAlign( m_pHeapBase, buffer_size, GFL_HIO_ALIGN );
#    else
	m_pWriteBuffer = ( c8* )_aligned_malloc( buffer_size, GFL_HIO_ALIGN );
#    endif
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
gfl::hio::HostWrite::~HostWrite( void )
{
#    if USE_GFL_NEW
	GflHeapFreeMemory( m_pWriteBuffer );
#    else
	_aligned_free( m_pWriteBuffer );
#    endif
	m_pWriteBuffer = 0;

	if( m_pHostRead ){
		if( !m_pHostRead->IsAlive( ) ){
			nn::hio::CTR::Finalize( );
		}
		m_pHostRead = 0;
	}
}
//==============================================================================
//==============================================================================
void gfl::hio::HostWrite::Main( void )
{
	gfl::core::Time::SleepMilliSecond( HIO_WAIT_TIME );
	switch( m_Step ){
	case WAIT_FOR_CONNECT:
		if(!WaitForConnect( )){
			break;
		}
	case CONNECTED:
//		gfl::core::Debug::PrintConsole( "write wait\n");
		Wait( );

		if( m_Step == TERMINATE ){
			break;
		}

//		gfl::core::Debug::PrintConsole( "write begin\n");

		if( !m_DataSize ){
			return;
		}
		//		GFL_ASSERT_MSG( m_DataSize < m_BufferSize, "HIO write buffer over 0x%x", m_DataSize );
//		WriteSize( m_DataSize );
		//					gfl::core::Debug::PrintConsole( "write size %d\n",m_DataSize.GetValue());
		Write( m_pData, m_DataSize );
		//			gfl::core::Debug::PrintConsole( "write %s\n",(c8*)m_pData);

		//		if( 0 == strcmp( s_pTerminationString, ( c8* )m_pData ) ){
		//				gfl::core::Debug::PrintConsole( "w\n");
		//		}

		m_DataSize = 0;
		break;
	case TERMINATE:
		//		gfl::core::Debug::PrintConsole( "term w %d\n",m_ChannelId);
		Close( );
		Kill( );
	}
}

//==============================================================================
//==============================================================================
void gfl::hio::HostWrite::Write( const void*buffer, size_t size )
{
//	gfl::core::Debug::PrintConsole( "Write begin %d\n", size );
	if(m_BufferSize <= size){
		size = m_BufferSize-1;
	}
	// PC側のプログラムへデータを送信します。
	memcpy( m_pWriteBuffer,buffer,size );
	m_pWriteBuffer[size] = 0x00;
	s32 ret = m_Channel.Write( m_pWriteBuffer, size, nn::hio::CTR::SerialChannel::ATTRIBUTE_NONE );

	if( ret <= 0 ){
		NN_LOG( "m_Channel.Write() ERROR(ret:%d)\n", ret );
		m_Channel.Disconnect( );
#    if 0 // 
		m_Channel.Close( );
#    endif
		m_Step = WAIT_FOR_CONNECT;
		return;
	}

	//	NN_LOG( "Write size: %d\n", writesize );

	// PC側のプログラムがデータを受信するまで待ちます。
	nn::Result result = m_Channel.Flush( );
	if( result.IsFailure( ) ){
		NN_LOG( "m_Channel.Flush() ERROR(code:%08x)\n", result.GetPrintableBits( ) );
		m_Channel.Disconnect( );
#    if 0 // 
		m_Channel.Close( );
#    endif
		m_Step = WAIT_FOR_CONNECT;
		return;
	}
	//	NN_LOG( "Flush\n" );
}

#    if 0 // 
//==============================================================================
//==============================================================================
void gfl::hio::HostWrite::SendStringToClient( const c8 * const str )
{
	for(;; ){
		// 書き込み待ちか？
		if( !GetDataSize( ) ){
			c8* buf = GetWriteBuffer( );

			strcpy( buf, str );
			SetData( buf );
			return;
		}
	}
}
#    endif

#endif
