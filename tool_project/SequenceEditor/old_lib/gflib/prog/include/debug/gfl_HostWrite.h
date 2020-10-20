//==============================================================================
/**
 * @file	HostWrite.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/28, 10:51
 */
// =============================================================================

#ifndef HOSTWRITE_H
#define	HOSTWRITE_H
#pragma once

#include <debug/gfl_HioConfig.h>


namespace gfl { namespace hio {

class HostWrite : public HostBase {
private:
	void Write( const void* buffer, const size_t size );

	c8* m_pWriteBuffer;
	HostRead* m_pHostRead;
	s32 m_BufferSize;

public:

	enum {
		DEFAULT_WRITE_BUFFER_SIZE = 0x400
	};

	HostWrite( gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap,  const s32 channel_id = GFL_HIO_CHANNEL_FROM_3DS
			 , const u32 buffer_size = DEFAULT_WRITE_BUFFER_SIZE, const u32 stackSize = GFL_DEFAULT_THREAD_STACK_SIZE );

	virtual ~HostWrite( void );

	virtual void Main( void );

	c8* GetWriteBuffer( ) const {
		return m_pWriteBuffer;
	}

	// クライアントに送る
//	void SendStringToClient( const c8* const str );
	void SetData( c8* Data ) {
		this->m_pData = (u8*)Data;
		this->m_DataSize = strlen(Data) + 1;
		Signal();
	}

	void SetHostRead( HostRead* HostRead ) {
		this->m_pHostRead = HostRead;
	}

	HostRead* GetHostRead( ) const {
		return m_pHostRead;
	}
};

} }

#endif	/* HOSTWRITE_H */

