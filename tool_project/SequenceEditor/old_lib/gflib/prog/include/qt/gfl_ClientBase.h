//==============================================================================
/**
 * @file	ClientBase.h
 * 	クライアント基底クラス
 * @author	kimura_shin
 * @data	2011/03/28, 10:26
 */
// =============================================================================

#ifndef CLIENTBASE_H
#define	CLIENTBASE_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace hio {

class ClientBase : public QThread {
protected:
	// 複数スレッドから呼ばれるのにスタティックでいいのか？ライブラリ内でちゃんと処理してると信じることにする。
	static ::CDEVHIO s_HostIo;
	static CISCTRDLL s_ISCTR;
	//	QMutex m_Mutex;
	QVector<QString*> m_Data;
	QString m_ErrorMessage;
	QString m_LogMessage;
	size_t* m_pSize;
	s32 m_Channel;
	s32 m_Step;
	static b32 s_IsISCTR;

	bool WaitForConnect( void );
	void ResetHio( const s32 channel );

public:
	void Initialize( void );
	ClientBase( const s32 channel );

	virtual ~ClientBase( void );

	bool IsConnected( void ) {
		return m_Step == CONNECTED;
	}

	bool IsTerminated( void ) {
		return m_Step == TERMINATE;
	}

	QVector<QString*>* GetData( ) {
		return &m_Data;
	}

	void DataClear( void ) {
		for( s32 i = 0; i < m_Data.size( ); ++i ){
			GFL_DELETE m_Data.at( i );
		}
		if(m_Data.size()){
			m_Data.clear( );
		}
	}

	void Reset( void );

#if 0 // 

	bool Reset( void ) {
		if( IsTerminated( ) ){
			m_Step = WAIT_FOR_CONNECT;
			return true;
		}
		return false;
	}
#endif
	//    QMutex* GetMutex( ){
	//  	return &m_Mutex;
	//}

	static ::CDEVHIO* GetHostIo( ) {
		return &s_HostIo;
	}

	QString* GetErrorMessage( ) {
		return &m_ErrorMessage;
	}

	QString* GetLogMessage( ) {
		return &m_LogMessage;
	}

    s32 GetStep( ) const {
    	return m_Step;
    }

    static CISCTRDLL* GetISCTR( ){
    	return &s_ISCTR;
    }

    static b32 IsISCTR( ){
    	return s_IsISCTR;
    }

    static void SetIsISCTR( b32 IsISCTR ) {
    	s_IsISCTR = IsISCTR;
    }
};

#define CallHio( func, ... )	(IsISCTR()?GetISCTR()->func(__VA_ARGS__):GetHostIo()->func(__VA_ARGS__))

}
}

#endif	/* CLIENTBASE_H */

