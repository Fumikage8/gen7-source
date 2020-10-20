//=============================================================================
/**
 * @file   gfl_BackupSystem.h
 * @brief  �o�b�N�A�b�v�V�X�e��
 * @author obata_toshihiro
 * @date   2011.02.16
 *
 * @note �Z�[�u�f�[�^�A�[�J�C�u�� "data:" �Ń}�E���g���܂��B
 */
//=============================================================================
#ifndef __GFL_BACKUPSYSTEM_H__
#define __GFL_BACKUPSYSTEM_H__
#pragma once

#include <gfl_Base.h>
#include <gfl_Heap.h>
#include <base/gfl_Singleton.h>
#include <base/gfl_Queue.h>


namespace gfl {
  namespace fs {

    class BackupThread;


    class BackupSystem : public gfl::base::Singleton<BackupSystem>
    {
      public:

        /**
         * @brief �Z�[�u�f�[�^���d�����邩�ǂ����̎w��
         */
        enum DuplicateFlag {
          DUPLICATE_ON = true,    // ��d������
          DUPLICATE_OFF = false,  // ��d�����Ȃ�
        };

        /**
         * @brief ���s����
         */
        enum Result {
          RESULT_OK,                     // ����
          RESULT_CONTINUE,               // ������
          RESULT_NG_MEDIA_NOT_EXIST,     // ���s�F���f�B�A�����݂��Ȃ�
          RESULT_NG_NOT_FORMATTED,       // ���s�F�Z�[�u�f�[�^�̈悪�t�H�[�}�b�g����Ă��Ȃ�
          RESULT_NG_BAD_FORMAT,          // ���s�F�t�H�[�}�b�g���s���ȏ�ԂɂȂ��Ă���
          RESULT_NG_BROKEN_DATA,         // ���s�F�f�[�^�����Ă��邩, ��₂���Ă���
          RESULT_NG_ERROR,               // ���s�F�ڐG�s�ǂȂ�, �n�[�h�E�F�A�v��
          RESULT_NG_PATH_NOT_EXIST,      // ���s�F�w�肵���p�X�����݂��Ȃ�
          RESULT_NG_PATH_ALREADY_EXIST,  // ���s�F�w�肵���p�X�����ɑ��݂���
          RESULT_NG_LACK_OF_SPACE,       // ���s�F���f�B�A�̋󂫗e�ʕs��
          RESULT_NG_UNKNOWN,             // ���s�F�z��O�̃G���[
          RESULT_NG_UNMOUNT,             // ���s�F�Z�[�u�f�[�^�A�[�J�C�u���}�E���g����Ă��Ȃ�
          RESULT_NG_ALREADY,             // ���s�F( �Z�[�u or ���[�h�� )���Ɏ��s��
          RESULT_NG_NOT_START,           // ���s�F( �Z�[�u or ���[�h�� )���s���Ă��Ȃ�
        };

