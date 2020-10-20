//=============================================================================
/**
 * @brief  GF�A�[�J�C�u�t�@�C���̓ǂݍ���
 * @file   gfl_ArcFile.cpp
 * @author obata_toshihiro
 * @date   2011.03.20
 */
//=============================================================================
#include <gfl_debug.h>
#include "cx/gfl_StreamingUncomp.h"
#include "fs/gfl_RomFile.h"
#include "fs/gfl_ArcFile.h"
#include "./gfl_ArcFile_SetupThread.h"
#include "./ArcFileAccessor/ver.2.x/gfl_ArcFileAccessor.h"
#include "./ArcFileAccessor/ver.3.x/gfl_ArcFileAccessor.h"
#include "./ArcFileAccessor/ver.4.x/gfl_ArcFileAccessor.h"


namespace gfl {
  namespace fs {


    u32 ArcFile::LANGUAGE_CODE = 0;
    size_t ArcFile::ARCFILE_NUM = 0;
    const wchar_t** ArcFile::FILENAME_TABLE = NULL; 
#if GFL_FILE_DEBUG
    const wchar_t** ArcFile::SD_FILENAME_TABLE = NULL;
    bool ArcFile::m_isEnableSd = false;
#endif
#if GFL_HIO_ENABLE
    const wchar_t** ArcFile::HIO_FILENAME_TABLE = NULL; 
#endif


