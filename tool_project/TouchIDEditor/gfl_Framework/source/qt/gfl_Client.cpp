//==============================================================================

#include "qt/gfl_ClientBase.h"
#include "core/gfl_Time.h"
#include "qt/gfl_Client.h"

/**
 * @file	gfl_Client.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/06/24, 15:26
 */
// =============================================================================
gfl::hio::Client::Client( const s32 FromHostChannel, const s32 ToHostChannel )
: m_pClientRead(0), m_pClientWrite(0), m_FromHostChannel( FromHostChannel ), m_ToHostChannel( ToHostChannel ), m_Ready( false )
, m_DeviceIdMax(0), m_SelectDeviceId(-1)
{
	::CDEVHIO();
	start( );
}
gfl::hio::Client::~Client( void )
{
	delete m_pClientRead;
	delete m_pClientWrite;
	if( !IsReady( ) ){
		terminate();
		wait();
	}
}
void gfl::hio::Client::run( void )
{
	for(;; ){
		gfl::core::Time::SleepMilliSecond(33);

		// デバッガを検索して接続する
//		gfl::core::Debug::PrintConsole("START\n");
		m_DeviceIdMax = ClientBase::GetHostIo( )->Find( m_DeviceId, DEVICE_ID_MAX );
		if( m_DeviceIdMax <= 0 ){
			//			gfl::core::Debug::PrintConsole( "NO DEVICES FOUND\n" );
			continue;
		}
		
		s32 id;
		if( 1 < m_DeviceIdMax ){
			while(m_SelectDeviceId == -1){
				gfl::core::Time::SleepMilliSecond(33);
			}
			id = m_SelectDeviceId;
		}else{
			id = 0;
		}
		
		bool ISCTR_flag = false;
		{
			CISCTRDLL* isctr = ClientBase::GetISCTR();
			bool ret = isctr->Open();
			if(ret){
				ISDDEVICEID dev = isctr->GetDeviceID();
				if(dev.eType == ISDDEV_ISCTRBOX){
					ISCTR_flag = true;
					gfl::core::Debug::PrintConsole( "IS CTR\n" );
				}
			}
		}
		ClientBase::SetIsISCTR(ISCTR_flag);
		
		if(ISCTR_flag){ // ISCTR
			if(!ClientBase::GetISCTR()->EnableHIO()){
				continue;
			}
		}else{
			if( ClientBase::GetHostIo( )->Connect( m_DeviceId[id] ) < 0 ){
				//			gfl::core::Debug::PrintConsole( "CONNECT ERROR\n" );
				continue;
			}
		}

		m_pClientRead = new gfl::hio::ClientRead( m_FromHostChannel );
		m_pClientWrite = new gfl::hio::ClientWrite( m_ToHostChannel );
		
		m_pClientRead->SetClientWrite( m_pClientWrite );
		m_pClientWrite->SetClientRead( m_pClientRead );
		
		m_pClientRead->start( );
		m_pClientWrite->start( );

//		gfl::core::Debug::PrintConsole( "end of client\n");

		for(;;){ // それぞれのスレッドが走っているけど、まだ死なないよ。
			gfl::core::Time::SleepMilliSecond(33);
			if( m_pClientRead->isFinished()
				&& m_pClientWrite->isFinished() ){
				delete m_pClientRead;
				m_pClientRead = 0;
				delete m_pClientWrite;
				m_pClientWrite = 0;
				break;
			}
		}
	}
//	m_Ready = true;
}
//==============================================================================
//==============================================================================
bool gfl::hio::Client::SendFileName( const QString& fname, const QString& com )
{
	if( !fname.isNull( ) ){
		gfl::hio::ClientRead* cr = GetClientRead( );
		gfl::hio::ClientWrite* cw = GetClientWrite( );

		if( !cw || !cw->IsConnected( ) || !cr || !cr->IsConnected( ) ){
			return false;
		}

		cw->SendData( ( com + QDir::cleanPath( QDir::current( ).absoluteFilePath( fname ) ) ).toAscii( ) );
		return true;
	}
	return false;
}