        //---------------------------------------------------------------------
        // �R���X�g���N�^�E�f�X�g���N�^
        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         *
         * @param heap  �V�X�e���̍�Ɨ̈�Ɏg�p����q�[�v
         */
        BackupSystem( gfl::heap::HeapBase* heap );

        /**
         * @brief �f�X�g���N�^
         */
        virtual ~BackupSystem( void );


        //---------------------------------------------------------------------
        // �Z�[�u�f�[�^�̏�����
        //
        // �o�b�N�A�b�v�̈�� "data:" �Ƃ����A�[�J�C�u���Ń}�E���g��,
        // "data:" �ȉ��Ƀt�H���_�E�t�@�C�����쐬���܂�.
        //---------------------------------------------------------------------
        /**
         * @brief �Z�[�u�f�[�^�̈���t�H�[�}�b�g����
         *
         * @param max_dir_count   �f�B���N�g���̍ő吔
         * @param max_file_count  �t�@�C���̍ő吔
         * @param duplicate       ��d�����邩�ǂ���
         * @retval true   �t�H�[�}�b�g�ɐ���
         * @retval false  �t�H�[�}�b�g�Ɏ��s
         *
         * @attention �����̃o�b�N�A�b�v�t�@�C���͑S�č폜����܂�.
         */
        Result Format( size_t max_directories, size_t max_files, DuplicateFlag duplicate );

        /**
         * @brief �Z�[�u�f�[�^�̈�̔񓯊��t�H�[�}�b�g���J�n����
         *
         * @param thread_priority  �t�H�[�}�b�g�X���b�h�̃v���C�I���e�B
         * @param max_dir_count    �f�B���N�g���̍ő吔
         * @param max_file_count   �t�@�C���̍ő吔
         * @param duplicate        ��d�����邩�ǂ���
         *
         * @retval RESULT_OK       �t�H�[�}�b�g�J�n
         * @retval RESULT_NG_XXXX  �t�H�[�}�b�g�J�n�Ɏ��s
         *
         * @attention �����̃o�b�N�A�b�v�t�@�C���͑S�č폜����܂�.
         */
        Result StartAsyncFormat( 
            u8 thread_priority, 
            size_t max_directories, size_t max_files, DuplicateFlag duplicate );

        /**
         * @brief �Z�[�u�f�[�^�̈�̔񓯊��t�H�[�}�b�g�̊�����҂�
         *
         * @retval RESULT_CONTINUE  �t�H�[�}�b�g��
         * @retval RESULT_OK        �t�H�[�}�b�g�I��
         * @retval RESULT_NG_XXXX   �t�H�[�}�b�g�Ɏ��s
         */
        Result WaitAsyncFormat( void );

        /**
         * @brief �o�b�N�A�b�v�t�@�C�����쐬����
         *
         * @param file_path  �쐬����t�@�C���̃p�X
         * @param file_size  �쐬����t�@�C���̃T�C�Y
         * @retval RESULT_OK       �t�@�C���̍쐬�ɐ���
         * @retval RESULT_NG_XXXX  �t�@�C���̍쐬�Ɏ��s
         */
        Result CreateBackupFile( const wchar_t* file_path, size_t file_size );

        /**
         * @brief �o�b�N�A�b�v�t�@�C����񓯊��ō쐬����
         *
         * @param thread_priority  �X���b�h�̃v���C�I���e�B
         * @param file_path  �쐬����t�@�C���̃p�X
         * @param file_size  �쐬����t�@�C���̃T�C�Y
         * @retval RESULT_OK       �t�@�C���̍쐬�J�n
         * @retval RESULT_NG_XXXX  �t�@�C���̍쐬�Ɏ��s
         */
        Result StartAsyncCreateFile( u8 thread_priority, const wchar_t* file_path, size_t file_size );

        /**
         * @brief �o�b�N�A�b�v�t�@�C���̍쐬��҂�
         *
         * @param file_path  �쐬����t�@�C���̃p�X
         * @param file_size  �쐬����t�@�C���̃T�C�Y
         * @retval RESULT_OK       �t�@�C���̍쐬�ɐ���
         * @retval RESULT_NG_XXXX  �t�@�C���̍쐬�Ɏ��s
         */
        Result WaitAsyncCreateFile( void );

        /**
         * @brief �o�b�N�A�b�v�t�@�C�����폜����
         *
         * @param file_path  �폜����t�@�C���̃p�X
         * @retval RESULT_OK       �t�@�C���̍폜�ɐ���
         * @retval RESULT_OK �ȊO  �t�@�C���̍폜�Ɏ��s
         */
        Result DeleteBackupFile( const wchar_t* file_path );

        /**
         * @brief �o�b�N�A�b�v�t�@�C���̗L���𒲂ׂ�
         *
         * @param file_path   ���ׂ�t�@�C���̃p�X
         * @retval true   �t�@�C���͑��݂���
         * @retval false  �t�@�C���͑��݂��Ȃ�
         */
        bool CheckBackupFileExist( const wchar_t* file_path );


        //---------------------------------------------------------------------
        // �o�b�N�A�b�v�f�[�^�̓o�^
        //---------------------------------------------------------------------
        /**
         * @brief ����Ώۂ̃o�b�N�A�b�v�f�[�^��o�^����
         *
         * @param[in]     file_path  �ǂݏ�������t�@�C���ւ̃p�X
         * @param[in,out] data       �������� or �ǂݍ��񂾃f�[�^�̊i�[��
         * @param[in]     datasize   �������� or �ǂݍ��ރf�[�^�̃T�C�Y
         * @retval true   �o�^����
         * @retval false  �o�^���s
         */
        bool AddBackupEntry( const wchar_t* file_path, void* data, u32 datasize );

        /**
         * @brief �o�^�����o�b�N�A�b�v�f�[�^���N���A����
         */
        void ClearBackupEntry( void );


        //---------------------------------------------------------------------
        // ���[�h
        //---------------------------------------------------------------------
        /**
         * @brief �o�b�N�A�b�v�t�@�C����ǂݍ���
         *
         * @retval RESULT_OK       �ǂݍ��ݐ���
         * @retval RESULT_NG_XXXX  �ǂݍ��ݎ��s
         *
         * @attention �ǂݍ��݂���������܂ŏ������Ԃ�܂���.
         */
        Result Load( void );

        /**
         * @brief �o�b�N�A�b�v�t�@�C���̔񓯊��ǂݍ��݂��J�n����
         *
         * @param thread_priority  �o�b�N�A�b�v�X���b�h�̃v���C�I���e�B
         * @retval RESULT_OK       �ǂݍ��݊J�n
         * @retval RESULT_NG_XXXX  �ǂݍ��݊J�n�Ɏ��s
         *
         * @note
         * ���̊֐����� RESULT_OK ���Ԃ����ۂɂ�,
         * WaitAsyncLoad() �� RESULT_OK ���Ԃ�܂ŌJ��Ԃ��Ă�ł�������.
         */
        Result StartAsyncLoad( u8 thread_priority );

        /**
         * @brief �o�b�N�A�b�v�t�@�C���̔񓯊��ǂݍ��݂̊�����҂�
         *
         * @retval RESULT_CONTINUE  �ǂݍ��ݒ�
         * @retval RESULT_OK        �ǂݍ��ݏI��
         * @retval RESULT_NG_XXXX   �ǂݍ��݂Ɏ��s
         */
        Result WaitAsyncLoad( void );



        //---------------------------------------------------------------------
        // �Z�[�u
        //---------------------------------------------------------------------
        /**
         * @brief �o�b�N�A�b�v�t�@�C���ɏ�������
         * @attention �������݂���������܂ŏ������Ԃ�܂���.
         *
         * @retval RESULT_OK       �������ݐ���
         * @retval RESULT_NG_XXXX  �������ݎ��s
         */
        Result Save( void );

        /**
         * @brief �o�b�N�A�b�v�t�@�C���ւ̔񓯊��������݂��J�n����
         *
         * @param thread_priority  �o�b�N�A�b�v�X���b�h�̃v���C�I���e�B
         * @retval RESULT_OK       �������݊J�n
         * @retval RESULT_NG_XXXX  �������݊J�n�Ɏ��s
         */
        Result StartAsyncSave( u8 thread_priority );

        /**
         * @brief �o�b�N�A�b�v�t�@�C���ւ̔񓯊��������݂̊�����҂�
         *
         * @retval RESULT_CONTINUE  �������ݒ�
         * @retval RESULT_OK        �������݊J�n
         * @retval RESULT_NG_XXXX   �������݊J�n�Ɏ��s
         */
        Result WaitAsyncSave( void );


        //---------------------------------------------------------------------
        // ���̑�
        //---------------------------------------------------------------------
        /**
         * @brief �󂫗e�ʂ��擾����
         * @param[out] free_bytes  �󂫗e�ʂ̊i�[��
         * @retval RESULT_OK       �����Bfree_bytes �ɗL���Ȓl���i�[����܂��B
         * @retval RESULT_NG_XXXX  ���s�Bfree_bytes �͗L���Ȓl�ł͂���܂���B
         */
        Result GetFreeBytes( s64* free_bytes ) const;

        /**
         * @brief �Z�[�u�E���[�h�Ώۂ̃t�@�C�������擾����
         */
        u32 GetBackupEntryCount( void ) const;

        /**
         * @brief �Z�[�u�E���[�h�����������t�@�C���̐����擾����
         */
        u32 GetBackupFinishedCount( void ) const;

        Result MountSaveData( void );
        Result UnMountSaveData( void );
        Result CommitSaveData( void );

        nn::Result GetLastNNResult( void ) const { return m_last_result; }


      private:

        static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1; // �p�X������̍ő咷( �I�[�������܂� )
        static const u32 MAX_BACKUP_ENTRY_NUM = 128;   // �o�^�\�ȃo�b�N�A�b�v�Ώۂ̐�
        static const char* SAVEDATA_ARCHIVE_NAME;      // �Z�[�u�f�[�^�A�[�J�C�u��
        static const u32 STACK_SIZE_FOR_FORMAT_THREAD; // �t�H�[�}�b�g�X���b�h�̃X�^�b�N�T�C�Y
        static const u32 STACK_SIZE_FOR_LOAD_THREAD;   // �ǂݍ��݃X���b�h�̃X�^�b�N�T�C�Y
        static const u32 STACK_SIZE_FOR_SAVE_THREAD;   // �������݃X���b�h�̃X�^�b�N�T�C�Y
        static const u32 STACK_SIZE_FOR_CREATE_THREAD; // �t�@�C�������X���b�h�񂨃X�^�b�N�T�C�Y
        static Result GetSystemResult( nn::Result nn_result );
        static Result GetSystemResult( BackupFile::Result file_result );

        // �o�b�N�A�b�v�����̑���Ώ�
        struct BackupEntry {
          wchar_t file_path[ MAX_FILE_PATH_LENGTH ];
          void* data;
          u32 datasize;
        };

        gfl::heap::HeapBase* m_heap;
        BackupEntry m_backup_entries[ MAX_BACKUP_ENTRY_NUM ];
        u32 m_backup_entry_count;
        u32 m_backup_finish_count;
        gfl::fs::BackupFile m_file;
        BackupThread* m_backup_thread;

        nn::Result m_last_result;
        bool m_is_mount;

        bool IsBackupEntryFull( void ) const;
        Result CreateFile( const wchar_t* file_path, size_t file_size );
        Result DeleteFile( const wchar_t* file_path );
        Result WaitAsync( u32 wait_mode );
    };


  }  // namespace fs
}  // namespace gfl


#endif  // __GFL_BACKUPSYSTEM_H__
