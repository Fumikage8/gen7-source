//==============================================================================
/**
 * @file	HostBase.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/28, 10:51
 */
// =============================================================================

#ifndef HOSTBASE_H
#define	HOSTBASE_H
#pragma once

namespace gfl { namespace hio {

class HostBase : public gfl::base::Thread {
protected:

	enum {
		DEFAULT_THREAD_STACK_SIZE = 0x1000,
		HIO_WAIT_TIME = 30
	};

	bool WaitForConnect( void );

	// CreateInstance Ç≈ÇÃÉGÉâÅ[ÇñhÇÆÇΩÇﬂ

	HostBase( void ) : gfl::base::Thread( 0, 0 ) {
	}

	nn::hio::CTR::SerialChannel m_Channel;
	u8* m_pData;
	gfl::heap::HeapBase* m_pHeapBase;
	gfl::heap::HeapBase* m_pDeviceHeap;

//	gfl::core::AtomicInteger m_DataSize;
	s32 m_DataSize;
//	gfl::core::AtomicInteger m_Step;
	s32 m_Step;
	size_t m_Size;
	s32 m_ChannelId;
//	u8 m_Opened;
	u8 m_Connected;

public:

	HostBase( gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap, const s32 channel, const u32 stackSize = DEFAULT_THREAD_STACK_SIZE );
	virtual ~HostBase( void );

	virtual void Initialize( void );
	virtual void Finalize( void ){
#if USE_GFL_NEW
		GFL_DELETE this;
#else
		delete this;
#endif
	}
	void FinalizeChannel( void );
	void Disconnect(void){
		m_Channel.Disconnect( );
	}
	void Flush(void){
		m_Channel.Flush( );
	}

	u8* GetData( ) const {
		return m_pData;
	}

	s32 GetDataSize( ){
		return m_DataSize;
	}

	bool IsConnected( void ) {
		return m_Step == CONNECTED;
	}
	void SetTerminated(void){
		m_Step = TERMINATE;
	}

	void Update(void){
		if(m_Step == WAIT_FOR_CONNECT){
			Signal();
		}
	}
	void Close(void){
		if(WAIT_FOR_CONNECT <= m_Step){
			m_Channel.Close();
		}
	}
};

} }

#endif	/* HOSTBASE_H */

