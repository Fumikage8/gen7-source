//=============================================================================
/**
 * @brief �t�@�C���ǂݍ��݊Ǘ�
 * @file gfl_FileReadManager.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include <base/gfl_ThreadManager.h>
#include <fs/gfl_ArcFile.h>
#include <fs/gfl_FileReadManager.h>

namespace gfl {
  namespace fs {

    //-----------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap_for_management  �C���X�^���X���m�ۂ���q�[�v
     * @param arcfile_count        �Ǘ�����A�[�J�C�u�t�@�C���̐�
     * @param max_request_count    �����Ɏ󂯕t���邱�Ƃ��ł���ǂݍ��݃��N�G�X�g�̐�
     * @param max_read_count       �����ɔ񓯊��ǂݍ��݂����s���鐔
     * @param read_thread_priority �ǂݍ��݃X���b�h�̗D�揇��
     */
    //-----------------------------------------------------------------------
    FileReadManager::FileReadManager( 
        gfl::heap::HeapBase* heap_for_management,
        u32 arcfile_count,
        u32 max_request_count,
        u8 max_read_count,
        base::Thread::Priority read_thread_priority ) :
      base::ThreadManager( heap_for_management, max_read_count, max_request_count ),
      base::IThreadManageCallback(),
      m_pWorkHeap( heap_for_management ),
      m_readThreadPriority( read_thread_priority ),
      m_manageData( NULL ),
      m_maxManageDataCount( arcfile_count )
    {
      m_manageData = GFL_NEW_ARRAY( heap_for_management ) FileManageData[ arcfile_count ];

      for( u32 i=0; i<m_maxManageDataCount; i++ ) {
        this->InitManageData( &m_manageData[i] );
      }
    }

    /**
     * @brief �t�@�C���Ǘ��f�[�^������������
     * @param pManageData ����������f�[�^
     */
    void FileReadManager::InitManageData( FileManageData* pManageData )
    {
      pManageData->arcID = ArcFile::ARCID_NULL;
      pManageData->arcFile = NULL;
      pManageData->heapForArcFile = NULL;
      pManageData->requestCount = 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    FileReadManager::~FileReadManager()
    {
      // ���s���̃��N�G�X�g��҂�
      this->WaitAllRunningRequest();

      // �����s�̃��N�G�X�g���L�����Z��
      this->CancelAllWaitingRequest();

      // ���ׂĂ̊Ǘ��f�[�^��j��
      for( u32 i=0; i<m_maxManageDataCount; i++ ) {
        this->DeleteManageData( &m_manageData[i] );
      }
      GFL_DELETE_ARRAY( m_manageData );
    }

    /**
     * @brief �w�肵���Ǘ��t�@�C����j������
     * @param pManageData �j������Ǘ��t�@�C���f�[�^
     */
    void FileReadManager::DeleteManageData( FileManageData* pManageData )
    {
      GFL_SAFE_DELETE( pManageData->arcFile );
      this->InitManageData( pManageData );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�u�t�@�C�����o�^�ς݂��H
     * @param arcID �`�F�b�N����A�[�J�C�u�t�@�C����ID
     * @retval true  �o�^�ς�
     * @retval false �o�^����Ă��Ȃ�
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::IsFileManaged( ArcFile::ARCID arcID ) const
    {
      return ( this->GetManageData( arcID ) != NULL );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�u�t�@�C����o�^����
     * @param heapForArc  ArcFile��u���q�[�v
     * @param arcID       �o�^����A�[�J�C�u�t�@�C����ID
     * @param hio         true:HIO�œǂ�, false:ROM����ǂ�
     *
     * @attention
     * heapForArc �Ɋm�ۂ���郁������ 
     * RemoveManageFile() ���ĂԂ��Ƃŉ������܂��B
     * �q�[�v�̒f�Љ��ɒ��ӂ��Ă��������B
     */
    //-------------------------------------------------------------------------
    void FileReadManager::AppendManageFile( 
        gfl::heap::HeapBase* heapForArc,
        ArcFile::ARCID arcID,
        bool hio )
    {
      if( this->IsFileManaged( arcID ) ) {
        GFL_PRINT( "WARNING!! �A�[�J�C�u( %d )�͊��ɓo�^�ς݂ł�\n", arcID );
        GFL_ASSERT(0);  //@check
        return;
      }

      u32 init_flag = ( hio ) ? ( ArcFile::HIO ) : ( 0 );
      FileManageData* manage_data = this->GetEmptyManageData();
      if( manage_data ) {
        manage_data->arcID = arcID;
        manage_data->arcFile = GFL_NEW( heapForArc ) ArcFile( heapForArc, arcID, init_flag );
        manage_data->heapForArcFile = heapForArc;
      }
      else {
        GFL_PRINT( "ERROR!! �o�^�t�@�C����������𒴂��Ă��܂�\n" );
        GFL_ASSERT_STOP(0);
      }
    }

    /**
     * @brief �w�肵���A�[�J�C�u�̊Ǘ��f�[�^���擾����
     * @param arcID �ΏۃA�[�J�C�u��ID
     * @retval NULL �w�肵���A�[�J�C�u���o�^����Ă��Ȃ��ꍇ
     */
    FileReadManager::FileManageData* FileReadManager::GetManageData( ArcFile::ARCID arcID ) const
    {
      for( u32 i=0; i<m_maxManageDataCount; i++ )
      {
        if( m_manageData[i].arcID == arcID ) {
          return &m_manageData[i];
        }
      }
      return NULL;
    }

    /**
     * @brief ���g�p�̊Ǘ��f�[�^���擾����
     * @return ���g�p�Ǘ��f�[�^�̃|�C���^
     * @retval NULL  ��̊Ǘ��f�[�^���Ȃ��ꍇ
     */
    FileReadManager::FileManageData* FileReadManager::GetEmptyManageData( void ) const
    {
      for( u32 i=0; i<m_maxManageDataCount; i++ )
      {
        if( m_manageData[i].arcFile == NULL ) {
          return &m_manageData[i];
        }
      }
      return NULL;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �o�^����Ă���A�[�J�C�u�t�@�C�����擾����
     * @param arcID  �擾�Ώۂ̃A�[�J�C�u�t�@�C��ID
     * @retval NULL  �w�肵���A�[�J�C�u�t�@�C�����o�^����Ă��Ȃ��ꍇ
     */
    //-----------------------------------------------------------------------
    gfl::fs::ArcFile* FileReadManager::GetManageFile( ArcFile::ARCID arcID ) const
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        return NULL;
      }
      return manage_data->arcFile;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����Ǘ��Ώۂ��珜�O����
     * @param arcID �o�^����������A�[�J�C�u�t�@�C����ID
     *
     * @attention
     * arcID �ւ̓ǂݍ��݂��I�����Ă��Ȃ��ꍇ, �J�����g�X���b�h���x�~�����܂��B
     */
    //-------------------------------------------------------------------------
    void FileReadManager::RemoveManageFile( ArcFile::ARCID arcID )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        return;
      }

      // �Ώۃt�@�C���̓ǂݍ��݂��I�����Ă��Ȃ��Ȃ�, �I����҂�
      while( 0 < manage_data->requestCount ) 
      {
        GFL_PRINT( "WARNING!! �A�[�J�C�u( %d )�͓ǂݍ��ݒ��ł��B�ǂݍ��݂̏I����҂��܂��B\n", arcID );
        GFL_ASSERT(0);  //@check
        this->UpdateThreadManagement();
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) ); 
      }

      // �t�@�C���̊Ǘ�����j������
      this->DeleteManageData( manage_data );
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief �w��A�[�J�C�u��FastMode=���������[�h�Ɉڍs����(�����ǂݍ��ݔ�)
     *
     * @param  arcID  FastMode�Ɉڍs������A�[�J�C�uID
     * @param  heapForFastTable  �������e�[�u�����m�ۂ���q�[�v(NULL�w��Ȃ�AppendManageFile�ɓn�����q�[�v����m��)
     *
     * @retval true   FastMode�Ɉڍs�ł���
     * @retval false  FastMode�Ɉڍs�o���Ȃ�����(���o�^,HIT�I�v�V�������L���A����FastMode�Ȃ�)
     *
     * @note
     * AppendManageFile()�ŃA�[�J�C�u��o�^�����i�K�ł́A�t�@�C���̍������e�[�u���̓�������Ƀ��[�h����Ă��Ȃ��B
     *
     * AppendManageFile()�ƍŏ���AddReadRequest()�̔��s�ʒu������Ă���ƁA
     * �������e�[�u�����������ɏ��^�C�~���O������邽�߁A�q�[�v�̒f�Љ��v���ɂȂ肦��B
     *
     * ���̂悤�ȏꍇ��AppendManageFile()����ɁA�����I��FastMode�ւ̈ڍs�����Ă������ƂŒf�Љ���h��
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::SetupForFastMode( ArcFile::ARCID arcID, gfl::heap::HeapBase* heapForFastTable )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        GFL_ASSERT_MSG( 0, "arcID=%d �̓}�l�[�W���[�ɖ��o�^�ł�\n", arcID );  //@check
        return false;
      }
      ArcFile* arcfile = manage_data->arcFile;

      if( arcfile->IsHioFile() != false ){
        return false; //HIO�ǂݍ��݃I�v�V�����t���̏ꍇ�͈ڍs���Ȃ�
      }
      if( arcfile->IsFastMode() != false ){
        return false; //����FastMode
      }
      //�����ǂݍ��݂�FastMode��
      if( heapForFastTable == NULL ){
        heapForFastTable = manage_data->heapForArcFile;
      }

      if( arcfile->IsOpen() == false ) {
        arcfile->Open();
      }
      arcfile->SetupForFastMode( heapForFastTable );
      return true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w��A�[�J�C�u��FastMode�ڍs���J�n����(�񓯊��ǂݍ��ݔ�)
     *
     * @param  arcID             FastMode�Ɉڍs������A�[�J�C�uID
     * @param  heapForThread     �������ڍs�X���b�h�𐶐�����ꎞ��Ɨp�q�[�vNULL�w��Ȃ�AppendManageFile�ɓn�����q�[�v�̌������m�ہj 
     * @param  heapForFastTable  �������e�[�u�����m�ۂ���q�[�v(NULL�w��Ȃ�AppendManageFile�ɓn�����q�[�v����m��)
     *
     * @retval true  FastMode�ڍs�J�n
     * @retval false FastMode�J�n�o���Ȃ�����(���o�^,HIT�I�v�V�������L���A����FastMode�Ȃ�)
     *
     * ����ɕK��WaitSetupForFastMode�ő҂��ƁB
     *
     * �Ăяo��ArcFile::StartSetupForFastMode()��
     * ���������[�h�ڍs�ɓ����ŃX���b�h��p���邽�߁A
     * StartSetupForFastMode()�Ăяo�������WaitSetupForFastMode()�ő҂��Ȃ���
     * �������e�[�u�������A�h���X����܂�Ȃ��\��������B
     *
     * StartSetupForFastMode��false��Ԃ����ꍇ�ɁA
     * WaitSetupForFastMode()�ő҂��Ă����U��true(=�I��)���Ԃ�A���S�Ɏg����悤�ɂ��Ă���܂��B
     *
     * �������A�w�肵���A�[�J�C�u�����o�^�̏ꍇ�AGFL_ASSERT()�͔������܂��B
     *
     * @note
     * AppendManageFile()�ŃA�[�J�C�u��o�^�����i�K�ł́A�t�@�C���̍������e�[�u���̓�������Ƀ��[�h����Ă��Ȃ��B
     *
     * AppendManageFile()�ƍŏ���AddReadRequest()�̔��s�ʒu������Ă���ƁA
     * �������e�[�u�����������ɏ��^�C�~���O������邽�߁A�q�[�v�̒f�Љ��v���ɂȂ肦��B
     *
     * ���̂悤�ȏꍇ��AppendManageFile()����ɁA�����I��FastMode�ւ̈ڍs�����Ă������ƂŒf�Љ���h��
     *
     * �ꉞ�AArcFile�N���X�̓��������Ƃ��āA���������[�h�ڍs�X���b�h�������Ă���Ԃ�
     * ArcFile�N���X���j�����ꂽ�ꍇ�A�X���b�h��ArcFile�N���X�̃f�X�g���N�^�ɎE����邪
     * ��{�I�Ȏg�����Ƃ��Ă̓��N�G�X�g���o�������Wait�ő҂ׂ��ł���B
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::StartSetupForFastMode(
          ArcFile::ARCID arcID,
          gfl::heap::HeapBase* heapForThread, 
          gfl::heap::HeapBase* heapForFastTable )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        GFL_ASSERT_MSG( 0, "arcID=%d �̓}�l�[�W���[�ɖ��o�^�ł�\n", arcID );  //@check
        return false;
      }
      ArcFile* arcfile = manage_data->arcFile;

      if( arcfile->IsHioFile() != false ){
        return false; //HIO�ǂݍ��݃I�v�V�����t���̏ꍇ�͍��������[�h�ڍs���N�G�X�g���̂��s��Ȃ��̂ŉ������Ȃ�
      }
      if( arcfile->IsFastMode() != false ){
        return false; //����FastMode
      }
      if( heapForFastTable == NULL ){
        heapForFastTable = manage_data->heapForArcFile;
      }
      if( heapForThread == NULL ){
        heapForThread = manage_data->heapForArcFile;
      }
      //�t�@�C����Open��StartSetupForFastMode�̃X���b�h���s���Ă����̂ŁA�����ŃI�[�v�����Ȃ��Ă���
      arcfile->StartSetupForFastMode( heapForThread->GetLowerHandle(), heapForFastTable );
      return true;
    }
   
    //-------------------------------------------------------------------------
    /**
     * @brief �w��A�[�J�C�u��FastMode�ڍs��҂�(�񓯊��ǂݍ��ݔ�)
     *
     * @retval true  �ڍs����
     * @retval false �܂��ڍs��
     *
     * StartSetupForFastMode()�ŊJ�n���AWaitSetupForFastMode�ő҂�
     * true ���Ԃ�܂ŌJ��Ԃ��Ă�, �ڍs�̊�����҂��Ă�������.
     *
     * �w�肵���A�[�J�C�u�����o�^�̏ꍇ�Ȃǂ́A�A�T�[�g������true���Ԃ�܂�
     *
     * @note
     * AppendManageFile()�ŃA�[�J�C�u��o�^�����i�K�ł́A�t�@�C���̍������e�[�u���̓�������Ƀ��[�h����Ă��Ȃ��B
     *
     * AppendManageFile()�ƍŏ���AddReadRequest()�̔��s�ʒu������Ă���ƁA
     * �������e�[�u�����������ɏ��^�C�~���O������邽�߁A�q�[�v�̒f�Љ��v���ɂȂ肦��B
     *
     * ���̂悤�ȏꍇ��AppendManageFile()����ɁA�����I��FastMode�ւ̈ڍs�����Ă������ƂŒf�Љ���h��
     *
     * �񓯊��ł̂�����́A�Ăяo��ArcFile::StartSetupForFastMode()��
     * ���������[�h�ڍs�ɓ����ŃX���b�h��p���邽�߁A
     * StartSetupForFastMode()�Ăяo�������WaitSetupForFastMode()�ő҂��Ȃ���
     * �������e�[�u�������A�h���X����܂�Ȃ��\��������̂Œ��ӁB
     *
     * �ꉞ�AArcFile�N���X�̓��������Ƃ��āA���������[�h�ڍs�X���b�h�������Ă���Ԃ�
     * ArcFile�N���X���j�����ꂽ�ꍇ�A�X���b�h��ArcFile�N���X�̃f�X�g���N�^�ɎE����邪
     * ��{�I�Ȏg�����Ƃ��Ă̓��N�G�X�g���o�������Wait�ő҂ׂ��ł���B
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::WaitSetupForFastMode( ArcFile::ARCID arcID )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        GFL_ASSERT_MSG( 0, "arcID=%d �̓}�l�[�W���[�ɖ��o�^�ł�\n", arcID );  //@check
        return true;
      }
      ArcFile* arcfile = manage_data->arcFile;

      if( arcfile->IsHioFile() != false ){
        return true; //HIO�ǂݍ��݃I�v�V�����t���̏ꍇ�͍��������[�h�ڍs���N�G�X�g���̂��s��Ȃ��̂ŉ������Ȃ�
      }
      return arcfile->WaitSetupForFastMode();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݃��N�G�X�g��o�^����
     * @param req  ���N�G�X�g�f�[�^
     * @retval true  ���N�G�X�g��o�^�ł���
     * @retval false ���N�G�X�g��o�^�ł��Ȃ�����
     *
     * @note
     * ���N�G�X�g�����s������,
     * �Ώۂ̃f�[�^�T�C�Y�̃o�b�t�@�� heapForBuffer ����m�ۂ���, �f�[�^��ǂݍ��݂܂�.
     * *ppDestBuffer �Ɋm�ۂ����o�b�t�@�ւ̃|�C���^���Z�b�g���܂�.
     * *pDestBufferSize �ɓǂݍ��񂾃T�C�Y���Z�b�g���܂�.
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::AddReadRequest( const ReadRequest& req )
    {
      FileManageData* manage_data = this->GetManageData( req.arcID );

      // �A�[�J�C�u�����o�^
      if( manage_data == NULL ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager �ǂݍ��݃��N�G�X�g�𔭍s����O�ɁA�ΏۃA�[�J�C�u�� AppendManageFile() �œo�^���Ă��������B\n" );
        return false;
      };

      // ����ȏ�o�^�ł��Ȃ�
      if( this->GetMaxWaitingRequestCount() <= this->GetWaitingRequestCount() ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager ����ȏ�A�ǂݍ��݃��N�G�X�g��o�^�ł��܂���B\n" );
        return false;
      }

      // �ǂݍ��݃X���b�h�̓�����`
      ArcFileReadThread::Description read_desc;
      read_desc.dest_buf        = req.ppDestBuffer;
      read_desc.dest_size       = req.pDestBufferSize;
      read_desc.buf_alignment   = req.destBufferAlignment;
      read_desc.arcfile         = manage_data->arcFile;
      read_desc.datID           = req.datID;
      read_desc.compressed      = req.compressed;
      read_desc.heap_for_arc    = manage_data->heapForArcFile;
      read_desc.heap_for_buf    = req.heapForBuffer;
      read_desc.heap_for_uncomp = req.heapForUncomp;

      // �ǂݍ��݃X���b�h����
      // �X���b�h�I�����̃R�[���o�b�N�Ŕj������
      ArcFileReadThread* read_thread;
      if( req.heapForWork )
      {
        read_thread = GFL_NEW( req.heapForWork ) gfl::fs::ArcFileReadThread( req.heapForWork, read_desc );
      }
      else
      {
        gfl::heap::HeapBase* lower_heap = m_pWorkHeap->GetLowerHandle();
        read_thread = GFL_NEW( lower_heap ) gfl::fs::ArcFileReadThread( lower_heap, read_desc );
      }

      // �X���b�h���N�G�X�g��
      // �e�N���X�ł��� ThreadManager �ɔ��s����
      base::ThreadManager::Request request;
      request.thread          = read_thread;
      request.threadPriority  = m_readThreadPriority;
      request.requestPriority = req.requestPriority;
      request.ID              = this->CalcRequestID( req.ppDestBuffer );
      request.callbackTarget  = this;
      request.pUserWork       = static_cast<void*>( manage_data );
      bool add_result = this->AddThreadRequest( request );
      GFL_ASSERT_STOP( add_result );

      // �A�[�J�C�u�t�@�C���̃��N�G�X�g�J�E���^���C���N�������g
      manage_data->requestCount++;

      return true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݃��N�G�X�g��o�^����
     * @param[out] ppDestBuffer        �ǂݍ��񂾃f�[�^�ւ̃|�C���^���Z�b�g����|�C���^�ϐ��̃A�h���X
     * @param[out] pDestBufferSize     �ǂݍ��񂾃f�[�^�T�C�Y�̊i�[��( �s�v�Ȃ�NULL )
     * @param[in]  heapForBuffer       �ǂݍ��ݐ�o�b�t�@���m�ۂ���q�[�v
     * @param[in]  destBufferAlignment �ǂݍ��ݐ�o�b�t�@�̃A���C�������g
     * @param[in]  arcID               �ǂݍ��ރf�[�^�̃A�[�J�C�u��ID
     * @param[in]  datID               �ǂݍ��ރf�[�^�̃A�[�J�C�u���f�[�^ID
     * @param[in]  compressed          true:���k����Ă���, false:���k����Ă��Ȃ�
     * @param[in]  requestPriority     ���N�G�X�g�̎��s�D��x( �����������D��x������ )
     *
     * @retval true  ���N�G�X�g��o�^�ł���
     * @retval false ���N�G�X�g��o�^�ł��Ȃ�����
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::AddReadRequest( 
        void** ppDestBuffer,
        u32* pDestBufferSize,
        gfl::heap::HeapBase* heapForBuffer,
        u32 destBufferAlignment,
        ArcFile::ARCID arcID,
        ArcFile::ARCDATID datID,
        bool compressed,
        u8 requestPriority )
    {
      ReadRequest req;
      req.ppDestBuffer        = ppDestBuffer;
      req.pDestBufferSize     = pDestBufferSize;
      req.heapForBuffer       = heapForBuffer;
      req.destBufferAlignment = destBufferAlignment;
      req.arcID               = arcID;
      req.datID               = datID;
      req.compressed          = compressed;
      req.requestPriority     = requestPriority;
      return this->AddReadRequest( req );
    }

		// ��2012.7.4.�ǉ� tetsu
    //-------------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݃��N�G�X�g��o�^����(�O���w���ɓǂݍ���)
     * @param req  ���N�G�X�g�f�[�^
     * @retval true  ���N�G�X�g��o�^�ł���
     * @retval false ���N�G�X�g��o�^�ł��Ȃ�����
		 *
		 * ���N�G�X�gID�͓ǂݍ��ݐ�o�b�t�@�Ɠ����� ID = (u32)pDestBuffer
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::AddReadRequestStatic( const ReadRequestStatic& req )
    {
      if( ( req.pDestBuffer == NULL ) ||
          ( req.destBufferSize <= 0 ) ||
          ( ( req.compressed == true ) && ( req.heapForUncomp == NULL ) ) ) {
        GFL_ASSERT(0);  //@fix
        return false;
      }

      FileManageData* manage_data = this->GetManageData( req.arcID );

      // �A�[�J�C�u�����o�^
      if( manage_data == NULL ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager �ǂݍ��݃��N�G�X�g�𔭍s����O�ɁA�ΏۃA�[�J�C�u�� AppendManageFile() �œo�^���Ă��������B\n" );
        return false;
      };

      // ����ȏ�o�^�ł��Ȃ�
      if( this->GetMaxWaitingRequestCount() <= this->GetWaitingRequestCount() ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager ����ȏ�A�ǂݍ��݃��N�G�X�g��o�^�ł��܂���B\n" );
        return false;
      }

      // �ǂݍ��݃X���b�h�̓�����`
      ArcFileReadThreadStatic::Description read_desc;
      read_desc.dest_buf        = req.pDestBuffer;
      read_desc.dest_size       = req.destBufferSize;
      read_desc.arcfile         = manage_data->arcFile;
      read_desc.datID           = req.datID;
      read_desc.compressed      = req.compressed;
      read_desc.heap_for_arc    = manage_data->heapForArcFile;
      read_desc.heap_for_uncomp = req.heapForUncomp;
      read_desc.read_size       = req.pReadSize;

      // �ǂݍ��݃X���b�h����
      // �X���b�h�I�����̃R�[���o�b�N�Ŕj������
      ArcFileReadThreadStatic* read_thread;
			if( req.heapForWork ){
        read_thread = GFL_NEW( req.heapForWork ) gfl::fs::ArcFileReadThreadStatic( req.heapForWork, read_desc );
			}else{
        gfl::heap::HeapBase* lower_heap = m_pWorkHeap->GetLowerHandle();
        read_thread = GFL_NEW( lower_heap ) gfl::fs::ArcFileReadThreadStatic( lower_heap, read_desc );
      }

      // �X���b�h���N�G�X�g��
      // �e�N���X�ł��� ThreadManager �ɔ��s����
      base::ThreadManager::Request request;
      request.thread          = read_thread;
      request.threadPriority  = m_readThreadPriority;
      request.requestPriority = req.requestPriority;
      request.ID              = (u32)req.pDestBuffer;	// �L�����Z�����̌ŗLID
																											// ���肦�Ȃ��͂����������ɓ��̈�ɓǂݍ��݂������ۃL�����Z�������
																											// �Ӑ}���Ȃ��������o�邩������Ȃ��̂Ōx�����K�v���H
      request.callbackTarget  = this;
      request.pUserWork       = static_cast<void*>( manage_data );
      bool add_result = this->AddThreadRequest( request );
      GFL_ASSERT_STOP( add_result );

      // �A�[�J�C�u�t�@�C���̃��N�G�X�g�J�E���^���C���N�������g
      manage_data->requestCount++;

      return true;
    }

		// �ȉ�ID�̎w����@�̖��ɂ��ʊ֐��쐬
    //-----------------------------------------------------------------------
    /**
     * @brief ID�𒼐ڎw�肵�ēǂݍ��݃��N�G�X�g���L�����Z������
     * @retval true   �L�����Z�����܂���
     * @retval false  �L�����Z���ł��܂���ł���( ���łɓǂݍ��ݒ� )
     *
     * @note 
     * ���ɓǂݍ��ݒ��̃��N�G�X�g�̓L�����Z���ł��܂���B
     * IsReadFinished() ��, �ǂݍ��݂̏I����҂��Ă��������B
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::CancelReadRequest( u32 requestID )
    {
      if( this->IsRequestWaiting( requestID ) ) {
        this->CancelWaitingThreadRequest( requestID );
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ID�𒼐ڎw�肵�Ďw�肵���f�[�^�̓ǂݍ��݂��������������m�F
     * @retval true  ��������
     * @retval false �������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::IsReadFinished( u32 requestID ) const
    {
      return this->IsRequestFinished( requestID );
    }

		// ��2012.7.4.�ǉ� tetsu


    /**
     * @brief �ǂݍ��݃��N�G�X�g��ID���v�Z����
     * @param[in] ppDestBuffer �Ǎ���o�b�t�@�̃A�h���X
     */
    u32 FileReadManager::CalcRequestID( void** ppDestBuffer ) const
    {
      return (u32)( ppDestBuffer ); // �Ǎ���A�h���X��ID�Ƃ���
    }

    /**
     * @brief �R�[���o�b�N: ���N�G�X�g�I��
     * @param finished_request  �I���������N�G�X�g
     */
    void FileReadManager::OnRequestFinished( const base::ThreadManager::Request& finished_request )
    {
      this->FinalizeRequest( finished_request ); 

#if GFLTHREADMAN_TICK_ENABLE
      // �����������Ԃ��o��
      nn::os::Tick tick( finished_request.GetRunningTick() );
      GFL_PRINT( "�t�@�C���ǂݍ��ݏI��: ������������=%dms\n", tick.ToTimeSpan().GetMilliSeconds() );
#endif
    }

    /**
     * @brief �R�[���o�b�N: ���N�G�X�g�����s���ꂸ�ɏI������
     * @param canceled_request  ���s����Ȃ��������N�G�X�g
     */
    void FileReadManager::OnRequestCanceled( const base::ThreadManager::Request& canceled_request )
    {
      this->FinalizeRequest( canceled_request );
    }

    /**
     * @brief ���N�G�X�g�̏I������
     * @param request �Ώۂ̃��N�G�X�g
     */
    void FileReadManager::FinalizeRequest( const base::ThreadManager::Request& request )
    {
      // �ǂݍ��݃J�E���^���f�N�������g
      FileManageData* manage_data = reinterpret_cast<FileManageData*>( request.pUserWork );
      GFL_ASSERT_STOP( 0 < manage_data->requestCount );  //@check
      manage_data->requestCount--;

      // �ǂݍ��݃X���b�h��j��
      GFL_DELETE request.thread; 
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݃��N�G�X�g���L�����Z������
     * @retval true   �L�����Z�����܂���
     * @retval false  �L�����Z���ł��܂���ł���( ���łɓǂݍ��ݒ� )
     *
     * @note 
     * ���ɓǂݍ��ݒ��̃��N�G�X�g�̓L�����Z���ł��܂���B
     * IsReadFinished() ��, �ǂݍ��݂̏I����҂��Ă��������B
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::CancelReadRequest( void** ppDestBuffer )
    {
      u32 requestID = this->CalcRequestID( ppDestBuffer ); 
      if( this->IsRequestWaiting( requestID ) ) {
        this->CancelWaitingThreadRequest( requestID );
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���f�[�^�̓ǂݍ��݂������������H
     * @param[in] ppDestBuffer �Ǎ���o�b�t�@
     * @retval true  ��������
     * @retval false �������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::IsReadFinished( void** ppDestBuffer ) const
    {
      u32 requestID = this->CalcRequestID( ppDestBuffer );
      return this->IsRequestFinished( requestID );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �S�Ă̓ǂݍ��݂������������H
     * @retval true   ��������
     * @retval falsek �������Ă��Ȃ�
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::IsAllReadFinished( void ) const
    {
      return this->IsAllRequestFinished();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X�V����
     */
    //-------------------------------------------------------------------------
    void FileReadManager::UpdateFileRead( void )
    {
      // �X���b�h�Ǘ��������X�V
      this->UpdateThreadManagement();
    }




  }  // namespace fs
}  // namespace gfl
