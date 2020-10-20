//==============================================================================
/**
 * @file	Host.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/06/07, 18:09
 */
// =============================================================================

#ifndef HOST_H
#define	HOST_H
#pragma once

#include <gfl_fs.h>

#if GFL_HIO_ENABLE

#include <gfl_Core2.h>

#include <debug/gfl_HioConfig.h>
#include <debug/gfl_HostBase.h>
#include <debug/gfl_HostRead.h>
#include <debug/gfl_HostWrite.h>

namespace gfl { namespace hio {

class Host {
	GFL_FORBID_COPY_AND_ASSIGN( Host );
public:
	GFL_SINGLETON(Host)
	inline static Host* CreateInstance(gfl::heap::HeapBase* heap,gfl::heap::HeapBase* dev_heap)
	{ 
		Host* ins = _spInstance; 
		if( !ins ){ 
			_spInstance = ins = GFL_NEW(heap) Host(heap,dev_heap); 
		} 
		return ins; 
	}
	enum {
		READ_THREAD_PRIORITY = (gfl::base::Thread::DefaultPriority + gfl::base::Thread::LowestPriority) >> 1,
		WRITE_THREAD_PRIORITY
	};
	//　チャンネルとかスタックとかにも対応すべきか
	Host( gfl::heap::HeapBase* heap ){ GFL_ASSERT_MSG(0,"HIO host needs device memory!!"); }
	Host( gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap );
	~Host( void );

	HostWrite* GetHostWrite( ) const {
		return m_pHostWrite;
	}

	HostRead* GetHostRead( ) const {
		return m_pHostRead;
	}

	static Host* Update();
	
	/**
	 * 終了リクエスト発行
     */
	void Kill(void);
	
	/**
	 * 終了したのか
     * @return 真　終了した　偽　まだ生きてる
     */
	bool IsTerminated(void);

	//	static s32 IsInitialized(void) {
	//		return s_Initialized;
	//	}
	

private:
	void UpdateReadWrite( void ) {
		if( m_pHostRead ){
			m_pHostRead->Update( );
		}
		if( m_pHostWrite ){
			m_pHostWrite->Update( );
		}
	}

	HostRead* m_pHostRead;
	HostWrite* m_pHostWrite;
	//	static s32 s_Initialized;
};

} }

#endif

#endif	/* HOST_H */

