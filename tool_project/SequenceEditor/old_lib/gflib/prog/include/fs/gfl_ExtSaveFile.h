//=============================================================================
/**
 * @brief  �o�b�N�A�b�v�t�@�C���A�N�Z�X
 * @file   gfl_ExtSaveFile.h
 * @author obata_toshihiro
 * @author tamada
 * @date   2012.09.13
 *
 *  gfl_BackupFile.cpp����R�s�y�ō쐬
 */
//=============================================================================
#ifndef __GFL_EXTSAVEFILE_H__
#define __GFL_EXTSAVEFILE_H__
#pragma once

#include <gfl_Macros.h>
#include <fs/gfl_FileBase.h>


namespace gfl {
  namespace fs {


    class ExtSaveFile : public File
    {
      friend class ExtSaveSystem;

      public:

        /**
         * @brief �t�@�C���A�N�Z�X�̌���
         */
        enum Result {
          ACCESS_RESULT_OK,                     // ����
          ACCESS_RESULT_NG_PATH_NOT_EXIST,      // ���s�F�w�肵���p�X�����݂��Ȃ�
          ACCESS_RESULT_NG_PATH_ALREADY_EXIST,  // ���s�F�w�肵���p�X�����ɑ��݂���
          ACCESS_RESULT_NG_BROKEN_DATA,         // ���s�F�f�[�^�����Ă��邩, ��₂���Ă���
          ACCESS_RESULT_NG_ERROR,               // ���s�F�ڐG�s�ǂȂ�, �n�[�h�E�F�A�v��
          ACCESS_RESULT_NG_LACK_OF_SPACE,       // ���s�F���f�B�A�̋󂫗e�ʕs��
          ACCESS_RESULT_NG_WRITE_PROTECTED,     // ���s�F���C�g�v���e�N�g����Ă���
          ACCESS_RESULT_NG_MEDIA_NOT_FOUND,     // ���s�FSD�J�[�h���}����Ă��Ȃ�
          ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR,  // ���s�F�ڐG�s�ǂȂ�
          ACCESS_RESULT_NG_ARCHIVE_INVALIDATED, // ���s�F�A�[�J�C�u�����iSD�J�[�h�������j
          ACCESS_RESULT_NG_UNKNOWN,             // ���s�F�z��O�̃G���[
        };

        /**
         * @brief �o�b�N�A�b�v�t�@�C���̃A�N�Z�X���ʂ��擾����
         *
         * @param nn_result nn::fs::TryXXXX�nAPI�̌���
         */
        static Result GetAccessResult( const nn::Result& nn_result );


        //-------------------------------------------------------------------------------
        // �R���X�g���N�^�E�f�X�g���N�^
        //-------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        ExtSaveFile( void );

        /**
         * @brief �f�X�g���N�^
         */
        virtual ~ExtSaveFile( void );


        //-------------------------------------------------------------------------------
        // �t�@�C���A�N�Z�X
        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C�����J��
         *
         * @param[in] path  �t�@�C���p�X
         * @param[in] mode  �I�[�v�����[�h
         *
         * @retval true   �I�[�v���ɐ��������ꍇ
         * @retval false  �I�[�v���Ɏ��s�����ꍇ
         *
         * @note
         * �p�X�̓��ɂ� "exdt:/" ��t���Ă�������.
         * �p�X�̋�؂�ɂ� "/" ���g�p���Ă�������.
         */
        virtual bool Open( const wchar_t* path, OpenMode mode );

        /**
         * @brief �t�@�C�������
         */
        virtual void Close( void );

        /**
         * @brief �t�@�C������ǂݍ���
         *
         * @param[out] buffer  �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
         * @param[in]  size    �ǂݍ��ރT�C�Y
         *
         * @return ���ۂɓǂݍ��񂾃T�C�Y
         */
        virtual size_t Read( void* buffer, u32 size );

        /**
         * @brief �t�@�C���ɏ�������
         *
         * @param[in] buffer  �������ރf�[�^( 4 byte alignment )
         * @param[in] size    �������ރT�C�Y
         *
         * @return ���ۂɏ������񂾃T�C�Y
         */
        virtual size_t Write( const void* buffer, u32 size );

        /**
         * @brief �t�@�C�����V�[�N����
         *
         * @param[in] base_pos  �V�[�N�̋N�_�ƂȂ�ʒu
         * @param[in] offset    �N�_�ʒu����̃I�t�Z�b�g
         *
         * @note ( base_pos + offset )�̈ʒu�Ɉړ����܂��B
         */
        virtual void Seek( SeekBasePos base_pos, s32 offset );

        /**
         * @brief �t�@�C���T�C�Y���擾����
         */
        virtual size_t GetSize( void ) const;

        /**
         * @brief ���݈ʒu���擾����
         *
         * @return �t�@�C���擪����̃I�t�Z�b�g[Byte]
         */
        virtual u32 GetPosition( void ) const;

        /**
         * @brief �t�@�C�����J���Ă��邩�ǂ����𒲂ׂ�
         *
         * @retval true   �t�@�C�����J���Ă���
         * @retval false  �t�@�C�����J���Ă��Ȃ�
         */
        virtual bool IsOpen( void ) const { return m_file_open_flag; }

        /**
         * @brief �ǂݍ��݂��o���邩�ǂ����𒲂ׂ�
         *
         * @retval true   �ǂݍ��݉\
         * @retval false  �ǂݍ��ݕs�\
         */
        virtual bool CanRead( void ) const;

        /**
         * @brief �������݂��o���邩�ǂ����𒲂ׂ�
         *
         * @retval true   �������݉\
         * @retval false  �������ݕs�\
         */
        virtual bool CanWrite( void ) const;

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
        virtual bool SetPriority( s32 priority );

        /**
         * @brief ���O�ɍs�����t�@�C���A�N�Z�X�̌��ʂ��擾����
         */
        inline Result GetLastAccessResult( void ) const { return m_access_result; }
        /**
         * @brief ���O�̍s�����t�@�C���A�N�Z�X�̌��ʂ��擾�Fnn::Result��
         */
        inline nn::Result GetLastAccessNNResult( void ) const { return m_nn_result; }


        /**
         * @brief �t�@�C���L���b�V�����f�o�C�X�ɏ����߂�
         */
        Result Flush( void );


      private:

        nn::fs::FileStream m_nn_file_stream;
        b32 m_file_open_flag;        // �t�@�C�����J���Ă��邩�ǂ���
        OpenMode m_file_open_mode;   // �I�[�v�����[�h
        nn::Result m_nn_result;      // 
        Result m_access_result;      // �t�@�C���A�N�Z�X�̌���
        bool  m_need_flush;          //  Flush����K�v�����邩

        bit32 GetNnOpenMode( OpenMode mode ) const;
    };


  } // namespace fs
} // namespace gfl

#endif // __GFL_FILEBASE_H__
