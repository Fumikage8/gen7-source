//=============================================================================
/**
 * @brief �T�E���h�q�[�v
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
       * @brief �R���X�g���N�^
       * @param heap_for_system     �e��C���X�^���X�̐����Ɏg�p����q�[�v
       * @param heap_for_sound_heap �T�E���h�q�[�v�̊m�ۂɎg�p����f�o�C�X�q�[�v
       * @param sound_heap_size     �m�ۂ���T�E���h�q�[�v�̃T�C�Y
       */
      //-----------------------------------------------------------------------
      SoundHeap( 
          gfl::heap::HeapBase* heap_for_system,
          gfl::heap::HeapBase* heap_for_sound_heap,
          u32 sound_heap_size );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~SoundHeap();

      //-----------------------------------------------------------------------
      /**
       * @brief NW�̃T�E���h�q�[�v�I�u�W�F�N�g���擾����
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
