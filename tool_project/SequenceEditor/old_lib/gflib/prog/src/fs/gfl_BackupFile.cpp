//=============================================================================
/**
 * @brief  �o�b�N�A�b�v�t�@�C���A�N�Z�X
 * @file   gfl_BackupFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=============================================================================
#include <nn.h>
#include <gfl_Base.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_BackupFile.h>


namespace gfl {
  namespace fs {


    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���̃A�N�Z�X���ʂ��擾����
     * @param nn_result nn::fs::TryXXXX�nAPI�̌���
     */
    //-------------------------------------------------------------------------
    BackupFile::Result BackupFile::GetAccessResult( const nn::Result& nn_result )
    {
      if( nn_result.IsSuccess() )
      {
        return ACCESS_RESULT_OK;
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
        if( nn_result <= nn::fs::ResultNotFound() )
        {
          return ACCESS_RESULT_NG_PATH_NOT_EXIST;
          // �w�肵���p�X�͑��݂��Ă��܂���B
          // ( �⑫ ) �t�@�C���E�f�B���N�g���쐬���ɂ́A
          //          �A�[�J�C�u�����Ԉ���Ă���ȊO�ɁA���̃G���[�͕Ԃ�܂���B
          //          ���̂悤�ȏ󋵂��������Ȃ��݌v�ł���΁A�n���h�����O�̕K�v�͂���܂���B
        }
        else if( nn_result <= nn::fs::ResultAlreadyExists() )
        {
          return ACCESS_RESULT_NG_PATH_ALREADY_EXIST;
          // �w�肵���p�X�́A���ɑ��݂��Ă��܂��B
          // ( �⑫ ) �t�@�C���E�f�B���N�g���폜���ɂ́A���̃G���[�͕Ԃ�܂���B
          //        ���̂悤�ȏ󋵂��������Ȃ��݌v�ł���΁A�n���h�����O�̕K�v�͂���܂���B
        }
        else if( nn_result <= nn::fs::ResultBadFormat() )
        {
          return ACCESS_RESULT_NG_BAD_FORMAT;
          // �f�o�b�K�Ńo�b�N�A�b�v���������̃f�[�^��j�󂷂�ȊO�̕��@�ŁA
          // �t�@�C���E�f�B���N�g�����쒆�ɂ��̃G���[���Ԃ邱�Ƃ͂���܂���B
          // ���i�łł̃n���h�����O�̕K�v�͂Ȃ��Ȃ�܂����B
          // ���̃t�@�C�� (�f�B���N�g��) �͍폜���āA��蒼���K�v������܂��B
          // ( ���R ) �t�@�C���t�H�[�}�b�g���s���ȏ�ԂɂȂ��Ă��܂��B
          // ( �⑫ ) �����������Ȃ��ꍇ�́A�Z�[�u�f�[�^���t�H�[�}�b�g����K�v������܂��B
        }
        else if( nn_result <= nn::fs::ResultVerificationFailed() )
        {
          return ACCESS_RESULT_NG_BROKEN_DATA;
          // ���̃t�@�C�� (�f�B���N�g��) �͍폜���āA��蒼���K�v������܂��B
          // ( ���R ) �Z�[�u�f�[�^�����Ă��邩�A��₂���Ă��܂��B
          // ( �⑫ ) ��d������Ă���ꍇ�́A���[�U����( �������ݒ��̓d���f��J�[�h������ )��
          //          ���̏�ԂɂȂ邱�Ƃ͂���܂���B
          //          ��d������Ă��Ȃ��ꍇ�́A���[�U����ł����̃G���[�͔������܂��B
          //          �����������Ȃ��ꍇ�́A�Z�[�u�f�[�^���t�H�[�}�b�g����K�v������܂��B
          // ( ���� ) ��x���������݂��s���Ă��Ȃ��̈��ǂݍ��񂾏ꍇ�A���̃G���[���Ԃ�܂��B
          //          nn::fs::TryCreateFile �֐���
          //          nn::fs::FileStream::TrySetSize �֐��Ȃǂ����s������́A���ӂ��Ă��������B
        }
        else if( nn_result <= nn::fs::ResultOperationDenied() )
        {
          return ACCESS_RESULT_NG_ERROR;
          // ���炩�̗��R�ŁA�A�N�Z�X�����ۂ���܂����B
          // ( �� ) ���ŊJ���Ă���t�@�C�����폜���悤�Ƃ����B
          //        �A�[�J�C�u���ׂ��Ńt�@�C����f�B���N�g�����ړ������悤�Ƃ����B
          //        ���[�g�f�B���N�g�����폜���悤�Ƃ����B
          // ( �⑫ ) ��L�̂悤�ȏ󋵂��������Ȃ��݌v�ł���΁A�n���h�����O�͕K�{�ł͂���܂���B
          //          �A���A�ڐG�s�Ǔ��ɂ��n�[�h�E�F�G�A�I�ȗv���ɂ�莸�s�����ꍇ�A
          //          nn::fs::ResultMediaAccessError ���Ԃ�܂��B
          //          ���̂Ƃ��́A���g���C�����A�Q�[���J�[�h�̑}�������A
          //          �{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
        }
        else if( nn_result <= nn::fs::ResultNotEnoughSpace() )
        {
          return ACCESS_RESULT_NG_LACK_OF_SPACE;
          // �A�[�J�C�u�A�܂��̓��f�B�A�̋󂫗e�ʂ�����܂���B
          // ( �⑫ ) �t�@�C���E�f�B���N�g���쐬���A�t�@�C���T�C�Y�ύX���ɔ�������\��������܂��B
          //          �t�@�C���쐬���ɂ��̃G���[�����������ꍇ�́A
          //          ���݃t�@�C�����쐬����Ă��邱�Ƃ�����܂��̂ŁA�폜����悤�ɂ��Ă��������B
          //          �t�@�C���T�C�Y�ύX���ɂ��̃G���[�����������ꍇ�́A
          //          �t�@�C���T�C�Y���\�Ȕ͈͂ŐL������Ă��邱�Ƃ�����܂��B
        }
        else if( nn_result <= nn::fs::ResultOutOfResource() )
        {
          //���\�[�X������܂���B���i�ł͔������Ȃ��悤�ɂ���ׂ��G���[�ł��B
          return ACCESS_RESULT_OUT_OF_RESOURCE;
        }
        else if( nn_result <= nn::fs::ResultAccessDenied() )
        {
          // �A�N�Z�X��������܂���B���i�ł͔������Ȃ��悤�ɂ���ׂ��G���[�ł��B
          return ACCESS_RESULT_ACCESS_DENIED;
        }
        else
        {
          GFL_PRINT( "===========================\n" );
          GFL_PRINT( "gfl_BackupFile Error[%d]\n", nn_result.GetDescription() );
          GFL_PRINT( "===========================\n" );
          return ACCESS_RESULT_NG_UNKNOWN;
        }
      }
    }






    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-------------------------------------------------------------------------
    BackupFile::BackupFile( void ) :
      File(),
      m_nn_file_stream(),
      m_file_open_flag( false ),
      m_file_open_mode( OPEN_MODE_READ ),
      m_access_result( ACCESS_RESULT_OK )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    BackupFile::~BackupFile( void )
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
     * �p�X�̓��ɂ� "data:/" ��t���Ă�������.
     * �p�X�̋�؂�ɂ� "/" ���g�p���Ă�������.
     */
    //-------------------------------------------------------------------------
    bool BackupFile::Open( const wchar_t* path, OpenMode mode )
    {
      // ���łɊJ���Ă���t�@�C�������
      if( this->IsOpen() ) {
        this->Close();
      }

      // �����̃t�@�C�����폜
      if( mode == OPEN_MODE_WRITE_NEW ) {
        nn::fs::TryDeleteFile( path );
      }

      bit32 nn_open_mode = this->GetNnOpenMode( mode );
      nn::Result nn_result = m_nn_file_stream.TryInitialize( path, nn_open_mode );
      m_access_result = GetAccessResult( nn_result );

      if( m_access_result == ACCESS_RESULT_OK )
      {
        m_file_open_flag = true;
        m_file_open_mode = mode;
        return true;
      }
      else
      {
        return false;  // ���s
      }
    }

    /**
     * @brief CTRSDK�̃I�[�v�����[�h���擾����
     */
    bit32 BackupFile::GetNnOpenMode( OpenMode mode ) const
    {
      bit32 nn_open_mode = 0;

      switch( mode ) {
      case OPEN_MODE_READ:
        nn_open_mode = nn::fs::OPEN_MODE_READ;
        break;
      case OPEN_MODE_WRITE_ADD:
      case OPEN_MODE_WRITE_NEW:
        nn_open_mode = nn::fs::OPEN_MODE_WRITE | nn::fs::OPEN_MODE_CREATE;
        break;
      }

      return nn_open_mode;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�������
     */
    //-------------------------------------------------------------------------
    void BackupFile::Close( void )
    {
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
    size_t BackupFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s32 read_size = 0;
      nn::Result nn_result = m_nn_file_stream.TryRead( &read_size, buffer, size );
      m_access_result = GetAccessResult( nn_result );
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
    size_t BackupFile::Write( const void* buffer, u32 size, bool flush /* = true */ )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s32 written_size = 0;
      nn::Result nn_result =
        m_nn_file_stream.TryWrite( &written_size, buffer, size, flush );  // �f�t�H���g�l��flush����
      m_access_result = GetAccessResult( nn_result );
      return ( m_access_result == ACCESS_RESULT_OK ) ? written_size : 0;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����V�[�N����
     *
     * @param[in] base_pos  �V�[�N�̋N�_�ƂȂ�ʒu
     * @param[in] offset    �N�_�ʒu����̃I�t�Z�b�g
     */
    //-------------------------------------------------------------------------
    void BackupFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      nn::Result nn_result = m_nn_file_stream.TrySeek( offset, nn_base );
      m_access_result = GetAccessResult( nn_result );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C���T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    size_t BackupFile::GetSize( void ) const
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
    u32 BackupFile::GetPosition( void ) const
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
    bool BackupFile::CanRead( void ) const
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
        GFL_ASSERT(0);  //@check ���Ή�
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
    bool BackupFile::CanWrite( void ) const
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
        GFL_ASSERT(0);  //@check ���Ή�
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
    bool BackupFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // �t�@�C�����J���Ă��Ȃ�
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );

      return result.IsSuccess();
    }


  } // namespace fs
} // namespace gfl
