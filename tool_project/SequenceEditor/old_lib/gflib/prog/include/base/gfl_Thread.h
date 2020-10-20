//=============================================================================
/**
 * @file   gfl_Thread.h
 * @brief  �X���b�h
 * @author obata_toshihiro
 * @date   2010.11.09
 */
//=============================================================================
#ifndef __GFL_THREAD_H__
#define __GFL_THREAD_H__
#pragma once

#include <nn.h>
#include <gfl_Heap.h> 
#include <gfl_Macros.h>

namespace gfl {
  namespace base {

    class SimpleStackArea;
    class SafeStackArea;

    class Thread 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( Thread );
      friend void ThreadFunc( Thread* thread );  // �X���b�h����֐�


      public: 

      /**
       * @brief �X���b�h�Ǘ����̏�����
       * @param heap �X���b�h�̊Ǘ��̈�Ƃ��Ďg�p����q�[�v
       */
      static void InitializeThreadManagement( gfl::heap::HeapBase* heap );

      /**
       * @brief �X���b�h�Ǘ��̌�Еt��
       *
       * @note
       * �Ǘ��Ώۂ̃X���b�h�������c���Ă���ꍇ
       * �����I�� kill ��, �X���b�h�̏I����҂��܂��B
       */
      static void FinalizeThreadManagement( void );

      /**
       * @brief ���쒆�̃X���b�h�̐����擾����
       * @return ���삪�������Ă��Ȃ�, gfl::base::Thread�C���X�^���X�̐�
       *
       * @attention 
       * ���C���X���b�h��T�E���h�X���b�h�Ȃ�
       * �V�X�e�������Ă��X���b�h�̐��͊܂܂�܂���B
       */
      static u32 GetValidUserThreadCount( void ); 

      /**
       * @brief �����D��x�̑��X���b�h�Ɏ��s�@���^����
       */
      static void Yield( void ); 

      /**
       * @brief �J�����g�X���b�h���w�莞�Ԓ�~������
       *
       * @param milliSeconds �X���b�h���~�����鎞�ԁi�~���b�j
       */
      static void Sleep( s64 milliSeconds );

      /**
       * @brief �S�Ă̓��쒆�̃X���b�h�� kill ����
       *
       * @note
       * �\�t�g�E�F�A���Z�b�g�Ȃ�, ��{����ꊇ���䂷��ړI�ō쐬�B
       * �{����, �X���b�h�̏��L�҂ł���I�u�W�F�N�g���������X���b�h�� kill ����ׂ��B
       */
      static void KillAllRunningThread( void );


      /**
       * @brief �X���b�hID�����C���X���b�h���`�F�b�N
       */
      static bool IsMainThread( bit32 thread_id );

      private:

      // ���쒆�̃X���b�h�Ǘ�
      static const u8 MAX_RUNNING_THREAD_COUNT = 32;
      static u32 s_runningThreadCount;
      static Thread* s_runningThread[ MAX_RUNNING_THREAD_COUNT ];
      static nn::os::CriticalSection* s_criticalSectionForThreadManagement;

      static void AddManagedThread( Thread* );
      static void RemoveManagedThread( Thread* );






      public: 


      enum StackType
      {
        // �X�^�b�N�I�[�o�[�t���[�E�X�^�b�N�A���_�[�t���[���o�Ȃ�
        // �X�^�b�N�̈�̃A���C�������g�Ȃ�
        STACK_TYPE_SIMPLE,

        // �X�^�b�N�I�[�o�[�t���[�E�X�^�b�N�A���_�[�t���[���o����
        // �X�^�b�N�̈��4096�o�C�g�A���C�������g
        // �X�^�b�N�̈�Ƃ͕ʂ�4096*2�o�C�g�K�v
        STACK_TYPE_SAFE,
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param heap       �X�^�b�N�̈�Ɏg�p����q�[�v
       * @param stackSize  �X�^�b�N�̈�̃T�C�Y
       * @param stackType  �X�^�b�N�̈�̃^�C�v
       */
      //-----------------------------------------------------------------------
      Thread( gfl::heap::HeapBase* heap, u32 stackSize, StackType stackType = STACK_TYPE_SIMPLE );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~Thread(); 

      /**
       * @brief �X���b�h�̗D��x
       *
       * @note 
       * 0���ł������D��x
       * 31���ł��Ⴂ�D��x
       */
      typedef u8 Priority;
      static const Priority HighestPriority = nn::os::HIGHEST_THREAD_PRIORITY;  // ( == 0 )
      static const Priority DefaultPriority = nn::os::DEFAULT_THREAD_PRIORITY;  // ( == 16 )
      static const Priority LowestPriority  = nn::os::LOWEST_THREAD_PRIORITY;   // ( == 31 )

      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h���J�n����
       * @param priority  �X���b�h�̗D��x
       */
      //-----------------------------------------------------------------------
      void Start( Priority priority );

      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h���ꎞ��~������
       */
      //-----------------------------------------------------------------------
      void PauseOn( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h�̈ꎞ��~����������
       */
      //-----------------------------------------------------------------------
      void PauseOff( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h���ꎞ��~�����ǂ����𒲂ׂ�
       * @retval true   �ꎞ��~��
       * @retval false  ���쒆
       */
      //-----------------------------------------------------------------------
      bool IsPausing( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h���I��������
       */
      //-----------------------------------------------------------------------
      void Kill( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h�������Ă��邩�ǂ����𒲂ׂ�
       * @retval true  �����Ă���
       * @retval false ����ł���
       */
      //-----------------------------------------------------------------------
      bool IsAlive( void ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief ���g���x�~��Ԃɂ���
       */
      //-----------------------------------------------------------------------
      void Wait( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �҂���Ԃ��H
       * @retval true  �҂����
       * @retval false �҂���ԂłȂ�
       */
      //-----------------------------------------------------------------------
      bool IsWaiting( void );

      //-----------------------------------------------------------------------
      /**
       * @brief Wait()�ɂ��x�~��Ԃ���������
       */
      //-----------------------------------------------------------------------
      void Signal( void );



      private:

      /**
       * @brief �J�n����
       *
       * @note
       * �X���b�h�̊J�n����ɌĂ΂��֐��ł��B
       * �h���N���X�Œ�`���Ă��������B
       */
      virtual void Initialize( void ) {}

      /**
       * @brief ���C������
       *
       * @note
       * �X���b�h�֐��̃��[�v���ŌĂ΂ꑱ����֐��ł��B
       * �h���N���X�Œ�`���Ă��������B
       */
      virtual void Main( void ) = 0;

      /**
       * @brief �I������
       *
       * @note
       * �X���b�h�̏I�����O�ɌĂ΂��֐��ł��B
       * �h���N���X�Œ�`���Ă��������B
       */
      virtual void Finalize( void ) {} 

      /**
       * @brief �X���b�h�I�����̏���
       *
       * @note
       * Kill() ���Ă΂ꂽ����Ɏ��s����鏈���ł��B
       * �h���N���X�Œ�`���Ă��������B
       */
      virtual void OnKill( void ) {}



      private:

      nn::os::Thread m_nnosThread;
      SimpleStackArea* m_simpleStackArea;
      SafeStackArea* m_safeStackArea;
      nn::os::LightEvent m_pauseEvent;
      nn::os::LightEvent m_waitEvent;
      bool m_isPausing;    // �|�[�Y�����ǂ���
      bool m_pauseRequest; // �|�[�Y���N�G�X�g�̗L��
      bool m_killFlag;     // �X���b�h���I��点�邩�ǂ���
      bool dummy;
    };

  } // namespace base
} // namespace gfl

#endif // __GFL_THREAD_H__
