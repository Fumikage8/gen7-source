//=======================================================================================
/**
 * @brief  SD�J�[�h�t�@�C���A�N�Z�X
 * @file   gfl_HioFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include "gfl_fs.h"
#if GFL_HIO_ENABLE
#include <nn.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_HioFile.h>


namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    HioFile::HioFile( void ) : 
      File(),
      m_nn_host_file(),
      m_file_open_flag( false ),
      m_file_size( 0 ),
      m_current_position( 0 ),
      m_file_open_mode( OPEN_MODE_READ )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    HioFile::~HioFile( void )
    {
      this->Close();
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J��
     *
     * @param[in] path  �t�@�C���p�X (��΃p�X�Ŏw�肵�Ă�������)
     * @param[in] mode  �I�[�v�����[�h
     *
     * @retval true   �I�[�v���ɐ��������ꍇ
     * @retval false  �I�[�v���Ɏ��s�����ꍇ
     *
     * @note �p�X�̋�؂�ɂ� '/' ���g�p���Ă�������.
     */
    //-----------------------------------------------------------------------------------
    bool HioFile::Open( const wchar_t* path, OpenMode mode )
    {
      nn::hio::HostFile::AccessMode access_mode;
      nn::hio::HostFile::OpenDisp open_disp;

      switch( mode ) {
      case OPEN_MODE_READ:
        access_mode = nn::hio::HostFile::ACCESS_MODE_READ;
        open_disp = nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING;
        break;
      case OPEN_MODE_WRITE_NEW:
        access_mode = nn::hio::HostFile::ACCESS_MODE_WRITE;
        open_disp = nn::hio::HostFile::OPEN_DISP_CREATE_ALWAYS;
        break;
      case OPEN_MODE_WRITE_ADD:
        access_mode = nn::hio::HostFile::ACCESS_MODE_WRITE;
        open_disp = nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING;
        break;
      default:
        GFL_ASSERT(0);
        break;
      }

      this->Close();  // ���łɊJ���Ă���t�@�C�������
      nn::Result result = m_nn_host_file.Open( path, access_mode, open_disp ); 

      if( result.IsSuccess() )
      {
        m_file_open_flag = true;
        m_file_open_mode = mode;
        m_file_size = m_nn_host_file.Seek( 0, nn::hio::HostFile::SEEK_TYPE_END );
        m_nn_host_file.Seek( 0, nn::hio::HostFile::SEEK_TYPE_SET );
        m_current_position = 0;
        return true;
      }
      else
      {
        return false;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�������
     */
    //-----------------------------------------------------------------------------------
    void HioFile::Close( void )
    {
      if( this->IsOpen() ) 
      {
        m_file_open_flag = false;
        m_file_size = 0;
        m_current_position = 0;
        m_nn_host_file.Close();
      } 
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
    size_t HioFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;
      }

      size_t read_size = 0;
      nn::Result result = m_nn_host_file.Read( &read_size, buffer, size );

      if( result.IsSuccess() )
      {
        m_current_position += read_size;
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
    size_t HioFile::Write( const void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;
      }

      size_t written_size = 0;
      nn::Result result = m_nn_host_file.Write( &written_size, buffer, size );

      if( result.IsSuccess() )
      {
        m_current_position += written_size;
        if( m_file_size < m_current_position ) {
          m_file_size = m_current_position;
        }
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
    void HioFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      if( !this->IsOpen() ) {
        return;
      }

      nn::hio::HostFile::SeekType seek_type;

      switch( base_pos ) {
      case SEEK_BASE_BEGIN:
        seek_type = nn::hio::HostFile::SEEK_TYPE_SET;
        break;
      case SEEK_BASE_CURRENT:
        seek_type = nn::hio::HostFile::SEEK_TYPE_CURRENT;
        break;
      case SEEK_BASE_END:
        seek_type = nn::hio::HostFile::SEEK_TYPE_END;
        break; 
      default:
        GFL_ASSERT(0);
        break;
      }

      m_current_position = m_nn_host_file.Seek( offset, seek_type );

      if( m_current_position < 0 ) {  // �G���[����
        m_current_position = 0;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C���T�C�Y���擾����
     */
    //-----------------------------------------------------------------------------------
    size_t HioFile::GetSize( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;
      }
      return m_file_size;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief ���݈ʒu���擾����
     *
     * @return �t�@�C���擪����̃I�t�Z�b�g[Byte]
     */
    //-----------------------------------------------------------------------------------
    u32 HioFile::GetPosition( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;
      }
      return m_current_position;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �ǂݍ��݂��o���邩�ǂ����𒲂ׂ�
     *
     * @retval true   �ǂݍ��݉\
     * @retval false  �ǂݍ��ݕs�\
     */
    //-----------------------------------------------------------------------------------
    bool HioFile::CanRead( void ) const
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
    bool HioFile::CanWrite( void ) const
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
    bool HioFile::SetPriority( s32 /*priority*/ )
    {
      if( !this->IsOpen() ) {
        return false;  // �t�@�C�����J���Ă��Ȃ�
      }
      // HIO�ɂ͗D�揇�ʂ��Ȃ�

      return true;
    }


  } // namespace fs
} // namespace gfl
#endif // GFL_HIO_ENABLE
