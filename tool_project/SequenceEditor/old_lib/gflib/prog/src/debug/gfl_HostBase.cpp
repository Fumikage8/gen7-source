#include <debug/gfl_Host.h>

#if GFL_HIO_ENABLE
//==============================================================================
/**
 * @file	HostBase.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/28, 10:57
 */
// =============================================================================
gfl::hio::HostBase::HostBase( gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap, const s32 channel, const u32 stackSize )
	: gfl::base::Thread( heap, stackSize ), m_pData( 0 ), m_pHeapBase( heap ), m_pDeviceHeap(dev_heap), m_DataSize( 0 ), m_Step( 0 ),m_Size(0), m_ChannelId( channel ), /*m_Opened(false),*/ m_Connected(false)
{
#    if 0 // 
#if USE_GFL_NEW
//	m_pSize = (size_t*)GflHeapAllocMemoryAlign(heap,sizeof(size_t)+1,GFL_HIO_ALIGN);
#else
//	m_pSize = (size_t*)_aligned_malloc( sizeof(size_t), GFL_HIO_ALIGN );
#endif
#    endif
}

gfl::hio::HostBase::~HostBase( void ) {
#    if 0 // 
#if USE_GFL_NEW
	if(m_pSize){
		GflHeapFreeMemory(m_pSize);
		m_pSize = 0;
	}
#    else // 
	_aligned_free(m_pSize);
#    endif
#    endif
}

//==============================================================================
//==============================================================================
void gfl::hio::HostBase::Initialize( void )
{
	nn::Result result;

	//=======================================================================
	// PC側のプログラムと接続して一回ずつデータの送受信を行います。
	//=======================================================================

//	gfl::core::Debug::PrintConsole( "hio start\n" );
	// チャンネルを開きます。
	result = m_Channel.Open( m_ChannelId, reinterpret_cast < void* >( GflHeapAllocMemory(m_pDeviceHeap,nn::hio::CTR::WORKMEMORY_SIZE) ));

	if( result.IsFailure( ) ){
		gfl::core::Debug::PrintConsole( "m_Channel.Open() ERROR(code:%08x)\n", result.GetPrintableBits( ) );
		m_Channel.Close( );
		return;
	}
//	gfl::core::Debug::PrintConsole( "hio open %d\n",m_ChannelId );
//	m_Opened = true;
	if(m_Step != TERMINATE){
		m_Step = WAIT_FOR_CONNECT;
	}
}

#    if 1 // 
//==============================================================================
//==============================================================================
void gfl::hio::HostBase::FinalizeChannel( void )
{
	if(m_Connected){ // PC側との接続を切ります
		nn::Result result = m_Channel.Disconnect( );
		if( result.IsFailure( ) ){
			gfl::core::Debug::PrintConsole( "m_Channel.Disconnect() ERROR(code:%08x)\n", result.GetPrintableBits( ) );
			m_Channel.Close( );
			return;
		}
	}
//	gfl::core::Debug::PrintConsole( "disconnect.\n" );
}
#    endif

//==============================================================================
//==============================================================================
bool gfl::hio::HostBase::WaitForConnect( void )
{
	// PC側のプログラムと接続します。
	// 接続されるまで待ちます。
#    if 1 // 
//	gfl::core::Debug::PrintConsole( "hio wait %d\n", m_ChannelId );
	nn::Result result = m_Channel.Connect( );
	if( result.IsFailure( ) ){
//		gfl::core::Debug::PrintConsole( "m_Channel.Connect() ERROR(code:%08x:%d:%d:%d:%d)\n"
//		, result.GetPrintableBits( ),result.GetLevel(),result.GetSummary(),result.GetModule()
//		, result.GetDescription() );
#        if 0 // 
		m_Channel.Close( );
		m_Opened = false;
#        endif
		return false;
	}
	gfl::core::Debug::PrintConsole( "hio connect %d\n", m_ChannelId );
	m_Step = CONNECTED;
#    else // 
	nn::Result result = m_Channel.WaitHost( nn::hio::CTR::SerialChannel::ATTRIBUTE_NO_WAIT );

	if( result == nn::hio::CTR::ResultConnected() ){
		gfl::core::Debug::PrintConsole( "connect %d\n",m_ChannelId );
		m_Step = CONNECTED;
	}else{
//		gfl::core::Debug::PrintConsole( "w\n" );
		Wait();
	}
#    endif
	return true;
}
#endif
