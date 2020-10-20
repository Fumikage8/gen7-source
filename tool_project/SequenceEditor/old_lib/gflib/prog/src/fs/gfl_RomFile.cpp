//=======================================================================================
/**
 * @brief  ROM�t�@�C���A�N�Z�X
 * @file   gfl_RomFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include <nn.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_RomFile.h>



namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    RomFile::RomFile( void ) : 
      File(),
      m_nn_file_stream(),
      m_file_open_flag( false )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    RomFile::~RomFile( void )
    {
      this->Close();
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J��
     *
     * @param[in] path  �t�@�C���p�X (rom:/ �Ŏn�܂�p�X���w�肵�Ă�������)
     * @param[in] mode  �I�[�v�����[�h (OPEN_MODE_READ �̂ݗL���ł�)
     *
     * @retval true   �I�[�v���ɐ��������ꍇ
     * @retval false  �I�[�v���Ɏ��s�����ꍇ
     *
     * @note �p�X�̋�؂�ɂ� '/' ���g�p���Ă�������.
     */
    //-----------------------------------------------------------------------------------
    bool RomFile::Open( const wchar_t* path, OpenMode mode )
    {
      if( mode != OPEN_MODE_READ ) {  
        return false;  // ���̃N���X�͓ǂݍ��ݐ�p
      }

      if( this->IsOpen() ) { 
        this->Close();  // ���łɊJ���Ă���t�@�C�������
      }


      nn::Result result = m_nn_file_stream.TryInitialize( path, nn::fs::OPEN_MODE_READ );


#if GFL_FILE_DEBUG
      // profiler�̂��߂ɁA�p�X��ۑ��B
      m_file_path = path;
#endif // GFL_FILE_DEBUG

      // ���C���X���b�h�̏ꍇ �v���C�I���e�B��ݒ�
      if( base::Thread::IsMainThread( nn::os::Thread::GetCurrentId() ) ){
        this->SetPriority( FileSystem::GetMainThreadPriority() );
      }

      if( result.IsSuccess() )
      {
        m_file_open_flag = true;
        return true;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return false;  // ���s
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C�������
     */
    //-----------------------------------------------------------------------------------
    void RomFile::Close( void )
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
    size_t RomFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // �t�@�C�����J���Ă��Ȃ�
      }

#if GFL_FILE_DEBUG
      nn::os::Tick start = nn::os::Tick::GetSystemCurrent();
      s32 position = m_nn_file_stream.GetPosition();
#endif // GFL_FILE_DEBUG

      s32 read_size = 0;
      nn::Result result = m_nn_file_stream.TryRead( &read_size, buffer, size );

#if GFL_FILE_DEBUG
      if( FileSystem::IsProfiling() ){
        nn::os::Tick end = nn::os::Tick::GetSystemCurrent();
        nn::os::Tick ans = end - start;
        bit32 ID = nn::os::Thread::GetCurrentId();

        FileSystem::AddProfilerData( *m_file_path, ID, size, position, ans.ToTimeSpan().GetMicroSeconds() );
      }
#endif // GFL_FILE_DEBUG
      

      if( result.IsSuccess() )
      {
        return read_size;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
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
    void RomFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      nn::Result result = m_nn_file_stream.TrySeek( offset, nn_base );
      if( !result.IsSuccess() )
      {
        NN_ERR_THROW_FATAL_ALL(result);
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief �t�@�C���T�C�Y���擾����
     */
    //-----------------------------------------------------------------------------------
    size_t RomFile::GetSize( void ) const
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
        NN_ERR_THROW_FATAL_ALL(result);
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
    u32 RomFile::GetPosition( void ) const
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
        NN_ERR_THROW_FATAL_ALL(result);
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
    bool RomFile::CanRead( void ) const
    {
      if( !this->IsOpen() ) 
      {
        return false;  // �J���Ă��Ȃ�
      }
      else
      {
        return true;  // �J���Ă���Ȃ�, �ǂݍ��݂̂݉\
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
    bool RomFile::CanWrite( void ) const
    {
      // �������݂͏o���Ȃ�
      return false;
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
    bool RomFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // �t�@�C�����J���Ă��Ȃ�
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );
      if(result.IsSuccess())
      {
        return true;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return false;
      }
    }


  } // namespace fs
} // namespace gfl
