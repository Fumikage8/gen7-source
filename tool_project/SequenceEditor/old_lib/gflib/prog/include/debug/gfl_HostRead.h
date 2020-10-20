//==============================================================================
/**
 * @file	HostRead.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/24, 18:48
 */
// =============================================================================

#ifndef HOSTREAD_H
#define	HOSTREAD_H
#pragma once

#include <debug/gfl_HostBase.h>


namespace gfl { namespace hio {

class HostWrite;

class HostRead : public HostBase {
private:
	s32 Read( void* buffer, const size_t size );
//	s32 ReadSize( void ){
//		return Read( m_pSize, sizeof(size_t) );
//	}

	HostWrite* m_pHostWrite;
	size_t m_AllocSize;

public:

	HostRead( gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap, const s32 channel_id = GFL_HIO_CHANNEL_TO_3DS
			 , const u32 stackSize = GFL_DEFAULT_THREAD_STACK_SIZE )
	: HostBase( heap, dev_heap, channel_id, stackSize ), m_pHostWrite( 0 ), m_AllocSize( 0 ) {
	}

	virtual ~HostRead( void );

	virtual void Main( void );

	void Clear( void );
	
	void RestartRead(void){
//		Clear();
		Signal();
	}

	void SetHostWrite( HostWrite* HostWrite ) {
		this->m_pHostWrite = HostWrite;
	}

	HostWrite* GetHostWrite( ) const {
		return m_pHostWrite;
	}
};

} }

#endif	/* HOSTREAD_H */

