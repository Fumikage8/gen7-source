//========================================================================================
/**
 * @file  gfl_FileApi.h
 * @brief GAMEFREAK Library FileIO System ( System API )
 * @author  GAMEFREAK inc.
 * @date  2010.09.24
 */
//========================================================================================
#ifndef __GFL_FILEAPI_H__
#define __GFL_FILEAPI_H__ 
#pragma once

#include <gfl_Base.h>
#include <gfl_Heap.h>


namespace gfl {
  namespace fs {

    // �t�@�C���A�N�Z�X�D�揇��
    enum{
      PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// ���p���ӁI�I�I�@�h�L�������g��ǂ��ǂނ���
      PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// ����
      PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// �Ⴂ
    };

    class FileSystem
    {
      public:

        // �p�X������̍ő咷( �I�[�������܂� )
        static const u32 MAX_PATH_LENGTH = 256;


        //------------------------------------------------------------------------------------
        /**
         * @brief fs���C�u����������������
         *
         * @param heap            ���C�u�����̍�Ɨ̈�Ƃ��Ďg�p����q�[�v
         * @param device_heap     ���C�u�����̍�Ɨ̈�Ƃ��Ďg�p����f�o�C�X�q�[�v
         * @param rom_mount_flag  ROM�A�[�J�C�u���}�E���g���邩�ǂ���
         *
         * @note fs���C�u�����ɂ���N���X��֐����g�����߂ɂ�, ��ɂ��̊֐����ĂԕK�v������܂�.
         */
        //------------------------------------------------------------------------------------
        static void Initialize( heap::HeapBase* heap, heap::HeapBase* device_heap, bool rom_mount_flag= true );

        //------------------------------------------------------------------------------------
        /**
         * @brief fs���C�u�������I������
         */
        //------------------------------------------------------------------------------------
        static void Finalize();

        //------------------------------------------------------------------------------------
        /**
         * @brief fs���C�u����������������Ă��邩�ǂ����𒲂ׂ�
         *
         * @retval true   �������ς�
         * @retval false  ����������Ă��Ȃ�
         */
        //------------------------------------------------------------------------------------
        static bool IsInitialized();


        //------------------------------------------------------------------------------------
        /**
         * @brief "�p�X + �t�@�C����" �̕����񂩂�, "�p�X" �� "�t�@�C����" �����ꂼ�ꒊ�o����.
         * @param[in]  src_string      ���o�Ώۂ̕�����( ������MAX_PATH_LENGTH�ȉ��ł��邱�� )
         * @param[out] dest_path       ���o�����p�X�̊i�[��o�b�t�@
         * @param[out] dest_file_name  ���o�����t�@�C�����̊i�[��o�b�t�@
         *
         * @retval true   ����
         * @retval false  ���s
         *
         * @note
         * ���Ƃ���, "rom:/test_data.bin" �̏ꍇ,
         * �p�X�� = "rom:/"
         * �t�@�C���� = "test_data.bin"
         */
        //------------------------------------------------------------------------------------
        static bool GetPathAndFileName( 
            const wchar_t* src_string,
            wchar_t* dest_path,
            wchar_t* dest_file_name ); 

        //------------------------------------------------------------------------------------
        /**
         * @brief �����A�[�J�C�u�ȉ��Ƀt�@�C�������݂��邩�ǂ����𒲂ׂ�
         * @param filename �`�F�b�N�Ώۂ̃t�@�C����( rom:/xxxx )
         * @retval true  �t�@�C�������݂���
         * @retval false �t�@�C�������݂��Ȃ�
         */
        //------------------------------------------------------------------------------------
        static bool CheckRomFileExist( const wchar_t* filename ); 


        //----------------------------------------------------------------------------
        /**
         *	@brief  �t�@�C���V�X�e���S�̂̓ǂݍ��ݗD�揇�ʂ�ݒ�
         *	
         *	@param  base_priority           �t�@�C���ǂݍ��ݗD�揇��
         *	@param  main_thread_priority    ���C���X���b�h�ł́A�t�@�C���ǂݍ��ݗD�揇��
         *
         *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// ���p���ӁI�I�I�@�h�L�������g��ǂ��ǂނ���
         *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// ����
         *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// �Ⴂ
         *
         *   main_thread_priority�́A���̊֐��R�[�����Open���ꂽRomFile�ɓK������܂��B
         */
        //-----------------------------------------------------------------------------
        static void SetPriority( s32 base_priority, s32 main_thread_priority );

        //----------------------------------------------------------------------------
        /**
         *	@brief  ���C���X���b�h�ł̃t�@�C���ǂݍ��ݗD�揇�ʂ��擾
         */
        //-----------------------------------------------------------------------------
        static s32 GetMainThreadPriority( void );

      private:

        /**
         * @brief ROM�A�[�J�C�u���}�E���g����
         */
        static void MountRomArchive( gfl::heap::HeapBase* heap );


        /**
         * @brief ���C���X���b�h�D�揇��
         */
        static s32 m_MainThreadPriority;
        
#if GFL_FILE_DEBUG
      public:

        /// �f�o�b�O�v�����g���[�h
        enum DebugPrintMode{
          DEBUG_PRINT_NONE,           ///< �f�o�b�O�v�����g�Ȃ�
          DEBUG_PRINT_MAIN_THREAD,    ///< ���C���X���b�h�ǂݍ��݂�\��
          DEBUG_PRINT_OTHER_THREAD,   ///< ���C���X���b�h�ȊO�̓ǂݍ��݂�\��
          DEBUG_PRINT_ALL,            ///< �S�ǂݍ��݂�\��

