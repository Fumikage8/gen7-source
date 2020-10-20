//=============================================================================
/**
 * @brief サウンドヒープ
 * @file gfl_SoundHeap.h
 * @author obata_toshihiro
 * @date 2012.02.24
 */
//=============================================================================
#ifndef __GFL_SOUNDHEAP_H__
#define __GFL_SOUNDHEAP_H__
#pragma once

#include <nw.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>

namespace gfl {
  namespace snd {


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
          gfl::heap::HeapBase* heap_for_system,
          gfl::heap::HeapBase* heap_for_sound_heap,
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


  } //namespace snd
} //namespace gfl

#endif //__GFL_SOUNDHEAP_H__
