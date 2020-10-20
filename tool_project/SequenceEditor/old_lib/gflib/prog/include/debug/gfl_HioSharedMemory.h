//=============================================================================================
/**
 * @file	  gfl_HioSharedMemory.h
 * @author	soga
 * @brief	  HIOのSharedMemoryを使用した通信
 * @date    2013.04.13
 */
//=============================================================================================

#ifndef __HIO_SHAREDMEMORY_H__
#define __HIO_SHAREDMEMORY_H__
#pragma once

#if GFL_HIO_ENABLE

#include <nn/hio.h>
#include <gfl_Common.h>
#include <base/gfl_Singleton.h>

namespace gfl{  
namespace hio{  
  class HioSharedMemory : public  gfl::base::Singleton<HioSharedMemory>
  { 
    GFL_FORBID_COPY_AND_ASSIGN( HioSharedMemory );

    private:
    nn::hio::CTR::SharedMemoryChannel*  mSharedMemory;
    void* mWorkMemory;
    s32   mWritePosTop;
    s32   mWritePos;

    public:
    HioSharedMemory( gfl::heap::HeapBase* appHeap, gfl::heap::HeapBase* devHeap, int ch );
    ~HioSharedMemory();

    void        Initialize( void );
    nn::Result  Open( int ch, void* pWorkMemory );
    nn::Result  Close( void );
    size_t      GetSpaceSize();
    nn::Result  Read( void* read_buf, size_t length, int offset );
    nn::Result  Write( const void* write_buf, size_t length, int offset );
    nn::Result  WriteRingBuffer( const void* write_buf, size_t length, const void* to_top_buf, size_t to_top_length, bool* to_top_result );
    bool        SetWritePosTop( s32 value );
  };
} //namespace hio
} //namespace gfl

/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( gfl::hio::HioSharedMemory );

#endif

#endif  //__HIO_SHAREDMEMORY_H__
