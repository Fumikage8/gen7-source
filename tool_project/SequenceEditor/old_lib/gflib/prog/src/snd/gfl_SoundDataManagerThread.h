//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_SoundDataManagerThread.h
 *	@brief  �T�E���h�f�[�^�}�l�[�W���@�f�[�^�ǂݍ��݁@�X���b�h
 *	@author	tomoya takahashi
 *	@date		2012.12.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_SOUNDDATAMANAGERTHREAD_H__
#define __GFL_SOUNDDATAMANAGERTHREAD_H__
#pragma once

#include <nw.h>

namespace gfl {
  namespace snd {

    class SoundSystem;

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

    class SoundDataManagerThread : public base::Thread
    {
      GFL_FORBID_COPY_AND_ASSIGN( SoundDataManagerThread );

      public:

      struct Description 
      {
        SoundSystem * m_pSoundSystem; // [in] SoundSystem
        SoundHeap * m_pDest;    // [out]    �������ǂݍ��݊i�[��
        bool *      m_pResult;  // [out]    ���ʊi�[��
        u32         m_itemID;   // [in]     �A�C�e��ID
        u32         m_loadFlag; // [in]     ���[�h�t���O
        Description( void ) : 
          m_pDest(NULL),
          m_itemID(0),
          m_loadFlag(false)
        { }
      };

      /**
       * @brief �R���X�g���N�^
       * @param heap_for_thread �X���b�h�̓���Ɏg�p����q�[�v
       * @param desc            �A�[�J�C�u�ǂݍ��݂̏ڍ׏��
       */
      SoundDataManagerThread( 
          gfl::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~SoundDataManagerThread();


      private:

      static const u32 STACK_SIZE;
      Description m_desc;

      //virtual void Initialize( void );
      virtual void Main( void ); 
      //virtual void Finalize( void );
      //virtual void OnKill( void );

    };


  } //namespace snd
} //namespace gfl

#endif	// __GFL_SOUNDDATAMANAGERTHREAD_H__

