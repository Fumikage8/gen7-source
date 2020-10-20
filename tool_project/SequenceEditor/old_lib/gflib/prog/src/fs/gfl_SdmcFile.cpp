//=======================================================================================
/**
 * @brief  SD�J�[�h�t�@�C���A�N�Z�X
 * @file   gfl_SdmcFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include "gfl_fs.h"
#if GFL_FILE_DEBUG
#include <nn.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_SdmcFile.h>


namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    SdmcFile::SdmcFile( void ) : 
      File(),
      m_nn_file_stream(),
      m_file_open_flag( false ),
      m_file_open_mode( OPEN_MODE_READ )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    SdmcFile::~SdmcFile( void )
    {
      this->Close();
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J��
     *
     * @param[in] path  �t�@�C���p�X (sdmc:/ �Ŏn�܂�p�X���w�肵�Ă�������)
     * @param[in] mode  �I�[�v�����[�h
     *
     * @retval true   �I�[�v���ɐ��������ꍇ
     * @retval false  �I�[�v���Ɏ��s�����ꍇ
     *
     * @note �p�X�̋�؂�ɂ� '/' ���g�p���Ă�������.
     */
    //-----------------------------------------------------------------------------------
    bool SdmcFile::Open( const wchar_t* path, OpenMode mode )
    {
      if( this->IsOpen() ) { 
        this->Close();  // ���łɊJ���Ă���t�@�C�������
      }

      // SD�J�[�h�̔����������s��ꂽ�ꍇ��z�肵, �}�E���g������
      // @todo �K�؂ȏꏊ�Ŏ��s���� > file system?
      nn::fs::Unmount( "sdmc" );
      nn::fs::MountSdmc();

      if( mode == OPEN_MODE_WRITE_NEW ) {
        nn::fs::TryDeleteFile( path );  // �����̃t�@�C�����폜
      }

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

      nn::Result result = m_nn_file_stream.TryInitialize( path, nn_open_mode );

      if( result.IsSuccess() )
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

    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�������
     */
    //-----------------------------------------------------------------------------------
    void SdmcFile::Close( void )
    {
      m_nn_file_stream.Finalize();
      m_file_open_flag = false;
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C������ǂݍ���
     *
     * @param[out] buffer  �ǂݍ��񂾃f�[�^�̊i�[��
     * @param[in]  size    �ǂݍ��ރT�C�Y
     *
     * @return ���ۂɓǂݍ��񂾃T�C�Y
     */
    //-----------------------------------------------------------------------------------
    size_t SdmcFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s32 read_size = 0;
      nn::Result result = m_nn_file_stream.TryRead( &read_size, buffer, size );

      if( result.IsSuccess() )
      {
        return read_size;
      }
      else
      {
        return 0;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C���ɏ�������
     *
     * @param[in] buffer  �������ރf�[�^
     * @param[in] size    �������ރT�C�Y
     *
     * @return ���ۂɏ������񂾃T�C�Y
     */
    //-----------------------------------------------------------------------------------
    size_t SdmcFile::Write( const void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s32 written_size = 0;
      nn::Result result = m_nn_file_stream.TryWrite( &written_size, buffer, size, true );  // Flash����

      if( result.IsSuccess() )
      {
        return written_size;
      }
      else
      {
        return 0;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����V�[�N����
     *
     * @param[in] base_pos  �V�[�N�̋N�_�ƂȂ�ʒu
     * @param[in] offset    �N�_�ʒu����̃I�t�Z�b�g
     */
    //-----------------------------------------------------------------------------------
    void SdmcFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      m_nn_file_stream.TrySeek( offset, nn_base );
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C���T�C�Y���擾����
     */
    //-----------------------------------------------------------------------------------
    size_t SdmcFile::GetSize( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s64 size = 0;
      nn::Result result = m_nn_file_stream.TryGetSize( &size );

      if( result.IsSuccess() )
      {
        return size;
      }
      else
      {
        return 0;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief ���݈ʒu���擾����
     *
     * @return �t�@�C���擪����̃I�t�Z�b�g[Byte]
     */
    //-----------------------------------------------------------------------------------
    u32 SdmcFile::GetPosition( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

      s64 pos = 0;
      nn::Result result = m_nn_file_stream.TryGetPosition( &pos );

      if( result.IsSuccess() )
      {
        return pos;
      }
      else
      {
        return 0;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݂��o���邩�ǂ����𒲂ׂ�
     *
     * @retval true   �ǂݍ��݉\
     * @retval false  �ǂݍ��ݕs�\
     */
    //-----------------------------------------------------------------------------------
    bool SdmcFile::CanRead( void ) const
    {
      if( !this->IsOpen() ) 
      {
        return false;  // �J���Ă��Ȃ�
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return true;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return false;
      default:
        GFL_ASSERT(0);  // ���Ή�
        return false;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �������݂��o���邩�ǂ����𒲂ׂ�
     *
     * @retval true   �������݉\
     * @retval false  �������ݕs�\
     */
    //-----------------------------------------------------------------------------------
    bool SdmcFile::CanWrite( void ) const
    {
      if( !this->IsOpen() ) 
      {
        return false;  // �J���Ă��Ȃ�
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return false;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return true;
      default:
        GFL_ASSERT(0);  // ���Ή�
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
    bool SdmcFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // �t�@�C�����J���Ă��Ȃ�
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );

      return result.IsSuccess();
    }


  } // namespace fs
} // namespace gfl

#endif // GFL_FILE_DEBUG
