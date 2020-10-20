//=============================================================================
/**
 * @brief サウンドヒープ
 * @file gfl2_SoundHeap.h
 * @author Miyuki Iwasawa
 * @date 2015.02.19
 */
//=============================================================================
#ifndef __GFL_SOUNDHEAP_H__
#define __GFL_SOUNDHEAP_H__
#pragma once

#include <nw/snd.h>
#include "heap/include/gfl2_Heap.h"
#include "math/include/gfl2_Math.h"

namespace gfl2 {
  namespace sound {


    class SoundHeap
    {
      GFL_FORBID_COPY_AND_ASSIGN( SoundHeap );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap_for_system     各種インスタンスの生成に使用するヒープ
       * @param heap_for_sound_heap サウンドヒープの確保に使用するデバイスヒープ
       * @param sound_heap_size     確保するサウンドヒープのサイズ
       */
      //-----------------------------------------------------------------------
      SoundHeap( 
          gfl2::heap::HeapBase* heap_for_system,
          gfl2::heap::HeapBase* heap_for_sound_heap,
          u32 sound_heap_size );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~SoundHeap();

      //-----------------------------------------------------------------------
      /**
       * @brief NWのサウンドヒープオブジェクトを取得する
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundHeap* GetNwSoundHeap( void );


      private: 
      void* m_buffer;
      u32 m_bufferSize;
      nw::snd::SoundHeap m_nwSoundHeap;
    };


  } //namespace sound
} //namespace gfl2

#endif //__GFL_SOUNDHEAP_H__
