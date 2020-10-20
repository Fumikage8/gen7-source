//=============================================================================
/**
 * @brief サウンドヒープ
 * @file gfl2_SoundHeap.cpp
 * @author Miyuki Iwasawa
 * @date 2015.02.19
 */
//=============================================================================
#include <nw/snd.h>
#include "heap/include/gfl2_Heap.h"
#include "math/include/gfl2_Math.h"
#include "sound/include/gfl2_SoundHeap.h"

namespace gfl2 {
  namespace sound {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_system     各種インスタンスの生成に使用するヒープ
     * @param heap_for_sound_heap サウンドヒープの確保に使用するデバイスヒープ
     * @param sound_heap_size     確保するサウンドヒープのサイズ
     */
    //-------------------------------------------------------------------------
    SoundHeap::SoundHeap( 
        gfl2::heap::HeapBase* heap_for_system,
        gfl2::heap::HeapBase* heap_for_sound_heap,
        u32 sound_heap_size ) :
      m_buffer( NULL ),
      m_bufferSize( 0 ),
      m_nwSoundHeap()
    {
      m_bufferSize = sound_heap_size;
      m_buffer = GflHeapAllocMemory( heap_for_sound_heap, sound_heap_size );
      bool result = m_nwSoundHeap.Create( m_buffer, m_bufferSize );
      GFL_ASSERT( result ); //@fix
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    SoundHeap::~SoundHeap()
    {
      m_nwSoundHeap.Destroy();
      GflHeapFreeMemory( m_buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief NWのサウンドヒープオブジェクトを取得する
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundHeap* SoundHeap::GetNwSoundHeap( void )
    {
      return &m_nwSoundHeap;
    }


  } //namespace sound 
} //namespace gfl2
