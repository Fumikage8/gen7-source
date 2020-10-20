//=============================================================================
/**
 * @brief  �o�b�N�A�b�v�t�@�C���A�N�Z�X
 * @file   gfl_ExtSaveFile.cpp
 * @author obata_toshihiro
 * @author tamada
 * @date   2012.09.13
 *
 *  gfl_BackupFile.cpp����R�s�y�ō쐬
 */
//=============================================================================
#include <nn.h>
#include <gfl_Base.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_ExtSaveFile.h>


namespace gfl {
  namespace fs {


    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���̃A�N�Z�X���ʂ��擾����
     * @param nn_result nn::fs::TryXXXX�nAPI�̌���
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::Result ExtSaveFile::GetAccessResult( const nn::Result& nn_result )
    {
      if( nn_result.IsSuccess() )
      {
        return ACCESS_RESULT_OK;
      }

      if(nn_result <= nn::fs::ResultNotFound())
      {
        if(nn_result <= nn::fs::ResultMediaNotFound())
        {
          return ACCESS_RESULT_NG_MEDIA_NOT_FOUND;
           // SD �J�[�h���}�����Ă��܂���B
           // (�⑫) ��ꂽ SD �J�[�h�� SD �J�[�h�ł͂Ȃ����f�B�A���}����Ă���Ƃ��ɂ��A���̃G���[���Ԃ�܂��B
           //      �A���A���̏ꍇ�� RegisterSdmcInsertedEvent �œo�^�����}���C�x���g�̓V�O�i������܂��B
        }
        else
        {
          return ACCESS_RESULT_NG_PATH_NOT_EXIST;
           // �w�肵���p�X�͑��݂��Ă��܂���B
           // (�⑫) �t�@�C���I�[�v�����ɂ��̃G���[���Ԃ��Ă����ꍇ�́ASD ��̃t�@�C���� PC �Ȃǂō폜���ꂽ�\��������܂��B
           // (����) �g���Z�[�u�f�[�^�́A���̃A�v���P�[�V����������A�N�Z�X�����\�������邽�߁A
           //      ���̃G���[�̓n���h�����O����K�v������܂��B
        }
      }
      else if(nn_result <= nn::fs::ResultAlreadyExists())
      {
        return ACCESS_RESULT_NG_PATH_ALREADY_EXIST;
        // �w�肵���p�X�́A���ɑ��݂��Ă��܂��B
        // (�⑫) �f�B���N�g���폜���ɂ́A���̃G���[�͕Ԃ�܂���B
        //      �t�@�C���쐬���ɂ��̃G���[���Ԃ��Ă����ꍇ�́ASD ��̃t�@�C���� PC �Ȃǂō폜���ꂽ�\��������܂��B
        // (����) �g���Z�[�u�f�[�^�́A���̃A�v���P�[�V����������A�N�Z�X�����\�������邽�߁A
        //      ���̃G���[�̓n���h�����O����K�v������܂��B
      }
      else if(nn_result <= nn::fs::ResultVerificationFailed())
      {
        return ACCESS_RESULT_NG_BROKEN_DATA;
        // ���̃t�@�C�� (�f�B���N�g��) �͍폜���āA��蒼���K�v������܂��B
        // (���R) �g���Z�[�u�f�[�^�����Ă��邩�A��₂���Ă��܂��B
        // (�⑫) �����������Ȃ��ꍇ�́A�g���Z�[�u�f�[�^����蒼���K�v������܂��B
        // (����) ��x���������݂��s���Ă��Ȃ��̈��ǂݍ��񂾏ꍇ�A���̃G���[���Ԃ�܂��B
        //      nn::fs::TryCreateFile �֐��� nn::fs::FileStream::TrySetSize �֐��Ȃǂ����s������́A���ӂ��Ă��������B
      }
      else if(nn_result <= nn::fs::ResultArchiveInvalidated())
      {
        return ACCESS_RESULT_NG_ARCHIVE_INVALIDATED;
        // �A�[�J�C�u���}�E���g�������K�v������܂��B
        // (���R) SD �J�[�h�������ꂽ���Ƃɂ��A�A�[�J�C�u�������ɂȂ��Ă��܂��B
        // (�⑫) �J���Ă���t�@�C����S�ĕ��Ann::fs::Unmount �֐����Ăяo���Ă���}�E���g�������Ă��������B
      }
      else if(nn_result <= nn::fs::ResultOperationDenied())
      {
        if(nn_result <= nn::fs::ResultWriteProtected())
        {
          return ACCESS_RESULT_NG_WRITE_PROTECTED;
           // SD �J�[�h�����C�g���b�N����Ă��܂��B
           // (�⑫) ���̃G���[�́ASD �J�[�h�ɏ������݂��s���Ƃ��ɔ������܂��B�t�@�C�����I�[�v�����������ł͔������܂���B
        }
        else if(nn_result <= nn::fs::ResultMediaAccessError())
        {
          return ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR;
           // �ڐG�s�Ǔ��ɂ��n�[�h�E�F�A�I�ȗv���̂Ƃ��̂݁A���̃G���[���Ԃ�܂��B
           // ���̂Ƃ��́ASD �J�[�h�̑}�������A�{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
        }
        else
        {
           return ACCESS_RESULT_NG_ERROR;
           // ��L�ȊO�̉��炩�̗��R�ŁA�A�N�Z�X�����ۂ���܂����B
           // (��)  �����݂ł��Ȃ��t�@�C���ɏ����������Ƃ����B
           //      ���ŊJ���Ă���t�@�C�����폜���悤�Ƃ����B
           //      �A�[�J�C�u���ׂ��Ńt�@�C����f�B���N�g�����ړ������悤�Ƃ����B
           //      ���[�g�f�B���N�g�����폜���悤�Ƃ����B
           // (����) �g���Z�[�u�f�[�^�́A���̃A�v���P�[�V������������e�̉��ς��s���邽�߁i�^�p�ɂ���ĕς��܂��j�A
           //      �܂��A���[�U�� PC ���� SD �J�[�h���̃f�[�^�𑀍�ł��Ă��܂����Ƃ���A���̃G���[�͕K���n���h�����O����K�v������܂��B
        }
      }
      else if(nn_result <= nn::fs::ResultNotEnoughSpace())
      {
        return ACCESS_RESULT_NG_LACK_OF_SPACE;
        // �A�[�J�C�u�A�܂��̓��f�B�A�̋󂫗e�ʂ�����܂���B
        // (�⑫) �t�@�C���E�f�B���N�g���쐬���A�t�@�C���T�C�Y�ύX���ɔ�������\��������܂��B
        //      �t�@�C���쐬���ɂ��̃G���[�����������ꍇ�́A���݃t�@�C�����쐬����Ă��邱�Ƃ�����܂��̂ŁA�폜����悤�ɂ��Ă��������B
        //      �t�@�C���T�C�Y�ύX���ɂ��̃G���[�����������ꍇ�́A�t�@�C���T�C�Y���\�Ȕ͈͂ŐL������Ă��邱�Ƃ�����܂��B
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
      
        GFL_ASSERT( nn::fs::ResultOutOfResource::Includes( nn_result ) );         //@check �v���O�����~�X���o�p�F���i�łł̓A�v���Ŗ���`�G���[�������邱�Ƃ��]�܂���
        GFL_ASSERT( nn::fs::ResultAccessDenied::Includes( nn_result ) );          //@check �v���O�����~�X���o�p�F���i�łł̓A�v���Ŗ���`�G���[�������邱�Ƃ��]�܂���
        GFL_ASSERT( nn::fs::ResultInvalidArgument::Includes( nn_result ) );       //@check �v���O�����~�X���o�p�F���i�łł̓A�v���Ŗ���`�G���[�������邱�Ƃ��]�܂���
        GFL_ASSERT( nn::fs::ResultNotInitialized::Includes( nn_result ) );        //@check �v���O�����~�X���o�p�F���i�łł̓A�v���Ŗ���`�G���[�������邱�Ƃ��]�܂���
        GFL_ASSERT( nn::fs::ResultAlreadyInitialized::Includes( nn_result ) );    //@check �v���O�����~�X���o�p�F���i�łł̓A�v���Ŗ���`�G���[�������邱�Ƃ��]�܂���
        GFL_ASSERT( nn::fs::ResultUnsupportedOperation::Includes( nn_result ) );  //@check �v���O�����~�X���o�p�F���i�łł̓A�v���Ŗ���`�G���[�������邱�Ƃ��]�܂���
        // ���̊g���Z�[�u�f�[�^�A�������� SD �J�[�h�ɃA�N�Z�X�ł��܂���ł����B
        // �����ɊY������G���[�́A�܂Ƃ߂ăn���h�����O���s���Ă��������B(FATAL �G���[�\����ʂɂ͑J�ڂ����Ȃ��ł��������B)
        //GFL_ASSERT(0); // ��L�ȊO�̃G���[�͔������܂���B
        GFL_PRINT( "===========================\n" );
        GFL_PRINT( "gfl_ExtSaveFile Error[%d]\n", nn_result.GetDescription() );
        GFL_PRINT( "===========================\n" );
        return ACCESS_RESULT_NG_UNKNOWN;
      }
    }





    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::ExtSaveFile( void ) :
      File(),
      m_nn_file_stream(),
      m_file_open_flag( false ),
      m_file_open_mode( OPEN_MODE_READ ),
      //m_nn_result(static_cast<nn::Result>(0)),
      m_access_result( ACCESS_RESULT_OK ),
      m_need_flush( false )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::~ExtSaveFile( void )
    {
      this->Close();
    }








    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J��
     *
     * @param[in] path  �t�@�C���p�X
     * @param[in] mode  �I�[�v�����[�h (OPEN_MODE_READ �̂ݗL���ł�)
     *
     * @retval true   �I�[�v���ɐ��������ꍇ
     * @retval false  �I�[�v���Ɏ��s�����ꍇ
     *
     * @note
     * �p�X�̓��ɂ� "exdt:/" ��t���Ă�������.
     * �p�X�̋�؂�ɂ� "/" ���g�p���Ă�������.
     */
    //-------------------------------------------------------------------------
    bool ExtSaveFile::Open( const wchar_t* path, OpenMode mode )
    {
      // ���łɊJ���Ă���t�@�C�������
      if( this->IsOpen() ) {
        this->Close();
      }

      // �����̃t�@�C�����폜
      //if( mode == OPEN_MODE_WRITE_NEW ) {
      //  nn::fs::TryDeleteFile( path );
      //}

      bit32 nn_open_mode = this->GetNnOpenMode( mode );
      m_nn_result = m_nn_file_stream.TryInitialize( path, nn_open_mode );
      m_access_result = GetAccessResult( m_nn_result );

      if( m_access_result == ACCESS_RESULT_OK )
      {
        m_file_open_flag = true;
        m_file_open_mode = mode;
        return true;
      }
      else
      {
        //NN_DBG_PRINT_RESULT( m_nn_result );
        return false;  // ���s
      }
    }

    /**
     * @brief CTRSDK�̃I�[�v�����[�h���擾����
     */
    bit32 ExtSaveFile::GetNnOpenMode( OpenMode mode ) const
    {
      bit32 nn_open_mode = 0;

      switch( mode ) {
      case OPEN_MODE_READ:
        nn_open_mode = nn::fs::OPEN_MODE_READ;
        break;
      case OPEN_MODE_WRITE_ADD:
        nn_open_mode = nn::fs::OPEN_MODE_WRITE;
        break;
      case OPEN_MODE_WRITE_NEW:
        GFL_ASSERT( 0 );  // @check �g���Z�[�u�f�[�^�ł�FileStream����̃t�@�C�������͂ł��Ȃ�
        //nn_open_mode = nn::fs::OPEN_MODE_WRITE | nn::fs::OPEN_MODE_CREATE;
        break;
      }

      return nn_open_mode;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�������
     */
    //-------------------------------------------------------------------------
    void ExtSaveFile::Close( void )
    {
      if( m_need_flush )
      {
        m_nn_result = m_nn_file_stream.TryFlush();
        m_access_result = GetAccessResult( m_nn_result );
        m_need_flush = false;
      }
      m_nn_file_stream.Finalize();
      m_file_open_flag = false;
    }







    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C������ǂݍ���
     *
     * @param[out] buffer  �ǂݍ��񂾃f�[�^�̊i�[��
     * @param[in]  size    �ǂݍ��ރT�C�Y
     *
     * @return ���ۂɓǂݍ��񂾃T�C�Y
     */
    //-------------------------------------------------------------------------
    size_t ExtSaveFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s32 read_size = 0;
      m_nn_result = m_nn_file_stream.TryRead( &read_size, buffer, size );
      m_access_result = GetAccessResult( m_nn_result );
      return ( m_access_result == ACCESS_RESULT_OK ) ? read_size : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C���ɏ�������
     *
     * @param[in] buffer  �������ރf�[�^
     * @param[in] size    �������ރT�C�Y
     *
     * @return ���ۂɏ������񂾃T�C�Y
     */
    //-------------------------------------------------------------------------
    size_t ExtSaveFile::Write( const void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      GFL_PRINT("ExtSaveFile::Write\n");
      s32 written_size = 0;
      m_nn_result =
        //m_nn_file_stream.TryWrite( &written_size, buffer, size, true );  // flush����
        m_nn_file_stream.TryWrite( &written_size, buffer, size, false );  // flush���Ȃ�
      NN_DBG_PRINT_RESULT( m_nn_result );
      m_access_result = GetAccessResult( m_nn_result );
      if ( m_access_result == ACCESS_RESULT_OK )
      {
        m_need_flush = true;
      }
      return ( m_access_result == ACCESS_RESULT_OK ) ? written_size : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C���L���b�V�����f�o�C�X�ɏ����߂�
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::Result ExtSaveFile::Flush( void )
    {
      m_nn_result = m_nn_file_stream.TryFlush();
      NN_DBG_PRINT_RESULT( m_nn_result );
      m_access_result = GetAccessResult( m_nn_result );
      return m_access_result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����V�[�N����
     *
     * @param[in] base_pos  �V�[�N�̋N�_�ƂȂ�ʒu
     * @param[in] offset    �N�_�ʒu����̃I�t�Z�b�g
     */
    //-------------------------------------------------------------------------
    void ExtSaveFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      nn::Result m_nn_result = m_nn_file_stream.TrySeek( offset, nn_base );
      m_access_result = GetAccessResult( m_nn_result );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C���T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    size_t ExtSaveFile::GetSize( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s64 size = 0;
      nn::Result nn_result = m_nn_file_stream.TryGetSize( &size );
      return ( nn_result.IsSuccess() ) ? size : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���݈ʒu���擾����
     *
     * @return �t�@�C���擪����̃I�t�Z�b�g[Byte]
     */
    //-------------------------------------------------------------------------
    u32 ExtSaveFile::GetPosition( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s64 pos = 0;
      nn::Result nn_result = m_nn_file_stream.TryGetPosition( &pos );
      return ( nn_result.IsSuccess() ) ? pos : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݂��o���邩�ǂ����𒲂ׂ�
     *
     * @retval true   �ǂݍ��݉\
     * @retval false  �ǂݍ��ݕs�\
     */
    //-------------------------------------------------------------------------
    bool ExtSaveFile::CanRead( void ) const
    {
      if( !this->IsOpen() ) {
        return false;  // �J���Ă��Ȃ�
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return true;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return false;
      default:
        GFL_ASSERT(0);  // @check ���Ή�
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �������݂��o���邩�ǂ����𒲂ׂ�
     *
     * @retval true   �������݉\
     * @retval false  �������ݕs�\
     */
    //-------------------------------------------------------------------------
    bool ExtSaveFile::CanWrite( void ) const
    {
      if( !this->IsOpen() ) {
        return false;  // �J���Ă��Ȃ�
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return false;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return true;
      default:
        GFL_ASSERT(0);  // @check ���Ή�
        return false;
      }
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �t�@�C���ǂݍ��ݗD�揇�ʂ�ݒ肷��
     *
     *	@param	priority    �D�揇��
     *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// ���p���ӁI�I�I�@�h�L�������g��ǂ��ǂނ���
     *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// ����
     *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// �Ⴂ
     */
    //-----------------------------------------------------------------------------
    bool ExtSaveFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // �t�@�C�����J���Ă��Ȃ�
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );

      return result.IsSuccess();
    }


  } // namespace fs
} // namespace gfl
