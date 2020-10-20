//==============================================================================

#include "qt/gfl_ClientBase.h"
#include "debug/gfl_HioConfig.h"

/**
 * @file	ClientBase.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/28, 10:29
 */
// =============================================================================

::CDEVHIO gfl::hio::ClientBase::s_HostIo;
CISCTRDLL gfl::hio::ClientBase::s_ISCTR;
b32 gfl::hio::ClientBase::s_IsISCTR;

//==============================================================================
//==============================================================================
void gfl::hio::ClientBase::Initialize( void )
{
	s32 ret = CallHio(OpenSio, m_Channel ); // チャンネルを開きます。
	if( ret != 0 ){
		QString buf("Error OpenSio " + QString::number(m_Channel));
		if(!m_ErrorMessage.contains(buf)){
			m_ErrorMessage += buf;
		}
		return;
	}
	m_LogMessage += "\nOpenSio " + QString::number(m_Channel);
	gfl::core::Debug::PrintConsole( "opened %d\n",m_Channel);
	m_Step = WAIT_FOR_CONNECT;
}

//==============================================================================
//==============================================================================
gfl::hio::ClientBase::ClientBase( const s32 channel ) : m_Channel( channel ), m_Step( 0 )
{
	m_pSize = (size_t*)_aligned_malloc( sizeof(size_t), GFL_HIO_ALIGN );
}

//==============================================================================
//==============================================================================
gfl::hio::ClientBase::~ClientBase( void )
{
	const s32 channel = m_Channel;
	// 接続を解除して、チャンネルを閉じます。
	if(m_Step == CONNECTED){
		s32 ret = CallHio(DisconnectSio, channel );
		if( ret != 0 ){
//			gfl::core::Debug::PrintConsole( "DisconnectSio() ERROR(code:%08x)\n", ret );
			CallHio(CloseSio, channel );
			return;
		}
	}

	if(m_Step){
		s32 ret = CallHio(CloseSio, channel );
		if( ret != 0 ){
//			gfl::core::Debug::PrintConsole( "CloseSio() ERROR(code:%08x)\n", ret );
			return;
		}
	}
	
	_aligned_free(m_pSize);

	// スレッドを終了させる
	terminate();
	wait();
	
//	gfl::core::Debug::PrintConsole( "close.\n" );
}

//==============================================================================
//==============================================================================
bool gfl::hio::ClientBase::WaitForConnect( void )
{
	const s32 channel = m_Channel;
	// CTR側のプログラムの接続を待ちます。
	gfl::core::Debug::PrintConsole( "wait %d\n", channel );
	QString wait("\nWait " + QString::number(m_Channel) );
	if(!m_LogMessage.contains(wait)){
		m_LogMessage += wait;
	}
	s32 ret = CallHio(WaitSio, channel, 0 );
//	s32 ret = CallHio(ConnectSio, channel );
	if( ret != 1 ){
		gfl::core::Debug::PrintConsole( "WaitSio(%d) ERROR(code:%08x)\n",channel, ret );
		CallHio(CloseSio, channel );
		m_Step = INIT;
		return false;
	}
	m_Step = CONNECTED;
	gfl::core::Debug::PrintConsole( "connected %d\n",channel);
	m_LogMessage += "\nConnected " + QString::number(m_Channel);
	return true;
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::hio::ClientBase::Reset()
{
	if(CONNECTED <= m_Step){
//		m_Step = WAIT_FOR_CONNECT;
		m_Step = RESET;
	}
}
