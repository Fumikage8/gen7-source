//==============================================================================

#include "qt/gfl_ClientBase.h"
#include "qt/gfl_ClientRead.h"

/**
 * @file	ClientRead.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/25, 16:34
 */
// =============================================================================

//==============================================================================
//==============================================================================
void gfl::hio::ClientRead::run( void )
{
	for(; m_Step != RESET ; ){
		switch( m_Step ){
		case INIT:
			Initialize();
			break;
		case WAIT_FOR_CONNECT:
			if(!WaitForConnect( )){
				break;
			}

		case CONNECTED:
		{
//			s32 size = (s32)ReadSize( );
			s32 size = CallHio(GetReadableBytesSio, m_Channel, 0 );
			if( 0 < size ){
//				gfl::core::Debug::PrintConsole( "readsize: %d\n", size );
				ReadData( size );
			}
		}
		}
//		gfl::core::Time::SleepMilliSecond(gfl::core::Time::GFL_MILLI_SECOND/GFL_FPS);
	}
}

#if 0 // 
//==============================================================================
//==============================================================================
size_t gfl::hio::ClientRead::ReadSize( void )
{
#if 0 // 
	size_t size;
	Read( static_cast < void* >( &size ), sizeof(size_t ) );
	return size;
#else // 
	Read( static_cast < void* >( m_pSize ), sizeof(size_t ) );
	return *m_pSize;
#endif
}
#endif

//==============================================================================
//==============================================================================
bool gfl::hio::ClientRead::Read( void* buffer, const size_t size )
{
	const s32 channel = m_Channel;

	// CTR側からのデータを受信します。
//	gfl::core::Debug::PrintConsole( "wait for reading %d\n", channel );
	int readsize = (s32)CallHio(ReadSio, channel, buffer, (s32)size, 0 );
	//	gfl::core::Debug::PrintConsole( "read size %d\n",size);
	if( readsize < 0 ){
#if 0 // 
		//		gfl::core::Debug::PrintConsole( "ReadSio() ERROR(code:%08x)\n", readsize );
		CallHio(DisconnectSio, channel );
#if 1 // 
		if(m_pClientWrite){
			m_pClientWrite->Disconnect();
		}
#endif
#endif
		m_ErrorMessage += "ReadError " + QString::number(channel) + "\n";
		m_Step = WAIT_FOR_CONNECT;
		GetClientWrite()->Reset();
		//				CallHio(CloseSio, channel );
		return false;
	}
	return true;
}
//==============================================================================
//==============================================================================
void gfl::hio::ClientRead::ReadData( const size_t size )
{
	// CTR側からのデータを受信します。
#if 0
	QByteArray dat;
	dat.resize( size + 1 );
	Read( dat.data( ), size );
	m_Data.push_back( dat );
#else // 
	void* buf = _aligned_malloc( size+1, GFL_HIO_ALIGN );
	Read( buf, size );
	
	if(((c8*)buf)[0] == '!'){
		m_Connected = true;
	}else{
		m_Data.push_back( DbgNew QString((const c8 *)buf ));
	}
	_aligned_free(buf);
#endif
//	m_Mutex.unlock( ); // for terminate check
}

