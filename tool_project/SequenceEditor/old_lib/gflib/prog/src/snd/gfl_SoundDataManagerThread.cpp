//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_SoundDataManagerThread.cpp
 *	@brief  �T�E���h�f�[�^�}�l�[�W���@�f�[�^�ǂݍ��݁@�X���b�h
 *	@author	tomoya takahashi
 *	@date		2012.12.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nw.h>
#include <snd/gfl_SoundSystem.h>
#include "gfl_SoundDataManagerThread.h"

namespace gfl {
  namespace snd {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

    // �X���b�h�̃X�^�b�N�T�C�Y
    const u32 SoundDataManagerThread::STACK_SIZE = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap_for_thread �X���b�h�̓���Ɏg�p����q�[�v
     * @param desc            �A�[�J�C�u�ǂݍ��݂̏ڍ׏��
     */
    //-------------------------------------------------------------------------
    SoundDataManagerThread::SoundDataManagerThread( 
        gfl::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      base::Thread( heap_for_thread, STACK_SIZE ),
      m_desc( desc )
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    SoundDataManagerThread::~SoundDataManagerThread()
    {
    }



    /**
     * @brief ���C������
     * @note gfl::base::Thread ���I�[�o�[���C�h
     */
    void SoundDataManagerThread::Main( void )
    {
      *m_desc.m_pResult = m_desc.m_pSoundSystem->LoadSoundData( m_desc.m_pDest, m_desc.m_itemID, m_desc.m_loadFlag);
      // ���g�̃X���b�h��KILL
      this->Kill();
    }


  } //namespace snd
} //namespace gfl


