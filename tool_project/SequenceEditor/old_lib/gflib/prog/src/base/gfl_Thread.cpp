//=============================================================================
/**
 * @file gfl_Thread.cpp
 * @brief �X���b�h
 * @author obata_toshihiro
 * @date 2010.11.09
 */
//=============================================================================
#include <nn.h>
#include <gfl_Debug.h>
#include <base/gfl_Thread.h>
#include "gfl_SimpleStackArea.h"
#include "gfl_SafeStackArea.h"

namespace gfl {
  namespace base {


    u32 Thread::s_runningThreadCount = 0;
    Thread* Thread::s_runningThread[ MAX_RUNNING_THREAD_COUNT ];
    nn::os::CriticalSection* Thread::s_criticalSectionForThreadManagement = NULL;

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h�Ǘ����̏�����
     * @param heap �X���b�h�̊Ǘ��̈�Ƃ��Ďg�p����q�[�v
     */
    //-------------------------------------------------------------------------
    void Thread::InitializeThreadManagement( gfl::heap::HeapBase* heap )
    {
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ ) {
        s_runningThread[i] = NULL;
      }
      s_criticalSectionForThreadManagement = GFL_NEW( heap ) nn::os::CriticalSection();
      s_criticalSectionForThreadManagement->Initialize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h�Ǘ��̌�Еt��
     *
     * @note
     * �Ǘ��Ώۂ̃X���b�h�������c���Ă���ꍇ,
     * �����I�� kill ��, �X���b�h�̏I����҂��܂�.
     */
    //-------------------------------------------------------------------------
    void Thread::FinalizeThreadManagement( void )
    {
      // �X���b�h�̏I����҂�
      while( 0 < s_runningThreadCount )
      {
        GFL_PRINT( "WARNING!! �X���b�h��%d�����Ă��܂��B"\
            "�X���b�h�̏I����҂��܂��B\n", s_runningThreadCount );
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
      }

      // ��Еt��
      s_criticalSectionForThreadManagement->Finalize();
      GFL_DELETE s_criticalSectionForThreadManagement;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���쒆�̃X���b�h�̐����擾����
     * @return ���삪�������Ă��Ȃ�, gfl::base::Thread�C���X�^���X�̐�
     *
     * @attention
     * ���C���X���b�h��T�E���h�X���b�h�Ȃ�
     * �V�X�e�������Ă��X���b�h�̐��͊܂܂�܂���B
     */
    //-------------------------------------------------------------------------
    u32 Thread::GetValidUserThreadCount( void )
    {
      return s_runningThreadCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����D��x�̑��X���b�h�Ɏ��s�@���^����
     */
    //-------------------------------------------------------------------------
    void Thread::Yield( void )
    {
      nn::os::Thread::Yield();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �J�����g�X���b�h���w�莞�Ԓ�~������
     *
     * @param milliSeconds �X���b�h���~�����鎞�ԁi�~���b�j
     */
    //-------------------------------------------------------------------------
    void Thread::Sleep( s64 milliSeconds )
    {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(milliSeconds) );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �S�Ă̓��쒆�̃X���b�h�� kill ����
     *
     * @note
     * �\�t�g�E�F�A���Z�b�g�Ȃ�, ��{����ꊇ���䂷��ړI�ō쐬�B
     * �{����, �X���b�h�̏��L�҂ł���I�u�W�F�N�g���������X���b�h�� kill ����ׂ��B
     */
    //-------------------------------------------------------------------------
    void Thread::KillAllRunningThread( void )
    {
      // ���̊֐����s����, ���̃X���b�h�ɂ��
      // �X���b�h�̊Ǘ���񂪏�������邱�Ƃ�h�����߃N���e�B�J���Z�N�V�����ɓ���.
      nn::os::CriticalSection::ScopedLock lock( *s_criticalSectionForThreadManagement );

      // �S�ẴX���b�h���E��
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ ) {
        if( s_runningThread[i] ) {
          s_runningThread[i]->Kill();
        }
      }
    }

    /**
     * @brief �X���b�hID�����C���X���b�h���`�F�b�N
     */
    bool Thread::IsMainThread( bit32 thread_id )
    {
      const nn::os::Thread& thread = nn::os::Thread::GetMainThread();
      if( thread_id == thread.GetId() ){
        return true;
      }
      return false;
    }

    /**
     * @brief �X���b�h���Ǘ��ΏۂƂ��Ēǉ�����
     * @param thread �V���ɊǗ�����X���b�h
     */
    void Thread::AddManagedThread( Thread* thread )
    {
      GFL_ASSERT_MSG( ( s_runningThreadCount < MAX_RUNNING_THREAD_COUNT ),
          "����ȏ�X���b�h���Ǘ��ł��܂���B�X���b�h�̓����N�����𒲐����Ă��������B\n" ); //@check

      // ���̊֐����s����, ���̃X���b�h�ɂ��
      // �X���b�h�̊Ǘ���񂪏�������邱�Ƃ�h�����߃N���e�B�J���Z�N�V�����ɓ���.
      nn::os::CriticalSection::ScopedLock lock( *s_criticalSectionForThreadManagement );

      // �󂢂Ă���ꏊ�ɊǗ��X���b�h��ǉ�
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ )
      {
        if( s_runningThread[i] == NULL ) {
          s_runningThread[i] = thread;
          s_runningThreadCount++;
          break;
        }
      }
    }

    /**
     * @brief �X���b�h���Ǘ��Ώۂ��珜�O����
     * @param thread ���O����X���b�h
     */
    void Thread::RemoveManagedThread( Thread* thread )
    {
      GFL_ASSERT_MSG( ( 0 < s_runningThreadCount ), "�z��O�̃G���[�ł��Bby obata\n" ); //@check

      // ���̊֐����s����, ���̃X���b�h�ɂ��
      // �X���b�h�̊Ǘ���񂪏�������邱�Ƃ�h�����߃N���e�B�J���Z�N�V�����ɓ���.
      nn::os::CriticalSection::ScopedLock lock( *s_criticalSectionForThreadManagement );

      // �X���b�h���Ǘ����珜�O
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ )
      {
        if( s_runningThread[i] == thread ) {
          s_runningThread[i] = NULL;
          s_runningThreadCount--;
          break;
        }
      }
    }



    //=========================================================================
    /**
     * @brief �X���b�h����֐�
     */
    //=========================================================================
    void ThreadFunc( gfl::base::Thread* thread )
    {
      Thread::AddManagedThread( thread );
      thread->Initialize();

      while( !thread->m_killFlag )
      {
        thread->Main();

        if( thread->m_pauseRequest )
        {
          thread->m_isPausing = true;
          thread->m_pauseEvent.Wait();

          thread->m_isPausing = false;
          thread->m_pauseRequest = false;
        }
      }

      thread->Finalize();
      Thread::RemoveManagedThread( thread );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap       �X�^�b�N�̈�Ɏg�p����q�[�v
     * @param stackSize  �X�^�b�N�̈�̃T�C�Y
     * @param stackType  �X�^�b�N�̈�̃^�C�v
     */
    //-------------------------------------------------------------------------
    Thread::Thread( gfl::heap::HeapBase* heap, u32 stackSize, StackType stackType ) :
      m_nnosThread(),
      m_simpleStackArea( NULL ),
      m_safeStackArea( NULL ),
      m_pauseEvent(),
      m_waitEvent(),
      m_isPausing( false ),
      m_pauseRequest( false ),
      m_killFlag( false )
    {

      switch( stackType )
      {
      case STACK_TYPE_SIMPLE:
        m_simpleStackArea = GFL_NEW( heap ) SimpleStackArea( heap, stackSize );
        break;

      case STACK_TYPE_SAFE:
        m_safeStackArea = GFL_NEW( heap ) SafeStackArea( heap, stackSize );
        break;

      default:
        GFL_ASSERT_STOP(0); //@check
        break;
      }

      m_pauseEvent.Initialize( false );
      m_waitEvent.Initialize( false );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    Thread::~Thread()
    {
      if( m_nnosThread.IsValid() ) {
        this->Kill();
        this->PauseOff();
        m_nnosThread.Join();
        m_nnosThread.Finalize();
      }
      GFL_SAFE_DELETE( m_simpleStackArea );
      GFL_SAFE_DELETE( m_safeStackArea );
      m_pauseEvent.Finalize();
      m_waitEvent.Finalize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h���J�n����
     * @param Priority priority �X���b�h�̗D��x
     */
    //-------------------------------------------------------------------------
    void Thread::Start( Priority priority )
    {
      if( m_nnosThread.IsValid() ) {
        GFL_ASSERT(0); //@fix Start �ς�
        return;
      }

      if( m_killFlag ) {
        GFL_ASSERT(0); //@fix Kill �ς�
        return;
      }

      priority = gfl::math::Max( priority, HighestPriority );
      priority = gfl::math::Min( priority, LowestPriority );

      if( m_safeStackArea )
      {
        m_nnosThread.Start( ThreadFunc, this, *m_safeStackArea, priority );
      }
      else if( m_simpleStackArea )
      {
        m_nnosThread.Start( ThreadFunc, this, *m_simpleStackArea, priority );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h���ꎞ��~������
     */
    //-------------------------------------------------------------------------
    void Thread::PauseOn( void )
    {
      m_pauseRequest = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h�̈ꎞ��~����������
     */
    //-------------------------------------------------------------------------
    void Thread::PauseOff( void )
    {
      if( this->m_isPausing )
      {
        m_pauseEvent.Signal();
      }
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �X���b�h���ꎞ��~�����ǂ����𒲂ׂ�
     * @retval true   �ꎞ��~��
     * @retval false  ���쒆
     */
    //-----------------------------------------------------------------------
    bool Thread::IsPausing( void ) const
    {
      return m_isPausing;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h���I��������
     */
    //-------------------------------------------------------------------------
    void Thread::Kill( void )
    {
      if( !m_killFlag ) {
        this->OnKill();
        this->PauseOff();
        m_killFlag = true;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h�������Ă��邩�ǂ����𒲂ׂ�
     * @retval true  �����Ă���
     * @retval false ����ł���
     */
    //-------------------------------------------------------------------------
    bool Thread::IsAlive( void ) const
    {
      if( !m_nnosThread.IsValid() ) { // �J�n����Ă��Ȃ�
        return false;
      }
      return m_nnosThread.IsAlive();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���g���x�~��Ԃɂ���
     */
    //-------------------------------------------------------------------------
    void Thread::Wait( void )
    {
      m_waitEvent.Wait();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief Wait()�ɂ��x�~��Ԃ���������
     */
    //-------------------------------------------------------------------------
    void Thread::Signal( void )
    {
      m_waitEvent.Signal();
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �҂���Ԃ��H
     * @retval true  �҂����
     * @retval false �҂���ԂłȂ�
     */
    //-----------------------------------------------------------------------
    bool Thread::IsWaiting( void )
    {
      return m_waitEvent.TryWait();
    }



  } // namespace base
} // namespace gfl