          DEBUG_PRINT_MAX,            ///< �V�X�e���Ŏg�p
        };

        /// �v���t�@�C�����O�p�����[�^
        struct ProfilerParam{
          s32   main_thread_us;         ///< ���C���X���b�h�œǂݍ��݂������鎞�ԁi�}�C�N���b�j
          DebugPrintMode debug_print;   ///< �f�o�b�O�v�����g���[�h
          bool  main_thread_us_over_assert; ///< main_thread_us���I�[�o�[������~�߂邩�H
        };
        /// �v���t�@�C���[�̐ݒ�
        static void SetProfiler( bool flag, const ProfilerParam* cp_param );

        /// �v���t�@�C���[�����쒆���`�F�b�N
        static bool IsProfiling( void );
        
        /// ���C�����[�v�łP����s���Ă��������B
        static void UpdateProfiler( void );

        /// �P�ǂݍ��݃f�[�^���Ď�
        static void AddProfilerData( const wchar_t& path, bit32 thread_id, s32 size, s32 offset, s32 time );

        /// �Ō�̃v���t�@�C�����ʂ��G���[���`�F�b�N
        static bool IsLastProfilerError( void );
        static s32 GetLastProfilerTotalUs( void );
        

        /**
         * @brief SD�J�[�h�A�[�J�C�u���}�E���g����
         */
        static void MountSdmcArchive( void );

      private:
        
        /// 1�t���[�����̓ǂݍ��ݏ��̃o�b�t�@
        static const u32 PR_FILE_BUFFER_COUNT   = 64;
        static const u32 PR_FILE_BUFFER_STRNUM  = 32;
        static bool m_ProfilerFlag;
        static ProfilerParam m_ProfilerParam;
        static wchar_t m_ProfilerFileNameBuffer[PR_FILE_BUFFER_COUNT][PR_FILE_BUFFER_STRNUM];
        static s32 m_ProfilerFileSizeBuffer[PR_FILE_BUFFER_COUNT];
        static s32 m_ProfilerFileOffsetBuffer[PR_FILE_BUFFER_COUNT];
        static s32 m_ProfilerFileUsBuffer[PR_FILE_BUFFER_COUNT];
        static s32 m_ProfilerTotalUs;
        static s32 m_ProfilerTotalSize;
        static s32 m_ProfilerFileCount;
        static nn::os::CriticalSection m_criticalSectionForProfilerAccess;
        static bool m_ProfilerLastError;
        static s32 m_ProfilerLastTotalUs;
#endif // GFL_FILE_DEBUG


        // HIO

#if GFL_HIO_ENABLE
      public:
        //------------------------------------------------------------------------------------
        /**
         * @brief HIO���C�u����������������
         * @param device_heap  HIO���C�u�����̍�ƃ��[�N���m�ۂ���f�o�C�X�q�[�v
         */
        //------------------------------------------------------------------------------------
        static void InitializeHio( gfl::heap::HeapBase* device_heap );

        //------------------------------------------------------------------------------------
        /**
         * @brief HIO���C�u�������I������
         */
        //------------------------------------------------------------------------------------
        static void FinalizeHio( void );

        //------------------------------------------------------------------------------------
        /**
         * @brief HIO�A�[�J�C�u���}�E���g����
         * @param device_heap     HIO���C�u�����̍�ƃ��[�N���m�ۂ���f�o�C�X�q�[�v
         * @param archive_name    HIO�A�[�J�C�u��
         * @param root_directory  HIO�A�[�J�C�u�̃��[�g�f�B���N�g��
         */
        //------------------------------------------------------------------------------------
        static void MountHioArchive( 
            gfl::heap::HeapBase* device_heap, char* archive_name, char* root_directory );

        //------------------------------------------------------------------------------------
        /**
         * @brief �z�X�g�t�@�C���V�X�e���Ƀt�@�C�������݂��邩�ǂ����𒲂ׂ�
         * @param filename �`�F�b�N�Ώۂ̃t�@�C����( �t���p�X�Ŏw�� )( Mount����Ă���A�[�J�C�u���Ȃ�Γ����Ńt���p�X�ɒu������ )
         * @retval true  �t�@�C�������݂���
         * @retval false �t�@�C�������݂��Ȃ�
         */
        //------------------------------------------------------------------------------------
        static bool CheckHioFileExist( const wchar_t* filename );


      private:
        static bool m_hio_initialize_flag;                       // HIO���C�u�������������������ǂ���
        static void* m_hio_library_work;                         // HIO���C�u�����̍�ƃ��[�N

        static const u8 MAX_HIO_MOUNT_COUNT = 4;                 // HIO�A�[�J�C�u���}�E���g�ł��鐔
        static u8 m_hio_mount_count;                             // HIO�A�[�J�C�u���}�E���g������
        static void* m_hio_mount_buffer[ MAX_HIO_MOUNT_COUNT ];  // HIO�A�[�J�C�u�̃}�E���g�o�b�t�@

        static char     m_hio_mount_archive_name[ MAX_HIO_MOUNT_COUNT ][ 256 ];  // HIO�A�[�J�C�u��
        static wchar_t  m_hio_mount_root_dir[ MAX_HIO_MOUNT_COUNT ][ 256 ];   // HIO�A�[�J�C�u�̃��[�g�p�X
#endif
    };



  } // namespace fs
} // namespace gfl 


#endif // __GFL_FILEAPI_H__
