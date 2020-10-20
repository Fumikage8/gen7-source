//=============================================================================
/**
 * @brief �X���b�h�Ǘ�
 * @file gfl_ThreadManager.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include <base/gfl_ThreadManager.h>

namespace gfl {
  namespace base {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap_for_thread_management �X���b�h�̊Ǘ��̈�Ƃ��Ďg�p����q�[�v
     * @param max_running_thread_count   �����ɋN���ł���X���b�h�̐�
     * @param max_waiting_request_count  �o�^�ł��郊�N�G�X�g�̍ő吔
     */
    //-------------------------------------------------------------------------
    ThreadManager::ThreadManager( 
        gfl::heap::HeapBase* heap_for_thread_management,
        u32 max_running_thread_count,
        u32 max_waiting_request_count ) :
      m_heap( heap_for_thread_management ),
      m_waitingRequest( NULL ),
      m_waitingRequestCount( 0 ),
      m_maxWaitingRequestCount( max_waiting_request_count ),
      m_runningRequest( NULL ),
      m_runningRequestCount( 0 ),
      m_maxRunningRequestCount( max_running_thread_count )
    {
      m_waitingRequest = GFL_NEW_ARRAY( m_heap ) Request[ m_maxWaitingRequestCount ];
      m_runningRequest = GFL_NEW_ARRAY( m_heap ) Request[ m_maxRunningRequestCount ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     *
     * @attention
     * �J�����g�X���b�h���x�~���邱�Ƃ�, ���X���b�h�̊�����҂��܂�.
     * IsAllRequestFinished() �őS�Ẵ��N�G�X�g���������Ă���j����������]�܂����ł�.
     */
    //-------------------------------------------------------------------------
    ThreadManager::~ThreadManager()
    {
      // ���s���̃��N�G�X�g�̏I����҂�
      this->WaitAllRunningRequest();

      // ���s����Ȃ��������N�G�X�g��j��
      this->CancelAllWaitingRequest();

      GFL_ASSERT_MSG( ( this->GetRunningRequestCount() == 0 ), "�z��O�̃G���[�ł��B�����܂ł��A�����������B\n" ); //@check
      GFL_ASSERT_MSG( ( this->GetWaitingRequestCount() == 0 ), "�z��O�̃G���[�ł��B�����܂ł��A�����������B\n" ); //@check

      GFL_DELETE_ARRAY m_waitingRequest;
      GFL_DELETE_ARRAY m_runningRequest;
    }

    /**
     * @brief ���s���̑S���N�G�X�g�̊�����҂�
     */
    void ThreadManager::WaitAllRunningRequest( void )
    {
      while( 0 < this->GetRunningRequestCount() ) 
      {
        GFL_PRINT( "WARNING!! ���s���̃X���b�h���c���Ă��܂��I�X���b�h�̏I����҂��܂��B\n" );
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) ); 
        this->RemoveDeadThread(); 
      }
    }

    /**
     * @brief �ҋ@���̑S���N�G�X�g��j������
     */
    void ThreadManager::CancelAllWaitingRequest( void )
    {
      while( 0 < this->GetWaitingRequestCount() )
      {
        GFL_PRINT( "WARNING!! �ҋ@���̃��N�G�X�g��j�����܂��B\n" );

        u32 req_index = this->GetWaitingRequestCount() - 1;
        this->CancelRequest( &m_waitingRequest[ req_index ] );
        this->RemoveWaitingRequestByIndex( req_index );
      }
    }

    /**
     * @brief �w�肵�����N�G�X�g���L�����Z������
     * @param request �L�����Z���Ώۂ̃��N�G�X�g
     */
    void ThreadManager::CancelRequest( const Request* request )
    {
      if( request->callbackTarget ) {
        request->callbackTarget->OnRequestCanceled( *request );
      }
    }

    /**
     * @brief �w�肵���ҋ@���̃X���b�h���Ǘ������菜��
     * @param req_index ��菜�����N�G�X�g�̃C���f�b�N�X
     */
    void ThreadManager::RemoveWaitingRequestByIndex( u32 req_index )
    {
      for( u32 i=req_index; i<(m_waitingRequestCount-1); i++ ) {
        m_waitingRequest[i] = m_waitingRequest[i+1];
      }
      --m_waitingRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h�Ǘ�����
     * ���s���̃X���b�h���Ď���, �I�������X���b�h���Ǘ����珜�O����.
     * ���s�������������X���b�h���N������.
     */
    //-------------------------------------------------------------------------
    void ThreadManager::UpdateThreadManagement( void )
    {
      // �I�������X���b�h�����O
      this->RemoveDeadThread(); 

      // �V�����X���b�h���N��
      if( this->IsNewThreadLaunchEnable() ) {
        this->StartHighestPriorityRequest();
      }
    }

    /**
     * @brief ���s���̃X���b�h�̓�, ���삪�I�������X���b�h���Ǘ������菜��
     */
    void ThreadManager::RemoveDeadThread( void )
    {
      for( u32 i=0; i<m_runningRequestCount; i++ ) 
      {
        if( m_runningRequest[i].thread->IsAlive() == false ) 
        {
#if GFLTHREADMAN_TICK_ENABLE
          // �`�b�N�v�����I��
          this->EndTick( &m_runningRequest[i] );
#endif
          // �R�[���o�b�N
          if( m_runningRequest[i].callbackTarget ) {
            m_runningRequest[i].callbackTarget->OnRequestFinished( m_runningRequest[i] );
          }

          // �Ǘ����珜�O
          this->RemoveRunningRequestByIndex( i ); 
        }
      }
    }

    /**
     * @brief �w�肵�����s���̃X���b�h���Ǘ������菜��
     * @param req_index ��菜�����N�G�X�g�̃C���f�b�N�X
     */
    void ThreadManager::RemoveRunningRequestByIndex( u32 req_index )
    {
      for( u32 i=req_index; i<(m_runningRequestCount-1); i++ ) {
        m_runningRequest[i] = m_runningRequest[i+1];
      }
      --m_runningRequestCount;
    }

#if GFLTHREADMAN_TICK_ENABLE

    /**
     * @brief �`�b�N�̌v�����I������
     * @param request �v�����I�����郊�N�G�X�g
     */
    void ThreadManager::EndTick( Request* request )
    {
      request->endTick = this->GetCurrentTick();
    }

    /**
     * @brief ���݂̃`�b�N�l���擾����
     */
    s64 ThreadManager::GetCurrentTick( void ) const
    {
      return nn::os::Tick::GetSystemCurrent();
    }

#endif

    /**
     * @brief �V�����X���b�h���N���ł��邩�ǂ����H
     * @retval true  �V�����X���b�h���N���ł���
     * @retval false �V�����X���b�h���N���ł��Ȃ�
     */
    bool ThreadManager::IsNewThreadLaunchEnable( void ) const
    {
      u32 wait = this->GetWaitingRequestCount();
      u32 run = this->GetRunningRequestCount();
      u32 max_run = this->GetMaxRunningRequestCount();
      return ( ( 0 < wait ) && ( run < max_run ) );
    }

    /**
     * @brief �ł������D��x�̃��N�G�X�g�������J�n����
     */
    void ThreadManager::StartHighestPriorityRequest( void )
    {
      Request& req = m_waitingRequest[0]; 
#if GFLTHREADMAN_TICK_ENABLE
      this->StartTick( &req );
#endif 
      this->RegisterRunningRequest( req );
      this->StartRequest( req );
      this->RemoveWaitingRequestByIndex( 0 ); 
    }

#if GFLTHREADMAN_TICK_ENABLE
    /**
     * @brief �`�b�N�̌v�����J�n����
     * @param request �`�b�N�v�����J�n���郊�N�G�X�g
     */
    void ThreadManager::StartTick( Request* request )
    {
      request->startTick = this->GetCurrentTick();
    }
#endif 

    /**
     * @brief �w�肵�����N�G�X�g�����s���̃��N�G�X�g�Ƃ��ĊǗ��Ώۂɓo�^����
     * @param req �o�^���郊�N�G�X�g
     */
    void ThreadManager::RegisterRunningRequest( const Request& req )
    {
      if( m_maxRunningRequestCount <= m_runningRequestCount ) {
        GFL_ASSERT(0); //@fix
        return;
      }
      m_runningRequest[ m_runningRequestCount ] = req;
      ++m_runningRequestCount;
    }

    /**
     * @brief �w�肵�����N�G�X�g�ɑ΂��鏈�����J�n����
     * @param req �������J�n���郊�N�G�X�g
     */
    void ThreadManager::StartRequest( const Request& req )
    {
      req.thread->Start( req.threadPriority );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h�N�����N�G�X�g��o�^����
     * @param req  �o�^���郊�N�G�X�g
     * @retval true  �o�^�ł���
     * @retval false �o�^�ł��Ȃ�����
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::AddThreadRequest( const Request& req )
    {
      if( m_waitingRequestCount < m_maxWaitingRequestCount ) 
      { 
        // �ҋ@�D��x�̍~���ɂȂ�悤�ɓK�؂Ȉʒu�ɑ}������
        u32 insert_pos = 0;
        for( insert_pos=0; insert_pos<m_waitingRequestCount; insert_pos++ ) {
          if( req.requestPriority < m_waitingRequest[ insert_pos ].requestPriority ) {
            break;
          }
        }
        for( s32 i=m_waitingRequestCount; insert_pos<i; i-- ) {
          m_waitingRequest[i] = m_waitingRequest[i-1];
        }
        m_waitingRequest[ insert_pos ] = req;
        ++m_waitingRequestCount;
        return true;
      }
      else
      {
        GFL_PRINT( "WARNING!! gfl::base::ThreadManager �o�^�\�ȍő吔���I�[�o�[���Ă��܂�\n" );
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �ҋ@���̃��N�G�X�g�̓�, �w�肵��ID�̃��N�G�X�g���폜����
     * @param requestID �폜���郊�N�G�X�g��ID
     */
    //-------------------------------------------------------------------------
    void ThreadManager::CancelWaitingThreadRequest( u32 requestID )
    {
      for( s32 i=m_waitingRequestCount-1; 0<=i; i-- )
      {
        if( m_waitingRequest[i].ID == requestID ) {
          this->CancelRequest( &m_waitingRequest[i] );
          this->RemoveWaitingRequestByIndex(i);
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �S�Ẵ��N�G�X�g�̎��s���I���������H
     * @retval true  �S�ẴX���b�h���I������
     * @retval false �I�����Ă��Ȃ��X���b�h�����݂���
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsAllRequestFinished( void ) const
    {
      u32 wait = this->GetWaitingRequestCount();
      u32 run = this->GetRunningRequestCount();
      return ( ( wait == 0 ) && ( run == 0 ) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵��ID�̃��N�G�X�g���I���������H
     * @param requestID �`�F�b�N���郊�N�G�X�g��ID
     * @retval true  �I������
     * @retval false �܂��I�����Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsRequestFinished( u32 requestID ) const
    {
      return ( 
          !this->IsRequestRunning( requestID ) && 
          !this->IsRequestWaiting( requestID ) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵��ID�̃��N�G�X�g�����s�����H
     * @param requestID �`�F�b�N���郊�N�G�X�g��ID
     * @retval true  ���s��
     * @retval false �ҋ@�� or ���N�G�X�g�����݂��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsRequestRunning( u32 requestID ) const
    {
      u32 req_count = this->GetRunningRequestCount();
      for( u32 i=0; i<req_count; i++ ) {
        if( m_runningRequest[i].ID == requestID ) {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵��ID�̃��N�G�X�g���ҋ@�����H
     * @retval true  �ҋ@��
     * @retval false ���s�� or ���N�G�X�g�����݂��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsRequestWaiting( u32 requestID ) const
    {
      u32 req_count = this->GetWaitingRequestCount();
      for( u32 i=0; i<req_count; i++ ) {
        if( m_waitingRequest[i].ID == requestID ) {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s���̃��N�G�X�g�̐����擾����
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetRunningRequestCount( void ) const
    {
      return m_runningRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����ɋN���ł��郊�N�G�X�g�̐����擾����
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetMaxRunningRequestCount( void ) const
    {
      return m_maxRunningRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����ɕێ��ł��郊�N�G�X�g�̐����擾����
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetMaxWaitingRequestCount( void ) const
    {
      return m_maxWaitingRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �N���҂��̃��N�G�X�g�̐����擾����
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetWaitingRequestCount( void ) const
    {
      return m_waitingRequestCount;
    }



  }  //namespace base
}  //namespace gfl
