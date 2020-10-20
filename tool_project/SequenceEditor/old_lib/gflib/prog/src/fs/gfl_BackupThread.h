//=============================================================================
/**
 * @brief  �o�b�N�A�b�v�t�@�C���̓ǂݍ��݂��s���X���b�h
 * @file   gfl_BackupThread.h
 * @author obata_toshihiro
 * @date   2011.02.17
 */
//=============================================================================
#ifndef __GFL_BACKUPTHREAD_H__
#define __GFL_BACKUPTHREAD_H__
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>


namespace gfl {
  namespace fs { 

    class BackupSystem;


    class BackupThread : public gfl::base::Thread
    {
      public:

        /**
         * @brief ���샂�[�h
         */
        enum Mode {
          MODE_SAVE,
          MODE_LOAD,
          MODE_FORMAT,
          MODE_CREATE,
        };

        //---------------------------------------------------------------------
        // �R���X�g���N�^�E�f�X�g���N�^
        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         * @param heap           �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
         * @param stack_size     �X���b�h�̃X�^�b�N�̈�̃T�C�Y
         * @param backup_system  �o�b�N�A�b�v�V�X�e��
         * @param mode           ���샂�[�h
         *
         * @note �C���X�^���X������, �����I�ɃX���b�h���N������.
         */
        BackupThread( 
            gfl::heap::HeapBase* heap,
            u32 stack_size,
            BackupSystem* backup_system,
            Mode mode );

        /**
         * @brief �f�X�g���N�^
         */
        virtual ~BackupThread(); 

        //---------------------------------------------------------------------
        // �t�H�[�}�b�g
        //---------------------------------------------------------------------
        struct FormatParam
        {
          size_t maxDirectoryCount;                // �f�B���N�g���̍ő吔
          size_t maxFileCount;                     // �t�@�C���̍ő吔
          BackupSystem::DuplicateFlag isDuplicate; // ��d�����邩�ǂ���
        };

        /**
         * @brief MODE_FORMAT �̓���p�����[�^���Z�b�g����
         * @param param �t�H�[�}�b�g�ݒ�
         */
        void SetFormatParam( const FormatParam& param );

        //---------------------------------------------------------------------
        //  �t�@�C������
        //---------------------------------------------------------------------
        struct CreateParam
        {
          const wchar_t * filePath;
          size_t          fileSize;
        };

        /**
         * @brief
         * @param cparam
         */
        void SetCreateParam( const CreateParam& cparam );

        //---------------------------------------------------------------------
        // ����
        //--------------------------------------------------------------------- 
        /**
         * @brief ���s���ʂ��擾����
         */
        BackupSystem::Result GetResult( void ) const;

        BackupThread::Mode GetMode( void ) const { return m_mode; }


      private: 

        static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1; // �p�X������̍ő咷( �I�[�������܂� )
        wchar_t m_path[ MAX_FILE_PATH_LENGTH ];

        Mode m_mode;
        FormatParam m_format_param;
        CreateParam m_create_param;
        BackupSystem* m_backup_system;
        BackupSystem::Result m_result;

        virtual void Main( void );
    }; 


  }  // namespace fs
}  // namespace gfl


#endif // __GFL_BACKUPTHREAD_H__ 
