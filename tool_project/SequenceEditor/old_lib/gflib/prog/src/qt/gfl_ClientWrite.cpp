//==============================================================================

#include "qt/gfl_ClientBase.h"
#include "qt/gfl_ClientWrite.h"
#include "core/gfl_Utility.h"
#include "qt/Shader/CombinerAttribute.h"

/**
 * @file	ClientWrite.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/28, 10:37
 */
// =============================================================================

//==============================================================================
//==============================================================================
void gfl::hio::ClientWrite::run( void )
{
	for(;m_Step != RESET; ){
		switch( m_Step ){
		case INIT:
			Initialize();
			break;

		case WAIT_FOR_CONNECT:
//			gfl::core::Debug::PrintConsole( "wait begin\n");
			DataClear();
			if(!WaitForConnect( )){
				break;
			}

		case CONNECTED:
//			gfl::core::Debug::PrintConsole( "connect lock begin\n");
//			m_Mutex.lock();
//			gfl::core::Debug::PrintConsole( "connect lock end\n");
			if( m_Data.size( ) ){
//				gfl::core::Debug::PrintConsole( "size %d\n",m_Data.size());
				QByteArray dat(m_Data.at( 0 )->toAscii());
				*m_pSize = dat.size( ) + 1;

				// 先にデータサイズを送る
//				gfl::core::Debug::PrintConsole( "write size %d\n",size);
//				if( Write( ( void* )m_pSize, sizeof(size_t ) ) ){
//					gfl::core::Debug::PrintConsole( "write data\n");
					// 次にデータの実体を送る
					if(m_pWriteBuffer){
						_aligned_free(m_pWriteBuffer);	
					}
					const size_t size = *m_pSize;
					u8* buf = (u8*)_aligned_malloc( size +1, GFL_HIO_ALIGN );
					m_pWriteBuffer = buf;
					memcpy(buf,dat.data(),size);
//					gfl::core::Debug::PrintConsole( "write %s\n",buf );
					Write( buf, size );
//					if( Write( ( void* )dat.data( ), size ) ){
//						gfl::core::Debug::PrintConsole( "write end\n");
//						if( 0 == strcmp( s_pTerminationString, dat.data() ) ){
//							m_Step = TERMINATE;
//						}
					GFL_DELETE m_Data.at(0);
					m_Data.pop_front();
//				}
			}
//			m_Mutex.unlock();
		}
//		gfl::core::Time::SleepMilliSecond(gfl::core::Time::GFL_MILLI_SECOND/GFL_FPS);
	}
}

//==============================================================================
//==============================================================================
bool gfl::hio::ClientWrite::Write( void* buffer, const size_t size )
{
	const s32 channel = m_Channel;
	GFL_ASSERT_MSG( !(((size_t)buffer) & 0x1f), "write align error" );
//	u32* buf_u32 = (u32*)buffer;
//	buf_u32[0] = gfl::core::Utility::CalcHash( (const c8*)&buf_u32[1] );
	// CTR側のプログラムへデータを送信します。
	int writesize = CallHio(WriteSio, channel, buffer, (int)size, 0 );
	if( writesize < 0 ){
		QString buf("WriteError " + QString::number(channel) + "\n");
		if(!m_ErrorMessage.contains(buf)){
			gfl::core::Debug::PrintConsoleDirect( buf.toAscii( ).data( ) );
			m_ErrorMessage += buf;
		}
		CallHio(DisconnectSio, channel );
//			m_Step = WAIT_FOR_CONNECT;
		gfl::core::Debug::PrintConsole( "dissconnect\n" );
		CallHio(CloseSio, channel );
//		m_Step = INIT;
		m_Step = RESET;
		gfl::core::Debug::PrintConsole( "closed\n" );
		GetClientRead()->Reset();
		//		CallHio(CloseSio, channel );
		return false;
	}
//	gfl::core::Debug::PrintConsole( "write(%d)%s size: %d\n", channel,buffer, writesize );
	s32 ret = CallHio(FlushSio, channel );
	if( ret != 0 ){
		gfl::core::Debug::PrintConsole( "FlushSio(%d) ERROR(code:%08x)\n",channel, ret );
		QString buf("FlushError " + QString::number(channel) + "\n");
		if(!m_ErrorMessage.contains(buf)){
			m_ErrorMessage += buf;
		}
		m_Step = WAIT_FOR_CONNECT;
//		CallHio(CloseSio( channel );
		CallHio(DisconnectSio, channel );
		GetClientRead()->Reset();
		return false;
	}
#if 0 // def ENSURE_READ
	// 送られたか確認
	u8* buf = (u8*)_aligned_malloc( 1, GFL_HIO_ALIGN );
	buf[0] = 0;
	gfl::core::Debug::PrintConsole( "wait answer\n" );
	CallHio(ReadSio, channel, buf, 1, 0, /* DEVHIO_SIO_ATTR_NONE */ );
	u8 ret_u8 = buf[0];
	_aligned_free(buf);
	if(!ret_u8){
		gfl::core::Debug::PrintConsole( "ReWrite\n" );
		gfl::core::Time::SleepMilliSecond(gfl::core::Time::GFL_MILLI_SECOND/GFL_FPS);
		Write(buffer,size);		
	}
#endif
	//	gfl::core::Debug::PrintConsole( "flush %d\n", channel );
	return true;
}
#if 0 // 
//==============================================================================
//==============================================================================
bool gfl::hio::ClientWrite::SendTerminationToHost( void )
{
	// 書き込み待ちか？
	if( m_Data.isEmpty( ) ){
		if( m_pClientRead ){
			m_pClientRead->SetClientWrite( 0 ); // ループ回避
		}
		SendData( s_pTerminationString );
		return true;
	}
	return false;
}
#endif
