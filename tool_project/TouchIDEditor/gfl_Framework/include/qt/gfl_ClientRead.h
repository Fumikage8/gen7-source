//==============================================================================
/**
 * @file	ClientRead.h
 * 	読み込みクライアントスレッド
 * @author	kimura_shin
 * @data	2011/03/25, 16:29
 */
// =============================================================================

#ifndef CLIENTREAD_H
#define	CLIENTREAD_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace hio {

class ClientWrite;

class ClientRead : public ClientBase {
private:
	ClientWrite* m_pClientWrite;
	s32 m_Connected;

	virtual void run( void );

	bool Read( void* buffer, const size_t size );
	void ReadData( const size_t size );
	//	size_t ReadSize( void );

public:

	ClientRead( const s32 channel = GFL_HIO_CHANNEL_FROM_3DS ) : ClientBase( channel ), m_pClientWrite( 0 ), m_Connected( 0 ) {
	}

	~ClientRead( void ) {
	}

	void SetClientWrite( ClientWrite* ClientWrite ) {
		this->m_pClientWrite = ClientWrite;
	}

	ClientWrite* GetClientWrite( ) const {
		return m_pClientWrite;
	}

	void SetConnected( s32 Connected ) {
		this->m_Connected = Connected;
	}

	s32 GetConnected( ) const {
		return m_Connected;
	}
};

}
}

#endif	/* CLIENTREAD_H */
