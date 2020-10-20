//=============================================================================
/**
 * @brief �T�E���h�q�[�v
 * @file gfl_SoundHeap.cpp
 * @author obata_toshihiro
 * @date 2012.02.25
 */
//=============================================================================
#include <nw.h>
#include "gfl_Heap.h"
#include "gfl_Math.h"
#include "snd/gfl_SoundHeap.h"

namespace gfl {
  namespace snd {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap_for_system     �e��C���X�^���X�̐����Ɏg�p����q�[�v
     * @param heap_for_sound_heap �T�E���h�q�[�v�̊m�ۂɎg�p����f�o�C�X�q�[�v
     * @param sound_heap_size     �m�ۂ���T�E���h�q�[�v�̃T�C�Y
     */
    //-------------------------------------------------------------------------
    SoundHeap::SoundHeap( 
        gfl::heap::HeapBase* heap_for_system,
        gfl::heap::HeapBase* heap_for_sound_heap,
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
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    SoundHeap::~SoundHeap()
    {
      m_nwSoundHeap.Destroy();
      GflHeapFreeMemory( m_buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief NW�̃T�E���h�q�[�v�I�u�W�F�N�g���擾����
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundHeap* SoundHeap::GetNwSoundHeap( void )
    {
      return &m_nwSoundHeap;
    }


  } //namespace snd
} //namespace gfl