    //-------------------------------------------------------------------------
    /**
     * @brief ����R�[�h��ݒ肷��
     * @param language  �ݒ肷�錾��R�[�h
     */
    //-------------------------------------------------------------------------
    void ArcFile::SetLanguage( u32 language )
    {
      LANGUAGE_CODE = language;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�u�t�@�C���̖��O�e�[�u����ݒ肷��
     * @param filename_table  �A�[�J�C�uID�ɑΉ�����t�@�C�����̔z��
     * @param arcfile_num     �t�@�C�����z��̗v�f��
     */
    //-------------------------------------------------------------------------
    void ArcFile::SetFileNameTable( const wchar_t** filename_table, size_t arcfile_num )
    {
      FILENAME_TABLE = filename_table;
      ARCFILE_NUM = arcfile_num;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�uID�ɑΉ�����t�@�C�������擾����
     * @param arcid  �t�@�C�������擾����A�[�J�C�uID
     * @return �Ή�����ROM�A�[�J�C�u��̃t�@�C����
     */
    //-------------------------------------------------------------------------
    const wchar_t* ArcFile::GetRomFileName( ARCID arcid )
    {
      return GetFileName( arcid, FILENAME_TABLE, ARCFILE_NUM );
    }

    /**
     * @brief �A�[�J�C�uID�ɑΉ�����t�@�C�������擾����
     * @param arcid          �A�[�J�C�uID
     * @param filename_table �t�@�C�����e�[�u��
     * @param table_size     �t�@�C�����e�[�u���̗v�f��
     */
    const wchar_t* ArcFile::GetFileName( 
      ARCID arcid,
      const wchar_t** filename_table,
      u32 table_size )
    {
      GFL_ASSERT_MSG( ( arcid < table_size ), "�s���ȃA�[�J�C�uID�ł��B(%d)\n", arcid ); //@check
      GFL_ASSERT_MSG( ( filename_table != NULL ), "�t�@�C�����e�[�u�����Z�b�g����Ă��܂���B\n" );//@check
      return filename_table[ arcid ];
    }

#if GFL_FILE_DEBUG
    //SD�J�[�h�ǂݍ��݋@�\
    /**
     * @brief Arc��SD�J�[�h����ǂݍ��ނ��߂̃t�@�C�����e�[�u���̐ݒ�
     * @param filename_table  �A�[�J�C�uID�ɑΉ�����t�@�C�����̔z��
     */
    void ArcFile::SetSDMountFileNameTable( const wchar_t** filename_table )
    {
      SD_FILENAME_TABLE = filename_table;
    }

    /**
     * @brief Arc��SD�J�[�h����ǂݍ��ނ��̃t���O(�t�@�C�������݂��Ȃ���Βʏ�̓ǂݍ��݂����܂�)
     * @param bool �L���E����
     */
    void ArcFile::SetSDMountIsEnable( bool flg )
    {
      m_isEnableSd = flg;
    }

    /**
     * @brief Arc��SD�J�[�h����ǂݍ��ނ��̃t���O�̎擾
     * @return bool �L���E����
     */
    bool ArcFile::GetSDMountIsEnable( void )
    {
      return m_isEnableSd;
    }

      /**
       * @brief Arc��SD�J�[�h����ǂݍ��ގ��ɑΉ�����t�@�C�������擾����
       * @param arcid  �t�@�C�������擾����A�[�J�C�uID
       * @return �Ή�����A�[�J�C�u��̃t�@�C����
       */
    const wchar_t* ArcFile::GetSDMountFileName( ARCID arcid )
    {
      return GetFileName( arcid, SD_FILENAME_TABLE, ARCFILE_NUM );
    }

#endif

#if GFL_HIO_ENABLE
    //-------------------------------------------------------------------------
    /**
     * @brief HIO�A�N�Z�X���邽�߂̃t�@�C�����e�[�u����ݒ肷��
     * @param filename_table  �A�[�J�C�uID�ɑΉ�����t�@�C�����̔z��
     */
    //-------------------------------------------------------------------------
    void ArcFile::SetHioFileNameTable( const wchar_t** filename_table )
    {
      HIO_FILENAME_TABLE = filename_table;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�uID�ɑΉ�����t�@�C�������擾����
     * @param arcid  �t�@�C�������擾����A�[�J�C�uID
     * @return �Ή�����z�X�g�t�@�C���V�X�e����̃t�@�C����
     */
    //-------------------------------------------------------------------------
    const wchar_t* ArcFile::GetHioFileName( ARCID arcid )
    {
      return GetFileName( arcid, HIO_FILENAME_TABLE, ARCFILE_NUM );
    }
#endif





    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap       ��ƃ��[�N�Ɏg�p����q�[�v
     * @param arcID      �֘A�t����A�[�J�C�u�t�@�C��
     * @param init_flag  �R���X�g���N�^�ōs�������� InitBit �̘_���a�Ŏw��
     */
    //-------------------------------------------------------------------------
    ArcFile::ArcFile( gfl::heap::HeapBase* heap, ARCID arcID, u32 init_flag ) :
      m_workHeap( heap ),
      m_file(),
      m_filename( NULL ),
      m_dataNum( 0 ),
      m_maxDataSize( 0 ),
      m_fastAccessTable( NULL ),
      m_setupThread( NULL ),
      m_fileOpenEvent(),
      m_fastSetupEvent(),
      m_criticalSectionForFileOpen(),
      m_criticalSectionForFileRead(),
      m_criticalSectionForFastSetup()
#if GFL_HIO_ENABLE
      ,m_isHioFile( false )
#endif
#if GFL_DEBUG
      ,m_majorVersion( 0 )
      ,m_minorVersion( 0 )
#endif
    {
#if GFL_FILE_DEBUG
      //SD�J�[�h�ɃA�[�J�C�u�����邩�H
      if( m_isEnableSd && SD_FILENAME_TABLE )
      {
        GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
        //���̃t�@�C�������邩���`�F�b�N���A���ۂɓǂݍ��ނ������߂�B
        if( gfl::fs::FileSystem::CheckRomFileExist( SD_FILENAME_TABLE[ arcID ] ) )
        {
          m_filename = SD_FILENAME_TABLE[ arcID ];
          ARIIZUMI_PRINT("SD�J�[�h����A�[�N�����I[%ls]\n",m_filename);
        }
      }
#endif
      //SD�J�[�h���ǂ܂�Ă�����ǂ܂Ȃ�
      if( m_filename == NULL )
      {
#if GFL_HIO_ENABLE
        if( init_flag & HIO ) {
          if( HIO_FILENAME_TABLE )
          {

            GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
            m_filename = HIO_FILENAME_TABLE[ arcID ];
            //���̃t�@�C�������邩���`�F�b�N���A���ۂɓǂݍ��ނ������߂�B
            if( gfl::fs::FileSystem::CheckHioFileExist( m_filename ) ){
              m_isHioFile = true;
            }
          }
          if( m_isHioFile == false )
          {
            // HIO����
            TOMOYA_PRINT( "�t�@�C�����Ȃ�HIO���� %ls\n", m_filename );
            m_filename = FILENAME_TABLE[ arcID ];
            m_isHioFile = false;
          }
        }
        else {
          GFL_ASSERT( FILENAME_TABLE );  //@check
          GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
          m_filename = FILENAME_TABLE[ arcID ];
          m_isHioFile = false;
        }
#else
        GFL_ASSERT( FILENAME_TABLE );  //@check
        GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
        m_filename = FILENAME_TABLE[ arcID ];
#endif
      }

      this->InitExclusiveObjects();

      if( init_flag & OPEN ) {
        this->Open();
      }

      if( (init_flag & OPEN_FASTMODE) == OPEN_FASTMODE ) {
        this->SetupForFastMode( m_workHeap );
      }
    }

    /**
     * @brief �r������I�u�W�F�N�g������������
     */
    void ArcFile::InitExclusiveObjects( void )
    {
      m_fileOpenEvent.Initialize( true );  // �������Z�b�g���Ȃ�
      m_fastSetupEvent.Initialize( true );  // �������Z�b�g���Ȃ�
      m_criticalSectionForFileOpen.Initialize();
      m_criticalSectionForFileRead.Initialize();
      m_criticalSectionForFastSetup.Initialize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap       ��ƃ��[�N�Ɏg�p����q�[�v
     * @param filename   �֘A�t����A�[�J�C�u�t�@�C��
     * @param init_flag  �R���X�g���N�^�ōs�������� InitBit �̘_���a�Ŏw��
     */
    //-------------------------------------------------------------------------
    ArcFile::ArcFile( gfl::heap::HeapBase* heap, const wchar_t* filename, u32 init_flag ) :
      m_workHeap( heap ),
      m_file(),
      m_filename( filename ),
      m_dataNum( 0 ),
      m_maxDataSize( 0 ),
      m_fastAccessTable( NULL ),
      m_setupThread( NULL ),
      m_fileOpenEvent(),
      m_fastSetupEvent(),
      m_criticalSectionForFileOpen(),
      m_criticalSectionForFileRead(),
      m_criticalSectionForFastSetup()
#if GFL_HIO_ENABLE
      ,m_isHioFile( false )
#endif
#if GFL_DEBUG
      ,m_majorVersion( 0 )
      ,m_minorVersion( 0 )
#endif
    {
#if GFL_HIO_ENABLE
      m_isHioFile = ( (init_flag & HIO) != 0 );
#endif
      this->InitExclusiveObjects();

      if( init_flag & OPEN ) {
        this->Open();
      }

      if( (init_flag & OPEN_FASTMODE) == OPEN_FASTMODE ) {
        this->SetupForFastMode( m_workHeap );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J��
     */
    //-------------------------------------------------------------------------
    void ArcFile::Open( void )
    {
      // �����C���X�^���X�ɂ��āA
      // �����̃X���b�h�������� Open() ���Ă΂��\��������B
      // �������AOpen() �͔r���I�Ɏ��s����K�v�����邽�߁A
      // 1��ڂ̎��s���ɃN���e�B�J���Z�N�V���������b�N����B
      // �Ȃ��A1�񂾂����s����Ηǂ��͂��Ȃ̂ŁA�ȍ~�A�N���e�B�J���Z�N�V�����̓A�����b�N���Ȃ��B
      if( m_criticalSectionForFileOpen.TryEnter() )
      {
        if( this->OpenFile( m_filename ) )
        {
          m_fileOpenEvent.Signal();
        }
      }
    } 

    /**
     * @brief �t�@�C�����J��
     * @param filename �J���t�@�C���̃p�X
     */
    bool ArcFile::OpenFile( const wchar_t* filename )
    {
      bool result = m_file.Open( filename, OPEN_MODE_READ );
      if( !result ) {
        GFL_PRINT( "�t�@�C���I�[�v���Ɏ��s���܂����B( %ls )\n", filename );
        return false;
      }

#if GFL_DEBUG


      // �f�o�b�O�ł̂�, 
      // �t�@�C���V�O�l�`���ƃt�@�C���o�[�W�����̊m�F���s��.
      // �Ȃ�, �m�F�ɂ̓t�@�C���A�N�Z�X���K�v�ƂȂ邽��, �������x���l�����ăf�o�b�O�łł̂ݍs��.
      //
      // �����[�X�łł�,
      // �A�[�J�C�u�t�@�C���ȊO�̃t�@�C�����I�[�v�������R�[�h�͔r�������ׂ�.
      // �܂�, �I�[�v�������A�[�J�C�u�t�@�C����, ���ׂčŐV�̃o�[�W�����ł���ׂ�.

      // �t�@�C���V�O�l�`���̊m�F
      {
        char signature[4];
        this->SeekFile( 0 );
        this->LoadDataPtr( 4, &signature );
        if( ( signature[0] != 'C' ) || 
            ( signature[1] != 'R' ) || 
            ( signature[2] != 'A' ) || 
            ( signature[3] != 'G' ) ) {
          GFL_PRINT( ".garc�ȊO�̃t�@�C����ǂݍ��݂܂����B\n" );
          GFL_ASSERT(0);  //@check
        }
      }

      // �t�@�C���o�[�W�����̊m�F
      {
        const u16 LATEST_MAJOR_VERSION = 4;
        const u16 SUPPORTED_MAJOR_VERSION[] = { 2, 3, 4 };  // ver.2.x, ver.3.x ver.4.x ���T�|�[�g�Ώ�

        u16 version;
        this->SeekFile( 10 );
        this->LoadDataPtr( sizeof(u16), &version );
        m_majorVersion = ( version >> 8 );
        m_minorVersion = version & 0x00ff;

        bool supported = false;
        for( u32 i=0; i<GFL_NELEMS(SUPPORTED_MAJOR_VERSION); i++ ) {
          if( m_majorVersion == SUPPORTED_MAJOR_VERSION[i] ) {
            supported = true;
            break;
          }
        }

        if( !supported ) 
        {
          GFL_PRINT( "WARNING!! �A�[�J�C�u�t�@�C�� %ls �� ver.%d.%d �ŁA���C�u�����̃T�|�[�g�͏I�����Ă��܂��Bver.%d.x �ɍăA�[�J�C�u���Ă��������B\n",
              filename, m_majorVersion, m_minorVersion, LATEST_MAJOR_VERSION );
          GFL_ASSERT(0);  //@check
        }
        else if( m_majorVersion != LATEST_MAJOR_VERSION ) 
        {
          GFL_PRINT( "WARNING!! �A�[�J�C�u�t�@�C�� %ls �� ver.%d.%d �ŁA���C�u�����̃T�|�[�g�͏I���\��ł��Bver.%d.x �ɍăA�[�J�C�u���Ă��������B\n",
              filename, m_majorVersion, m_minorVersion, LATEST_MAJOR_VERSION );
        }
      }

#endif 

      return true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J���Ă��邩�ǂ����𒲂ׂ�
     * @retval true
     * @retval false  
     */
    //-------------------------------------------------------------------------
    bool ArcFile::IsOpen( void ) const 
    { 
      return ( m_fileOpenEvent.IsSignaled() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����J�������_�ŃV�O�i����ԂɂȂ�C�x���g�I�u�W�F�N�g���擾����
     * @note ��x�V�O�i����ԂɂȂ�����A�ȍ~���Z�b�g�͂���܂���B
     * @attention �擾�����C�x���g�I�u�W�F�N�g�̏�Ԃ�ύX���Ȃ��ł��������B
     */
    //-------------------------------------------------------------------------
    nn::os::LightEvent* ArcFile::GetOpenEvent( void )
    {
      return &m_fileOpenEvent;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    ArcFile::~ArcFile( void )
    {
      GFL_ASSERT( !this->IsSettingUpForFastMode() ); //@check
      this->EndSetupForFastMode();

      GFL_ASSERT( !this->CheckAsyncRead() );//@check
      this->EndAsyncRead();

      GFL_SAFE_DELETE_ARRAY( m_fastAccessTable );
      m_file.Close();

      m_fileOpenEvent.Finalize();
      m_fastSetupEvent.Finalize();
      m_criticalSectionForFileOpen.Finalize();
      m_criticalSectionForFileRead.Finalize();
      m_criticalSectionForFastSetup.Finalize();
    }





    //-------------------------------------------------------------------------
    /**
     * @brief �f�[�^1���̓ǂݍ��݃|�C���^���V�[�N����( ��舵�����ӁI)
     * @param[in] data_id            �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in] offset_from_data   �V�[�N�T�C�Y( byte, �f�[�^�擪����̃I�t�Z�b�g)
     */
    //-------------------------------------------------------------------------
    void ArcFile::SeekDataPtr( ARCDATID data_id, u32 offset_from_data )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      this->SeekFile( access_info.offsetFromArchiveTop + offset_from_data );
    } 

    /**
     * @brief �t�@�C���V�[�N����
     * @param offset_from_top  �t�@�C���擪����̃I�t�Z�b�g
     */
    void ArcFile::SeekFile( size_t offset_from_top )
    {
      m_file.Seek( SEEK_BASE_BEGIN, offset_from_top );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief SeekDataPtr()�ŃV�[�N�����|�C���^����f�[�^�ǂݍ���( ��舵�����ӁI)
     * @param[in]  size         �ǂݍ��݃T�C�Y
     * @param[out] dest_buffer  �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
     * @retval ���ۂɓǂݍ��񂾃o�C�g��
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::LoadDataPtr( u32 size, void* dest_buffer )
    {
      size_t read_size = m_file.Read( dest_buffer, size );
      return read_size;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�[�^��1�����ǂݍ���
     * @param[in]  data_id      �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[out] dest_buffer  �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
     * @retval ���ۂɓǂݍ��񂾃o�C�g��
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::LoadData( ARCDATID data_id, void* dest_buffer )
    { 
      size_t load_size = 0;
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      this->ExclusiveRead( access_info.offsetFromArchiveTop, access_info.dataSize, dest_buffer, &load_size );
      return load_size;
    }

    /**
     * @brief �r���I�Ƀt�@�C����ǂ�
     * @param[in]  readPos     �ǂݍ��ވʒu
     * @param[in]  readSize    �ǂݍ��ރT�C�Y
     * @param[out] pDestBuffer �ǂݍ��񂾃f�[�^�̊i�[��
     * @param[out] pReadSize   �ǂݍ��񂾃T�C�Y( �s�v�Ȃ�NULL )
     */
    void ArcFile::ExclusiveRead( u32 readPos, u32 readSize, void* pDestBuffer, u32* pReadSize )
    {
      m_criticalSectionForFileRead.Enter();
      {
        this->SeekFile( readPos );
        size_t load_size = this->LoadDataPtr( readSize, pDestBuffer );
        if( pReadSize ) {
          *pReadSize = load_size;
        }
      }
      m_criticalSectionForFileRead.Leave();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
     * @param[in]		  data_id	          �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in]		  heap		          �𓀍�Ɨp�q�[�v
     * @param[in,out]	buffer	          �𓀃f�[�^�̓W�J��o�b�t�@
     * @param[in]	    buffer_size       �𓀃f�[�^�̓W�J��o�b�t�@�̃T�C�Y
     * @param[out]	  uncompressed_size �W�J�f�[�^�̃T�C�Y
     */
    //-------------------------------------------------------------------------
    void ArcFile::LoadData( 
        ARCDATID data_id,
        gfl::heap::HeapBase* heap, 
        void* buffer,
        size_t buffer_size,
        size_t* uncompressed_size )
    {
      this->LoadCompressedData( data_id, heap, buffer, buffer_size, uncompressed_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
     * @param[in]		  data_id	          �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in]		  heap		          �𓀍�Ɨp�q�[�v
     * @param[in,out]	buffer	          �𓀃f�[�^�̓W�J��o�b�t�@
     * @param[in]	    buffer_size       �𓀃f�[�^�̓W�J��o�b�t�@�̃T�C�Y
     * @param[out]	  uncompressed_size �W�J�f�[�^�̃T�C�Y
     */
    //-------------------------------------------------------------------------
    void ArcFile::LoadCompressedData( 
        ARCDATID data_id,
        gfl::heap::HeapBase* heap, 
        void* buffer,
        size_t buffer_size,
        size_t* uncompressed_size )
    {
      // ���k�f�[�^��ǂݍ���
			size_t comp_size = this->GetDataSize( data_id ); 
			void * comp_data = GflHeapAllocMemoryLowAlign( heap, comp_size, 4 );
			s32 result = this->LoadData( data_id, comp_data );

      if( 0 < result ) {
        this->Uncompress( comp_data, comp_size, buffer, buffer_size, uncompressed_size ); // �w�肳�ꂽ�o�b�t�@�ɓW�J����
      }
      else {
        GFL_ASSERT(0);  //@fix
      }

      GflHeapFreeMemory( comp_data );
    }

    /**
     * @brief ���k�f�[�^���𓀂���
     * @param[in]     compressed_data   ���k�f�[�^
     * @param[in]     compressed_size   ���k�f�[�^�̃T�C�Y
     * @param[in,out]	buffer	          �𓀃f�[�^�̓W�J��o�b�t�@
     * @param[in]	    buffer_size       �𓀃f�[�^�̓W�J��o�b�t�@
     * @param[out]	  uncompressed_size �W�J�f�[�^�̃T�C�Y
     */
    void ArcFile::Uncompress( 
        void* compressed_data, 
        size_t compressed_size,
        void* buffer,
        size_t buffer_size,
        size_t* uncompressed_size )
    {
			size_t uncomp_size = nn::cx::GetUncompressedSize( compressed_data ); 

      // ���f�[�^�T�C�Y��0�Ȃ�ǂݍ��܂Ȃ�
      if( compressed_data == 0 ) {
        return; 
      } 

      // �o�b�t�@�T�C�Y������Ȃ�
      if( buffer_size < uncomp_size ) {
        GFL_PRINT( "�o�b�t�@�T�C�Y������܂���(buffer=0x%x, data=0x%x)\n", buffer_size, uncomp_size );
        GFL_ASSERT(0);  //@fix
        return; 
      }

			s32 result = gfl::cx::SecureUncompress( compressed_data, compressed_size, buffer, m_workHeap );
      GFL_ASSERT( result == 0 ); //@check

      if( uncompressed_size != NULL ) {
        *uncompressed_size = uncomp_size;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
     * @param[in]		datID			�A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in]		heap			�q�[�v
     * @param[in]		align			�W�J�f�[�^�̃A���C��
     * @param[out]	pSize			�W�J�f�[�^�̃T�C�Y
     * @return	�W�J�����f�[�^
     */
    //-------------------------------------------------------------------------
    void * ArcFile::LoadData( 
        ARCDATID datID,
        gfl::heap::HeapBase * heap,
        int align,
        size_t * pSize )
    {
      return this->LoadCompressedDataAlloc( datID, heap, heap, align, pSize );
		}

    //-------------------------------------------------------------------------
    /**
     * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
     * @param[in]		datID			  �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in]		heapForWork	�𓀍�ƂɎg�p����q�[�v
     * @param[in]		heapForData	�𓀃f�[�^���m�ۂ���q�[�v
     * @param[in]		align				�W�J�f�[�^�̃A���C��
     * @param[out]	pSize				�W�J�f�[�^�̃T�C�Y
     * @return	�W�J�����f�[�^
     */
    //-------------------------------------------------------------------------
    void * ArcFile::LoadCompressedDataAlloc( 
        ARCDATID datID, 
        gfl::heap::HeapBase * heapForWork,
        gfl::heap::HeapBase * heapForData,
        int align,
        size_t * pSize )
    {
      // ���k�f�[�^��ǂݍ���
			size_t comp_size = this->GetDataSize( datID );
			void * comp_data = GflHeapAllocMemoryLowAlign( heapForWork, comp_size, 4 );
			s32 loaded_size = this->LoadData( datID, comp_data );

			void * uncomp_data = NULL;
      if( 0 < loaded_size )
      {
        // �𓀃o�b�t�@���m�ۂ��ēW�J����
        size_t uncomp_size = nn::cx::GetUncompressedSize( comp_data );
        uncomp_data = GflHeapAllocMemoryAlign( heapForData, uncomp_size, align );
        this->Uncompress( comp_data, comp_size, uncomp_data, uncomp_size, pSize );
      }
      else
      {
        GFL_ASSERT(0); //@fix
      }

			GflHeapFreeMemory( comp_data );
			return uncomp_data;
		}

    //-------------------------------------------------------------------------
    /**
     * @brief �f�[�^1���̈ꕔ��ǂݍ���
     * @param[in]  data_id           �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in]  offset_from_data  �ǂݍ��݊J�n�I�t�Z�b�g(byte, �f�[�^�擪����̃I�t�Z�b�g)
     * @param[in]  read_size         �ǂݍ��݃T�C�Y(byte)
     * @param[out] dest_buffer       �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
     */
    //-------------------------------------------------------------------------
    void ArcFile::LoadDataPiece(
        ARCDATID data_id, u32 offset_from_data, u32 read_size, void* dest_buffer )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      this->ExclusiveRead( access_info.offsetFromArchiveTop + offset_from_data, read_size, dest_buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�[�^1�����̃T�C�Y���擾����
     * @param data_id  �A�[�J�C�u�t�@�C�����f�[�^ID
     *
     * @note 
     * �ʏ탂�[�h�̏ꍇ, 
     * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
     */
    //-------------------------------------------------------------------------
    u32 ArcFile::GetDataSize( ARCDATID data_id )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      return access_info.dataSize;
    } 

    /**
     * @brief �A�[�J�C�u���f�[�^�ւ̃A�N�Z�X����K�؂ȏꏊ����擾����
     * @param[in]  data_id      �t�@�C�����f�[�^ID
     * @param[out] access_info  �擾�����A�N�Z�X���̊i�[��
     *
     * @note �ʏ탂�[�h�̏ꍇ
     * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
     */
    void ArcFile::GetAccessInfo( ARCDATID data_id, AccessInfo* access_info )
    { 
      if( this->IsFastMode() )
      {
        this->GetAccessInfoFromTable( data_id, access_info );
      } 
      else 
      {
        size_t total_block_size = this->ReadTotalBlockSize();
        void* block_data = GFL_NEW_LOW_ARRAY( m_workHeap ) u8[ total_block_size ];
        this->GetAccessInfoFromFile_Fast( data_id, access_info, total_block_size, block_data );
        GFL_DELETE_ARRAY block_data;
      }
    } 

    /**
     * @brief �A�[�J�C�u���f�[�^�ւ̃A�N�Z�X�����e�[�u������擾����
     * @param[in]  data_id      �t�@�C�����f�[�^ID
     * @param[out] access_info  �擾�����A�N�Z�X���̊i�[��
     */
    void ArcFile::GetAccessInfoFromTable( ARCDATID data_id, AccessInfo* access_info )
    {
      GFL_ASSERT( this->IsFastMode() );  //@check �������[�h�łȂ��ƃ_��
      GFL_ASSERT( m_fastAccessTable );  //@check �������[�h�Ȃ玝���Ă���͂�
      GFL_ASSERT_STOP_MSG( data_id < m_dataNum, "ArcName[%ls] DataId[%d]\n",m_filename,data_id);  //@check �������[�h�Ȃ琳�����t�@�C�����������Ă���͂�
      *access_info = m_fastAccessTable[ data_id ];
    } 

    /**
     * @brief �A�[�J�C�u���f�[�^�ւ̃A�N�Z�X�����t�@�C������擾����( ����, �v��ƃq�[�v�� )
     * @param[in]     data_id           �t�@�C�����f�[�^ID
     * @param[out]    access_info       �擾�����A�N�Z�X���̊i�[��
     * @param[in]     total_block_size  �A�[�J�C�u�t�@�C���̃u���b�N�̍��v�T�C�Y
     * @param[in,out] temp_buffer       ��ƃ��[�N( total_block_size�ȏ�̑傫�����K�v )
     */
    void ArcFile::GetAccessInfoFromFile_Fast( 
        ARCDATID data_id, AccessInfo* access_info, size_t total_block_size, void* temp_buffer )
    {
      this->ExclusiveRead( 0, total_block_size, temp_buffer );
      IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
      accessor->AttachData( temp_buffer );
      GFL_ASSERT_STOP( data_id < accessor->GetFileIdNum() ); //@check
      access_info->offsetFromArchiveTop = accessor->GetOffsetToData( data_id, LANGUAGE_CODE );
      access_info->dataSize = accessor->GetDataSize( data_id, LANGUAGE_CODE );
      access_info->realSize = accessor->GetRealDataSize( data_id, LANGUAGE_CODE );
      GFL_DELETE accessor;
    }

#if 0
    // �A�[�J�C�u�t�@�C���ɒ��ڃA�N�Z�X���邽�߂̃I�t�Z�b�g���
    //const size_t ArcFile::OFFSET_TO_TOTAL_BLOCK_SIZE = 16;
    //const size_t ArcFile::OFFSET_TO_FILE_ID_NUM = 32;
    //const size_t ArcFile::OFFSET_TO_ALLOCATION_OFFSET = 36;
    //const size_t ArcFile::OFFSET_TO_FILE_NUM_FROM_ALLOCATION_TABLE_BLOCK = 8;
    //const size_t ArcFile::ALLOCATION_OFFSET_ELEMENT_SIZE = 4;
    //const size_t ArcFile::ALLOCATION_TABLE_BLOCK_HEADER_SIZE = 12;
    //const size_t ArcFile::ALLOCATION_TABLE_HEADER_SIZE = 4;
    //const size_t ArcFile::ALLOCATION_TABLE_ELEMENT_SIZE = 8;
    //const size_t ArcFile::FILE_IMAGE_BLOCK_SIZE = 12;

    /**
     * @brief �A�[�J�C�u���f�[�^�ւ̃A�N�Z�X�����t�@�C������擾����( �ᑬ, ��ƃq�[�v�s�v�� )
     * @param[in]  data_id      �t�@�C�����f�[�^ID
     * @param[out] access_info  �擾�����A�N�Z�X���̊i�[��
     *
     * @note ���݂͌Ă΂�Ȃ���, �A���P�[�g�����ɃA�N�Z�X�����擾������@�Ƃ��Ďc���Ă���.
     */
    void ArcFile::GetAccessInfoFromFile_Slow( ARCDATID data_id, AccessInfo* access_info )
    {
      const u16 FILE_ID_NUM = this->GetFileCountFromFile();
      GFL_ASSERT( data_id < FILE_ID_NUM );

      const size_t OFFSET_TO_ALLOCATION_OFFSET = m_accessor->GetOffsetToAllocationOffset();
      const size_t ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK = m_accessor->GetElementSizeOfAllocationOffsetBlock();
      const size_t OFFSET_TO_ALLOCATION_TABLE_BLOCK = OFFSET_TO_ALLOCATION_OFFSET + ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK * FILE_ID_NUM;
      const size_t OFFSET_TO_FILE_NUM = OFFSET_TO_ALLOCATION_TABLE_BLOCK + m_accessor->GetOffsetToFileNumFromAllocationTableBlock();

      size_t file_num = 0;
      this->SeekFile( OFFSET_TO_FILE_NUM );
      this->LoadDataPtr( sizeof(u32), &file_num ); 

      size_t offset_from_allocation_table = 0;
      this->SeekFile( OFFSET_TO_ALLOCATION_OFFSET + ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK * data_id );
      this->LoadDataPtr( ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK, &offset_from_allocation_table );

      u32 language_bit = 0;
      const size_t OFFSET_TO_ALLOCATION_TABLE = OFFSET_TO_ALLOCATION_TABLE_BLOCK + m_accessor->GetHeaderSizeOfAllocationTableBlock();
      this->SeekFile( OFFSET_TO_ALLOCATION_TABLE + offset_from_allocation_table );
      this->LoadDataPtr( sizeof(u32), &language_bit );

      const size_t HEADER_SIZE_OF_ALLOCATION_TABLE_ELEMENT = m_accessor->GetHeaderSizeOfAllocationTableElement();
      const size_t ELEMENT_SIZE_OF_ALLOCATION_TABLE = m_accessor->GetElementSizeOfAllocationTable();
      size_t offset_to_target_element = HEADER_SIZE_OF_ALLOCATION_TABLE_ELEMENT;
      if( language_bit & (1<<LANGUAGE_CODE) ) {  // �w�肳�ꂽ�����p�̃t�@�C��������
        for( u32 i=0; i<LANGUAGE_CODE; i++ ) {
          if( language_bit & (1<<i) ) {
            offset_to_target_element += ELEMENT_SIZE_OF_ALLOCATION_TABLE;
          }
        }
      } 

      u32 file_top = 0;
      u32 file_bottom = 0;
      u32 buffer[2];
      this->SeekFile( OFFSET_TO_ALLOCATION_TABLE + offset_from_allocation_table + offset_to_target_element );
      this->LoadDataPtr( sizeof(u32)*2, buffer );
      file_top = buffer[0];
      file_bottom = buffer[1];

      const size_t OFFSET_TO_FILE_IMAGE_FROM_ARCHIVE_TOP = 
        OFFSET_TO_ALLOCATION_TABLE + 
        HEADER_SIZE_OF_ALLOCATION_TABLE_ELEMENT * FILE_ID_NUM + 
        ELEMENT_SIZE_OF_ALLOCATION_TABLE * file_num + 
        m_accessor->GetSizeOfImageBlock();

      access_info->offsetFromArchiveTop = OFFSET_TO_FILE_IMAGE_FROM_ARCHIVE_TOP + file_top;
      access_info->dataSize = file_bottom - file_top;
    } 
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief �f�[�^1������( �p�f�B���O���܂܂Ȃ� )�T�C�Y���擾����
     * @param data_id  �A�[�J�C�u�t�@�C�����f�[�^ID
     *
     * @note �ʏ탂�[�h�̏ꍇ
     * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
     *
     * @note �f�[�^��ǂݍ��ނ��߂ɕK�v�ȃo�b�t�@�T�C�Y��, �p�f�B���O���܂񂾃T�C�Y�ł��B
     * �f�[�^��ǂݍ��ނ��߂̃f�[�^�T�C�Y�� GetDataSize() �Ŏ擾���Ă��������B
     *
     * @attention
     * ���̃��\�b�h��, ver.4.0 �ȍ~�̃A�[�J�C�u�t�@�C���ł̂ݗL���ƂȂ�܂��B
     * ver.3.x �܂ł̃A�[�J�C�u�t�@�C���ł̓p�f�B���O���܂ރf�[�^�T�C�Y���Ԃ�܂��B
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::GetRealDataSize( ARCDATID data_id )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      return access_info.realSize;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�u�Ɋ܂܂��f�[�^�̐����擾����
     *
     * @note �ʏ탂�[�h�̏ꍇ, 
     * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
     */
    //-------------------------------------------------------------------------
    u16 ArcFile::GetDataCount( void )
    {
      if( this->IsFastMode() )
      {
        return m_dataNum;
      } 
      else 
      {
        return this->GetFileCountFromFile();
      }
    }

    /**
     * @brief �i�[�t�@�C�������A�[�J�C�u�t�@�C������ǂݍ���
     */
    u16 ArcFile::GetFileCountFromFile( void )
    {
      IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
      const size_t OFFSET_TO_FILE_ID_NUM = accessor->GetOffsetToFileIDNum();
      GFL_DELETE accessor;

      u16 file_count = 0;
      this->ExclusiveRead( OFFSET_TO_FILE_ID_NUM, sizeof(u16), &file_count );
      return file_count;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�[�J�C�u�Ɋ܂܂��f�[�^�̂���, �ł��傫�ȃf�[�^�̃T�C�Y���擾����
     *
     * @note 
     * �ʏ탂�[�h�̏ꍇ, 
     * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
     *
     * @attention
     * ���̃��\�b�h��, ver.3.0 �ȍ~�̃A�[�J�C�u�t�@�C���ł̂ݗL���ƂȂ�܂��B
     * ver.2.x �܂ł̃A�[�J�C�u�t�@�C���ł͕K��0���Ԃ�܂��B
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::GetMaxDataSize( void )
    {
      if( this->IsFastMode() )
      {
        return m_maxDataSize;
      }
      else 
      {
        size_t total_block_size = this->ReadTotalBlockSize();
        void* temp_buffer = GFL_NEW_ARRAY( m_workHeap ) u8[ total_block_size ];
        this->ExclusiveRead( 0, total_block_size, temp_buffer );

        IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
        accessor->AttachData( temp_buffer );
        size_t max_size = accessor->GetMaxDataSize();

        GFL_DELETE accessor;
        GFL_DELETE_ARRAY temp_buffer;
        return max_size;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �񓯊��ǂݍ��݂��J�n����
     *
     * @param[in]  heap             �X���b�h�̐����Ɏg�p����q�[�v
     * @param[in]  data_id          �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[out] buffer           �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
     * @param[in]  thread_priority  �X���b�h�̗D��x( �� 0 �` 31 �� )
     * @param[in]  callback_func    �ǂݍ��ݏI�����̃R�[���o�b�N�֐�( NULL�Ȃ�Ă΂�Ȃ� )
     * @param[in]  callback_work    �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( �s�v�Ȃ�NULL )
     *
     * @return �񓯊��ǂݍ��݂��J�n�ł������ǂ���
     * @retval true   �J�n�ł���
     * @retval false  �J�n�ł��Ȃ�����
     *
     * @attention 
     * ArcFile�C���X�^���X1�ɂ�, ������2�ȏ�̔񓯊��ǂݍ��݂����s���邱�Ƃ͏o���܂���.
     * ���łɎ��s���̔񓯊��ǂݍ��݂�����ꍇ, ���̊֐��͎��s���܂�( false ���Ԃ�܂� ).
     *
     * @note
     * WaitAsyncRead()�œǂݍ��݂̊�����҂��Ă�������.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::StartAsyncRead( 
        gfl::heap::HeapBase* heap,
        ARCDATID data_id, 
        void* buffer, 
        gfl::base::Thread::Priority thread_priority,
        File::CallBackFuncOnAsyncAccessFinished callback_func,
        void* callback_work )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info ); 

      return m_file.StartAsyncRead( heap, m_filename,
          access_info.offsetFromArchiveTop, 
          access_info.dataSize, access_info.dataSize, buffer,  // �ꊇ�œǂ�
          thread_priority, callback_func, callback_work );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �񓯊��ǂݍ��݂��J�n����( �I�t�Z�b�g�w��ver. )
     *
     * @param[in]  heap             �X���b�h�̐����Ɏg�p����q�[�v
     * @param[in]  data_id          �A�[�J�C�u�t�@�C�����f�[�^ID
     * @param[in]  read_offset      �w�肵���f�[�^�̐擪����̃I�t�Z�b�g
     * @param[in]  read_size        �ǂݍ��ރf�[�^�T�C�Y
     * @param[out] buffer           �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
     * @param[in]  thread_priority  �X���b�h�̗D��x( �� 0 �` 31 �� )
     * @param[in]  callback_func    �ǂݍ��ݏI�����̃R�[���o�b�N�֐�( NULL�Ȃ�Ă΂�Ȃ� )
     * @param[in]  callback_work    �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( �s�v�Ȃ�NULL )
     *
     * @return �񓯊��ǂݍ��݂��J�n�ł������ǂ���
     * @retval true   �J�n�ł���
     * @retval false  �J�n�ł��Ȃ�����
     *
     * @attention 
     * ArcFile�C���X�^���X1�ɂ�, ������2�ȏ�̔񓯊��ǂݍ��݂����s���邱�Ƃ͏o���܂���.
     * ���łɎ��s���̔񓯊��ǂݍ��݂�����ꍇ, ���̊֐��͎��s���܂�( false ���Ԃ�܂� ).
     *
     * @note
     * WaitAsyncRead()�œǂݍ��݂̊�����҂��Ă�������.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::StartAsyncRead( 
        gfl::heap::HeapBase* heap, 
        ARCDATID data_id,
        u32 read_offset,
        u32 read_size,
        void* buffer, 
        gfl::base::Thread::Priority thread_priority,
        File::CallBackFuncOnAsyncAccessFinished callback_func,
        void* callback_work )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info ); 

      return m_file.StartAsyncRead( heap, m_filename, 
          access_info.offsetFromArchiveTop + read_offset, 
          read_size, read_size, buffer,  // �ꊇ�œǂ�
          thread_priority, callback_func, callback_work );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �񓯊��ǂݍ��݂̊�����҂�
     * @retval true   ������
     * @retval false  ����
     *
     * @attention
     * false ���Ԃ�܂ŌJ��Ԃ��Ă�, �ڍs�̊�����҂��Ă�������.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::WaitAsyncRead( void )
    {
      if( this->CheckAsyncRead() == false ) 
      {
        this->EndAsyncRead();
        return false;
      }
      return true;
    } 

    /**
     * @brief �񓯊��ǂݍ��݂����s�����ǂ����𒲂ׂ�
     * @retval true   ���s��
     * @retval false  ���s���Ă��Ȃ�
     */
    bool ArcFile::CheckAsyncRead( void ) const
    {
      return m_file.CheckAsyncAccess();
    } 

    /**
     * @brief �񓯊��ǂݍ��݂���������
     */
    void ArcFile::EndAsyncRead( void )
    {
      m_file.EndAsyncAccess();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���s���̔񓯊��ǂݍ��݂��L�����Z������
     *
     * @note
     * WaitAsyncRead()�œǂݍ��݂̊�����҂��Ă�������.
     */
    //-------------------------------------------------------------------------
    void ArcFile::CancelAsyncRead( void )
    {
      m_file.CancelAsyncAccess();
    } 



    const u32 ArcFile::STACK_SIZE_OF_SETUP_THREAD_FOR_FAST_MODE = 4 * 1024;

    //-------------------------------------------------------------------------
    /**
     * @brief �������[�h�ւ̈ڍs���J�n����
     * @param heap_for_thread_work       �X���b�h�̍�Ɨ̈�Ƃ��Ďg�p����q�[�v
     * @param heap_for_fastaccess_table  �����A�N�Z�X�p�f�[�^�̊m�ۂɎg�p����q�[�v
     *
     * @note WaitSetupForFastMode()�ňڍs�̊�����҂��Ă�������.
     */
    //-------------------------------------------------------------------------
    void ArcFile::StartSetupForFastMode( 
        gfl::heap::HeapBase* heap_for_thread_work,
        gfl::heap::HeapBase* heap_for_fastaccess_table )
    {
      if( !this->IsFastMode() && !this->IsSettingUpForFastMode() ) 
      {
        // �����̃X���b�h��j��
        GFL_SAFE_DELETE( m_setupThread );

        // �X���b�h���N������
        // �X���b�h�͏���������Ɏ��E����̂ŁA
        // �X���b�h���g�ƃX�^�b�N�̈�̓q�[�v�̌������m�ۂ���B
        gfl::heap::HeapBase* thread_heap = heap_for_thread_work->GetLowerHandle();
        m_setupThread = GFL_NEW_LOW( thread_heap ) 
          SetupThread( this, heap_for_fastaccess_table, thread_heap, STACK_SIZE_OF_SETUP_THREAD_FOR_FAST_MODE );
        m_setupThread->Start( gfl::base::Thread::DefaultPriority );
      }
    }

    /**
     * @brief �������[�h�ւ̈ڍs�����ǂ����𒲂ׂ�
     * @retval true   �ڍs��
     * @retval false  �ڍs���łȂ�
     */
    bool ArcFile::IsSettingUpForFastMode( void ) const
    {
      return ( m_setupThread && m_setupThread->IsAlive() );
    }

    //-------------------------------------------------------------------------
    /**
     * @breif �������[�h�ւ̈ڍs������҂�
     * @retval true   ��������
     * @retval false  ������
     *
     * @attention
     * true ���Ԃ�܂ŌJ��Ԃ��Ă�, �ڍs�̊�����҂��Ă�������.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::WaitSetupForFastMode( void )
    {
      if( this->IsSetupForFastModeFinished() ) 
      {
        this->EndSetupForFastMode();
        return true;
      }
      return false;
    } 

    /**
     * @brief �������[�h�ւ̈ڍs�������I���������ǂ����𒲂ׂ�
     * @retval true   ��������
     * @retval false  ������
     */
    bool ArcFile::IsSetupForFastModeFinished( void ) const
    {
      // ���łɍ������[�h
      if( this->IsFastMode() ) {
        return true;  
      }
      //����StartSetupForFastMode()���Ă΂ꂸ��Wait���Ăяo���ꂽ���̃t�H���[
      if( m_setupThread == NULL ){
        GFL_ASSERT(0); //@check
        return true;
      }
      // �ڍs����
      if( m_setupThread && !m_setupThread->IsAlive() ) {
        return true;  
      }

      return false;
    }

    /**
     * @brief �������[�h�ւ̈ڍs����������
     */
    void ArcFile::EndSetupForFastMode( void )
    {
      GFL_SAFE_DELETE( m_setupThread );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �������[�h���ǂ����𒲂ׂ�
     * @retval true   �������[�h
     * @retval false  �ʏ탂�[�h
     */
    //-------------------------------------------------------------------------
    bool ArcFile::IsFastMode( void ) const
    {
      return ( m_fastSetupEvent.IsSignaled() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �������[�h�ւ̈ڍs�������������_�ŃV�O�i����ԂɂȂ�C�x���g�I�u�W�F�N�g���擾����
     * @note ��x�V�O�i����ԂɂȂ�����A�ȍ~���Z�b�g�͂���܂���B
     * @attention �擾�����C�x���g�I�u�W�F�N�g�̏�Ԃ�ύX���Ȃ��ł��������B
     */
    //-------------------------------------------------------------------------
    nn::os::LightEvent* ArcFile::GetFastSetupEvent( void )
    {
      return &m_fastSetupEvent;
    }

    //=========================================================================
    /**
     * @brief �������[�h�ֈڍs����( �������� )
     * @param heap  �����A�N�Z�X���邽�߂̃f�[�^�m�ۂɎg�p����q�[�v
     */
    //=========================================================================
    void ArcFile::SetupForFastMode( gfl::heap::HeapBase* heap )
    {
      // �����C���X�^���X�ɂ��āA
      // �����̃X���b�h�������� SetupForFastMode() ���Ă΂��\��������B
      // �������ASetupForFastMode() �͔r���I�Ɏ��s����K�v�����邽�߁A
      // 1��ڂ̎��s���ɃN���e�B�J���Z�N�V���������b�N����B
      // �Ȃ��A1�񂾂����s����Ηǂ��͂��Ȃ̂ŁA�ȍ~�A�N���e�B�J���Z�N�V�����̓A�����b�N���Ȃ��B
      if( m_criticalSectionForFastSetup.TryEnter() )
      { 
        gfl::heap::HeapBase* lower_heap = m_workHeap->GetLowerHandle();

        size_t total_block_size = this->ReadTotalBlockSize();
        void* block_data = GFL_NEW_ARRAY( lower_heap ) u8[ total_block_size ];
        this->ExclusiveRead( 0, total_block_size, block_data );

        IArcFileAccessor* accessor = this->CreateArcFileAccessor( lower_heap );
        accessor->AttachData( block_data );

        GFL_SAFE_DELETE_ARRAY( m_fastAccessTable );
        m_fastAccessTable = this->CreateDataAccessTable( heap, accessor );
        m_dataNum = accessor->GetFileIdNum();
        m_maxDataSize = accessor->GetMaxDataSize();

        GFL_DELETE accessor;
        GFL_DELETE_ARRAY block_data; 

        m_fastSetupEvent.Signal();
      }
    }

    /**
     * @brief �A�[�J�C�u�t�@�C���ւ̃A�N�Z�b�T�𐶐�����
     * @param heap �A�N�Z�b�T�̊m�ۂɎg�p����q�[�v
     * @return ���������A�N�Z�b�T
     */
    IArcFileAccessor* ArcFile::CreateArcFileAccessor( gfl::heap::HeapBase* heap )
    {
      IArcFileAccessor* accessor = NULL;

#if GFL_DEBUG
      switch( m_majorVersion ) {
      case 2: accessor = GFL_NEW( heap ) ver2::ArcFileAccessor(); break; 
      case 3: accessor = GFL_NEW( heap ) ver3::ArcFileAccessor(); break; 
      case 4: accessor = GFL_NEW( heap ) ver4::ArcFileAccessor(); break; 
      } 
      if( accessor == NULL ) {
        GFL_PRINT( "m_majorVersion=%d\n", m_majorVersion );
        GFL_ASSERT_STOP(0); //@check
      }
#else
      accessor = GFL_NEW( heap ) ver4::ArcFileAccessor();
#endif

      return accessor;
    }

    /**
     * @brief �f�[�^�u���b�N�̍��v�T�C�Y��ǂݍ���
     * @return ���g�̃f�[�^�u���b�N�̍��v�T�C�Y
     */
    size_t ArcFile::ReadTotalBlockSize( void )
    {
      IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
      const size_t OFFSET_TO_TOTAL_BLOCK_SIZE = accessor->GetOffsetToTotalBlockSize();
      GFL_DELETE accessor;

      size_t total_block_size = 0;
      this->ExclusiveRead( OFFSET_TO_TOTAL_BLOCK_SIZE, sizeof(u32), &total_block_size );
      return total_block_size;
    } 

    /**
     * @brief �A�[�J�C�u���f�[�^�ւ̃A�N�Z�X�e�[�u���𐶐�����
     * @param heap      �e�[�u���̐����Ɏg�p����q�[�v
     * @param accessor  �A�[�J�C�u�t�@�C���̃A�N�Z�b�T
     * @return ���������A�N�Z�X�e�[�u��
     */
    ArcFile::AccessInfo* ArcFile::CreateDataAccessTable( gfl::heap::HeapBase* heap, const IArcFileAccessor* accessor ) const
    {
      u32 file_id_num = accessor->GetFileIdNum();
      AccessInfo* access_table = GFL_NEW_ARRAY( heap ) ArcFile::AccessInfo[ file_id_num ];

      for( u32 file_id=0; file_id<file_id_num; file_id++ ) {
        access_table[ file_id ].dataSize = accessor->GetDataSize( file_id, LANGUAGE_CODE );
        access_table[ file_id ].realSize = accessor->GetRealDataSize( file_id, LANGUAGE_CODE );
        access_table[ file_id ].offsetFromArchiveTop = accessor->GetOffsetToData( file_id, LANGUAGE_CODE );
      } 
      return access_table;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief HIO �ɂ��ǂݍ��݂��H
     * @retval true   HIO �ǂݍ���
     * @retval false  HIO �ǂݍ��݂łȂ�
     */
    //-------------------------------------------------------------------------
    bool ArcFile::IsHioFile( void ) const
    {
#if GFL_HIO_ENABLE
      return m_isHioFile;
#else
      return false;
#endif
    }


  }  //namespace fs
}  //namespace gfl
