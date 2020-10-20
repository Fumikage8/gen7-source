//=============================================================================
/**
 * @brief �X���b�h�Ǘ�
 * @file gfl_ThreadManager.h
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#ifndef __GFL_THREADMANAGER_H__
#define __GFL_THREADMANAGER_H__
#pragma once

// �`�b�N�v�����s�����H
#define GFLTHREADMAN_TICK_ENABLE (0)


namespace gfl {
  namespace base {

    class Thread; 
    class IThreadManageCallback;

    /**
     * @brief �X���b�h�Ǘ��N���X
     * �ݒ肳�ꂽ�X���b�h�̓������s���𒴂��Ȃ��悤,
     * �󂯕t�������N�G�X�g�̂����ł��D��x�̍����X���b�h���珇�Ɏ��s���܂�.
     * �Ȃ�, 3DS�̃v���b�g�t�H�[����ł̓X���b�h�̍ő哯���N������
     * ( SDK, NintendoWare �����p����X���b�h���܂߂� )32�ł�.
     */
    class ThreadManager
    {
      GFL_FORBID_COPY_AND_ASSIGN( ThreadManager );

      public:

      /**
       * @brief �X���b�h�N�����N�G�X�g
       *
       * @note
       * ���s�������X���b�h�� thread �ɃZ�b�g���Ă�������.
       * ThreadManager ��, requestPriority �̒l���������X���b�h���珇�Ɏ��s���܂�.
       * requestPriority �̂������ꍇ�͓o�^�������Ɏ��s���܂�.
       *
       * �X���b�h���I�������ۂ�, �X���b�h���L�����Z�����ꂽ�ۂȂ�,
       * �X���b�h���Ǘ��Ώۂ��珜�O����^�C�~���O�� callbackTarget �ɑ΂��Ēʒm���s���܂�.
       * ����ȍ~, ThreadManager �ł͑ΏۃX���b�h�ɑ΂��鑀�����؍s���܂���̂�,
       * callbackTarget ���ŃX���b�h�̔j���Ȃǂ̏I���������s���Ă�������.
       * ( ThreadManager ���ł� thread �̔j�����s���܂��� )
       */
      class Request 
      {
        public:
          Thread* thread;                        // �o�^����X���b�h
          Thread::Priority threadPriority;       // �X���b�h�̗D��x( 0 �` 31 ) ��0���ł������D��x
          u8 requestPriority;                    // ���N�G�X�g�̗D��x( 0 �` 31 ) ��0���ł������D��x
          u32 ID;                                // ���N�G�X�g����ID
          IThreadManageCallback* callbackTarget; // �R�[���o�b�N�^�[�Q�b�g( �s�v�Ȃ� NULL ) 
          void* pUserWork;                       // �C�ӂ̃��[�N

          Request( void ) : 
            thread( NULL ),
            threadPriority( gfl::base::Thread::DefaultPriority ),
            requestPriority( 16 ),
            ID( 0 ),
            callbackTarget( NULL ),
            pUserWork( NULL )
        {
#if GFLTHREADMAN_TICK_ENABLE
          startTick = 0;
          endTick = 0;
#endif
        }

#if GFLTHREADMAN_TICK_ENABLE
        public:
          s64 startTick;  // �J�n���̃`�b�N
          s64 endTick;    // �I�����̃`�b�N

        public:
          /**
           * @brief �X���b�h�̊J�n����I���܂łɊ|�������`�b�N�����擾����
           */
          s64 GetRunningTick( void ) const { return endTick - startTick; }
#endif
      };


      //-----------------------------------------------------------------------
      // ��{����
      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param heap_for_thread_management �X���b�h�̊Ǘ��̈�Ƃ��Ďg�p����q�[�v
       * @param max_running_thread_count   �����ɋN���ł���X���b�h�̐�
       * @param max_waiting_request_count  �o�^�ł��郊�N�G�X�g�̍ő吔
       */
      ThreadManager( 
          gfl::heap::HeapBase* heap_for_thread_management,
          u32 max_running_thread_count,
          u32 max_waiting_request_count );

      /**
       * @brief �f�X�g���N�^
       *
       * @attention
       * �J�����g�X���b�h���x�~���邱�Ƃ�, ���X���b�h�̊�����҂��܂�.
       * IsAllRequestFinished() �őS�Ẵ��N�G�X�g���������Ă���j����������]�܂����ł�.
       */
      virtual ~ThreadManager();

      /**
       * @brief �X���b�h�Ǘ�����( ���t���[���Ă�ł������� )
       * ���s���̃X���b�h���Ď���, �I�������X���b�h���Ǘ����珜�O����.
       * ���s�������������X���b�h���N������.
       */
      void UpdateThreadManagement( void );


      //-----------------------------------------------------------------------
      // ���N�G�X�g�̓o�^�E�L�����Z��
      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h�N�����N�G�X�g��o�^����
       * @param req  �o�^���郊�N�G�X�g
       * @retval true  �o�^�ł���
       * @retval false �o�^�ł��Ȃ�����
       */
      bool AddThreadRequest( const Request& req );

      /**
       * @brief �ҋ@���̃��N�G�X�g�̓�, �w�肵��ID�̃��N�G�X�g���폜����
       * @param requestID �폜���郊�N�G�X�g��ID
       */
      void CancelWaitingThreadRequest( u32 requestID );


      //-----------------------------------------------------------------------
      // ���N�G�X�g�̏�Ԏ擾
      //-----------------------------------------------------------------------
      /**
       * @brief �S�Ẵ��N�G�X�g�̎��s���I���������H
       * @retval true  �S�ẴX���b�h���I������
       * @retval false �I�����Ă��Ȃ��X���b�h�����݂���
       */
      bool IsAllRequestFinished( void ) const;

      /**
       * @brief �w�肵��ID�̃��N�G�X�g���I���������H
       * @param requestID �`�F�b�N���郊�N�G�X�g��ID
       * @retval true  �I������
       * @retval false �܂��I�����Ă��Ȃ�
       */
      bool IsRequestFinished( u32 requestID ) const;

      /**
       * @brief �w�肵��ID�̃��N�G�X�g���ҋ@�����H
       * @retval true  �ҋ@��
       * @retval false ���s�� or ���N�G�X�g�����݂��Ȃ�
       */
      bool IsRequestWaiting( u32 ) const;

      /**
       * @brief �w�肵��ID�̃��N�G�X�g�����s�����H
       * @param requestID �`�F�b�N���郊�N�G�X�g��ID
       * @retval true  ���s��
       * @retval false �ҋ@�� or ���N�G�X�g�����݂��Ȃ�
       */
      bool IsRequestRunning( u32 requestID ) const;

      /**
       * @brief �����ɋN���ł��郊�N�G�X�g�̐����擾����
       */
      u32 GetMaxRunningRequestCount( void ) const;

      /**
       * @brief ���s���̃��N�G�X�g�̐����擾����
       */
      u32 GetRunningRequestCount( void ) const;

      /**
       * @brief �����ɕێ��ł��郊�N�G�X�g�̐����擾����
       */
      u32 GetMaxWaitingRequestCount( void ) const;

      /**
       * @brief �ҋ@���̃��N�G�X�g�̐����擾����
       */
      u32 GetWaitingRequestCount( void ) const;



      protected: 
      //-----------------------------------------------------------------------
      // �h���N���X�Ɍ��J���郁�\�b�h
      //-----------------------------------------------------------------------
      /**
       * @brief �X���b�h�Ǘ��p�q�[�v���擾����
       */
      gfl::heap::HeapBase* GetHeapForThreadManagement( void ) const { return m_heap; }

      /**
       * @brief ���s���̑S���N�G�X�g�̊�����҂�
       */
      void WaitAllRunningRequest( void );

      /**
       * @brief �ҋ@���̑S���N�G�X�g���L�����Z������
       */
      void CancelAllWaitingRequest( void );



      private: 

      gfl::heap::HeapBase* m_heap;

      Request* m_waitingRequest;
      u32 m_waitingRequestCount;
      u32 m_maxWaitingRequestCount;

      Request* m_runningRequest;
      u32 m_runningRequestCount;
      u32 m_maxRunningRequestCount;

      void CancelRequest( const Request* );
      void RemoveDeadThread( void );
      void RemoveRunningRequestByIndex( u32 );
      void RemoveWaitingRequestByIndex( u32 );
      bool IsNewThreadLaunchEnable( void ) const;
      void StartHighestPriorityRequest( void );
      void RegisterRunningRequest( const Request& );
      void StartRequest( const Request& );
#if GFLTHREADMAN_TICK_ENABLE
      void StartTick( Request* );
      void EndTick( Request* );
      s64 GetCurrentTick( void ) const;
#endif
    };






    /**
     * @brief ���N�G�X�g�ɂ��Ă̒ʒm���󂯎�邽�߂̃C���^�[�t�F�[�X
     *
     * @note
     * ThreadManager �ɂ��Ǘ����I���������_�ŉ��ꂩ�̃��\�b�h���Ă΂��.
     */
    class IThreadManageCallback
    {
      friend class ThreadManager;

      private:

      /**
       * @brief ���N�G�X�g�I�����̃R�[���o�b�N
       * @param finished_req  �I���������N�G�X�g
       *
       * @note 
       * ���N�G�X�g���ꂽ�X���b�h�̓��삪��������( �X���b�h������ )�ۂɒʒm����܂�.
       * �ȍ~, ThreadManager �ł� finished_req �ɑ΂��鑀��͈�؍s���܂���.
       * ( �X���b�h�̔j�����s���܂��� ) 
       * ���̃��\�b�h���I�[�o�[���C�h��, �K�؂ȏI���������s���Ă�������.
       */
      virtual void OnRequestFinished( const ThreadManager::Request& finished_req ) { }

      /**
       * @brief ���N�G�X�g�����s���ꂸ�ɏI�������ۂ̃R�[���o�b�N
       * @param canceled_req  ���s����Ȃ��������N�G�X�g
       *
       * @note 
       * ���N�G�X�g�����s���ꂸ�ɔj�����ꂽ�ۂɒʒm����܂�.
       * �ȍ~, ThreadManager �ł� canceled_req �ɑ΂��鑀��͈�؍s���܂���.
       * ( �X���b�h�̔j�����s���܂��� ) 
       * ���̃��\�b�h���I�[�o�[���C�h��, �K�؂ȏI���������s���Ă�������.
       */
      virtual void OnRequestCanceled( const ThreadManager::Request& canceled_req ) { }
    };





  }  // namespace base
}  // namespace gfl

#endif  // __GFL_THREADMANAGER_H__
