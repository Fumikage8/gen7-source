//==============================================================================
/**
 * @file	gfl_Client.h
 * 	
 * @author	kimura_shin
 * @data	2011/06/07, 18:14
 */
// =============================================================================

#ifndef GFL_CLIENT_H
#define	GFL_CLIENT_H

#include "gfl_QtGui.h"

// このファイルは
// Program Files (x86)/KMC/CTRHOSTIO/SDK
// へのインクルードパスが必要！

#ifdef GFL_PLATFORM_PC
#define CTRHOSTIO_COMPATIBLE
#include <devhio_host.h>
#include <devhio_sio.h>
#include <devhio_smem.h>
#include <CDevHIO.h>
#include <conio.h>
#include <ISCTRDLL.h>

#include <debug/gfl_HioConfig.h>

#include <qt/gfl_ClientBase.h>
#include <qt/gfl_ClientRead.h>
#include <qt/gfl_ClientWrite.h>

namespace gfl {
namespace hio {

class Client : public QThread {
public:
	Client( const s32 FromHostChannel = GFL_HIO_CHANNEL_FROM_3DS, const s32 ToHostChannel = GFL_HIO_CHANNEL_TO_3DS );
	~Client( );

	ClientWrite* GetClientWrite( ) const {
		return m_pClientWrite;
	}

	ClientRead* GetClientRead( ) const {
		return m_pClientRead;
	}

	bool IsReady( void ) {
		if( m_pClientRead && m_pClientWrite ){
			return m_pClientRead->IsConnected( ) && m_pClientWrite->IsConnected( );
		}
		return false;
	}

	template<typename T>
	void Update( T* cls ) {
		if( IsReady( ) ){
			//			GetClientRead( )->GetMutex( )->lock( );
			QVector<QString*>* dat = GetClientRead( )->GetData( );
			for( s32 i = 0; i < dat->size( ); ++i ){
				//				gfl::core::Debug::PrintConsole( "[%d] %s\n",i,dat->at( i )->toAscii().data( ) );
				gfl::rtti::CallFunction( cls, (void*) dat->at( i )->toAscii( ).data( ) );
			}
			GetClientRead( )->DataClear( );
			//			GetClientRead( )->GetMutex( )->unlock( );
		}
	}

	bool SendFileName( const QString& fname, const QString& com = "LoadPcFile " );

	void SetSelectDeviceId( s32 SelectDeviceId ) {
		this->m_SelectDeviceId = SelectDeviceId;
	}

	s32 GetDeviceIdMax( ) const {
		return m_DeviceIdMax;
	}

	unsigned long* GetDeviceId( ) {
		return m_DeviceId;
	}

	s32 GetSelectDeviceId( ) const {
		return m_SelectDeviceId;
	}

private:
	static const s32 DEVICE_ID_MAX = 16;
	virtual void run( void );

	unsigned long m_DeviceId[DEVICE_ID_MAX];
	ClientRead* m_pClientRead;
	ClientWrite* m_pClientWrite;
	s32 m_FromHostChannel;
	s32 m_ToHostChannel;
	s32 m_Ready;
	s32 m_DeviceIdMax;
	s32 m_SelectDeviceId;
};

}
}

#endif

#endif	/* GFL_CLIENT_H */
