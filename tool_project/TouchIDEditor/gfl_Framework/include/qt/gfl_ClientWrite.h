//==============================================================================
/**
 * @file	ClientWrite.h
 * 	書き込みクライアントスレッド
 * @author	kimura_shin
 * @data	2011/03/28, 10:23
 */
// =============================================================================

#ifndef CLIENTWRITE_H
#define	CLIENTWRITE_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace hio {

class ClientWrite : public ClientBase {
private:
	ClientRead* m_pClientRead;
	u8* m_pWriteBuffer;

	virtual void run( void );
	bool Write( void* buffer, const size_t size );

public:

	ClientWrite( const s32 channel = GFL_HIO_CHANNEL_TO_3DS ) : ClientBase( channel ), m_pClientRead( 0 ), m_pWriteBuffer( 0 ) {
	}

	virtual ~ClientWrite( void ) {
		DataClear( );
		_aligned_free( m_pWriteBuffer );
		m_pWriteBuffer = 0;
	}

	void SendData( const QString& data ) {
		//		m_Mutex.lock();
		//		m_Data.push_back( data );
		m_Data.push_back( DbgNew QString( data ) );
		//		m_Mutex.unlock();
	}
	// 終了処理をホストに送る
	//	bool SendTerminationToHost( void );

	void SetClientRead( ClientRead* ClientRead ) {
		this->m_pClientRead = ClientRead;
	}

	void Disconnect( void ) {
		s_HostIo.DisconnectSio( m_Channel );
		m_Step = WAIT_FOR_CONNECT;
	}

	ClientRead* GetClientRead( ) const {
		return m_pClientRead;
	}
};

}
}

#endif	/* CLIENTWRITE_H */
