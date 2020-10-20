/**
 * @file	gfl_Dummy.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/05/23, 12:46
 */

#ifndef GFL_DUMMY_H
#define	GFL_DUMMY_H
#pragma once

#ifndef GFL_PLATFORM_3DS

// PCで動かすためのダミークラス

namespace gfl {

// PROC ========================================================================

GFL_NAMESPACE_BEGIN( proc )

class BaseProcess {
};

class Manager {
};

enum Result {
	RES_CONTINUE,
	RES_FINISH
};

#define GFL_FORBID_COPY_AND_ASSIGN(name)

GFL_NAMESPACE_END( proc )

// STD =========================================================================

GFL_NAMESPACE_BEGIN( std )
static void MemFill( void* dst, int fill, size_t size ) {
	memset( dst, fill, size );
}
//

static void MemCopy( const void* src, void* dst, size_t size ) {
	memcpy( dst, src, size );
}

GFL_NAMESPACE_END( std )

// HEAP ========================================================================
GFL_NAMESPACE_BEGIN( heap )

class HeapBase {
};

GFL_NAMESPACE_END( heap )

// The end of gfl
}

#define GFL_NEW( mem ) DbgNew
#define GFL_NEW_ARRAY( mem ) DbgNew
#define GFL_DELETE delete
#define GFL_DELETE_ARRAY delete [] 
// コピーコンストラクタ＋代入演算子禁止
#define GFL_FORBID_COPY_AND_ASSIGN( cls )
#define GflHeapAllocMemoryAlign(heapClass,size,align) _aligned_malloc( size,align )
#define GflHeapFreeMemory(buf) _aligned_free(buf)
class GameManager {
};

#endif

#endif	/* GFL_DUMMY_H */

