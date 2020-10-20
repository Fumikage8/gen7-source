#ifndef __GFL_FILEBASE_H__
#define __GFL_FILEBASE_H__
//=======================================================================================
/**
 * @brief  �t�@�C���A�N�Z�X���N���X
 * @file   gfl_FileBase.h
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#pragma once

#include <base/gfl_Thread.h>
#include <fs/gfl_FileApi.h>

namespace gfl {
  namespace fs {

    class FileAccessThread;
    class CompFileReadThread;


    /**
     * @brief �t�@�C���I�[�v�����[�h
     */
    enum OpenMode {
      OPEN_MODE_READ,       // �ǂݍ��ݗp�ɊJ��
      OPEN_MODE_WRITE_NEW,  // ��ɐV�K�쐬���ď������ݗp�ɊJ��(�����̃t�@�C���͍폜����܂�)
      OPEN_MODE_WRITE_ADD,  // �ǋL�p�ɊJ��(�t�@�C�������݂��Ȃ��ꍇ�͎��s���܂�)
    };

    /**
     * @brief �V�[�N�̋N�_�w��
     */
    enum SeekBasePos {
      SEEK_BASE_BEGIN,    // �t�@�C���̐擪
      SEEK_BASE_CURRENT,  // ���݈ʒu
      SEEK_BASE_END,      // �t�@�C���̖���
    };


    class File
    {
      GFL_FORBID_COPY_AND_ASSIGN( File );

      public:
        //-------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^�E�f�X�g���N�^
         */
        //-------------------------------------------------------------------------------
        File( void );
        virtual ~File( void );

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C�����J��
         *
         * @param[in] path  �t�@�C���p�X
         * @param[in] mode  �I�[�v�����[�h
         *
         * @note ROM�̈�̃t�@�C�����J���ꍇ�� rom:/ �Ŏn�܂�p�X���w�肵�Ă�������.
         *       SD�J�[�h��̃t�@�C�����J���ꍇ�� sdmc:/ �Ŏn�܂�p�X���w�肵�Ă�������.
         *       PC��̃t�@�C�����J���ꍇ�� ��΃p�X���w�肵�Ă�������.
         *
         * @retval true   �I�[�v���ɐ��������ꍇ
         * @retval false  �I�[�v���Ɏ��s�����ꍇ
         */
        //-------------------------------------------------------------------------------
        virtual bool Open( const wchar_t* path, OpenMode mode ) = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C�������
         */
        //-------------------------------------------------------------------------------
        virtual void Close( void ) = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C������ǂݍ���
         *
         * @param[out] buffer  �ǂݍ��񂾃f�[�^�̊i�[�� (4�o�C�g�A���C�������g)
         * @param[in]  size    �ǂݍ��ރT�C�Y
         *
         * @return ���ۂɓǂݍ��񂾃T�C�Y
         *
         * @note
         * �f�t�H���g�̓���Ƃ���,�u�������Ȃ��v��������������Ă��܂�.
         * �h���N���X�œǂݍ��݂��\�ȏꍇ�̂ݎ������Ă�������.
         */
        //-------------------------------------------------------------------------------
        virtual size_t Read( void* buffer, u32 size );

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C���ɏ�������
         *
         * @param[in] buffer  �������ރf�[�^ (4�o�C�g�A���C�������g)
         * @param[in] size    �������ރT�C�Y
         *
         * @return ���ۂɏ������񂾃T�C�Y
         *
         * @note
         * �f�t�H���g�̓���Ƃ���,�u�������Ȃ��v��������������Ă��܂�.
         * �h���N���X�ŏ������݂��\�ȏꍇ�̂ݎ������Ă�������.
         */
        //-------------------------------------------------------------------------------
        virtual size_t Write( const void* buffer, u32 size );

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C�����V�[�N����
         *
         * @param[in] base_pos  �V�[�N�̋N�_�ƂȂ�ʒu
         * @param[in] offset    �N�_�ʒu����̃I�t�Z�b�g
         *
         * @note (�N�_�ʒu + �I�t�Z�b�g) �̈ʒu�Ɉړ����܂��B
         */
        //-------------------------------------------------------------------------------
        virtual void Seek( SeekBasePos base_pos, s32 offset ) = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C���T�C�Y���擾����
         */
        //-------------------------------------------------------------------------------
        virtual size_t GetSize( void ) const = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief ���݈ʒu���擾����
         *
         * @return �t�@�C���擪����̃I�t�Z�b�g[Byte]
         */
        //-------------------------------------------------------------------------------
        virtual u32 GetPosition( void ) const = 0;

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
        virtual bool SetPriority( s32 priority ) = 0;
        

        //-------------------------------------------------------------------------------
        /**
         * @brief ���݈ʒu���t�@�C���̖������ǂ����𒲂ׂ�
         *
         * @retval true   �t�@�C�������̏ꍇ
         * @retval false  �t�@�C�������łȂ��ꍇ
         */
        //-------------------------------------------------------------------------------
        bool IsEOF( void ) const { return ( GetSize() <= GetPosition() ); }

        //-------------------------------------------------------------------------------
        /**
         * @brief �t�@�C�����J���Ă��邩�ǂ����𒲂ׂ�
         *
         * @retval true   �t�@�C�����J���Ă���
         * @retval false  �t�@�C�����J���Ă��Ȃ�
         */
        //-------------------------------------------------------------------------------
        virtual bool IsOpen( void ) const = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief �ǂݍ��݂��o���邩�ǂ����𒲂ׂ�
         *
         * @retval true   �ǂݍ��݉\
         * @retval false  �ǂݍ��ݕs�\
         */
        //-------------------------------------------------------------------------------
        virtual bool CanRead( void ) const = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief �������݂��o���邩�ǂ����𒲂ׂ�
         *
         * @retval true   �������݉\
         * @retval false  �������ݕs�\
         */
        //-------------------------------------------------------------------------------
        virtual bool CanWrite( void ) const = 0;



        // �񓯊�����

        /**
         * @brief �񓯊��������I�������Ƃ��̃R�[���o�b�N�֐�
         * @param operated_data_size  ���ۂɏ������ꂽ�f�[�^�̃T�C�Y
         * @param work                �C�ӂ̃��[�N
         *
         * @note �L�����Z�������ꍇ���Ă΂�܂�.
         * @note operated_data_size ���m�F��, ���������s�������������ǂ������`�F�b�N���Ă�������.
         */
        typedef void (*CallBackFuncOnAsyncAccessFinished)( size_t operated_data_size, void* work );

        //-------------------------------------------------------------------------------
        /**
         * @brief �񓯊��ǂݍ��݂��J�n����
         * @param[in]  heap                  �X���b�h�̐����Ɏg�p����q�[�v
         * @param[in]  file_path             �ǂݍ��ރt�@�C���̃p�X
         * @param[in]  file_offset           �ǂݍ��ݐ�̃t�@�C���擪����̃I�t�Z�b�g
         * @param[in]  total_data_size       �ǂݍ��ރf�[�^�̃T�C�Y
         * @param[in]  one_time_access_size  ���̃t�@�C���A�N�Z�X�œǂݍ��ރf�[�^�T�C�Y
         * @param[out] buffer                �ǂݍ��񂾃f�[�^�̊i�[��
         * @param[in]  thread_priority       �X���b�h�̗D��x( �� 0 �` 31 �� )
         * @param[in]  callback_func         �ǂݍ��ݏI�����̃R�[���o�b�N�֐�( NULL�Ȃ�Ă΂�Ȃ� )
         * @param[in]  callback_work         �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N
         * @return �񓯊��ǂݍ��݂��J�n�ł������ǂ���
         * @retval true   �J�n�ł���
         * @retval false  �J�n�ł��Ȃ�����
         *
         * @note
         * CheckAsyncAccess()�œǂݍ��݂̏I����҂�����,
         * EndAsyncAccess()���Ă�ł��������B
         */
        //-------------------------------------------------------------------------------
        virtual bool StartAsyncRead(
            gfl::heap::HeapBase* heap,
            const wchar_t* file_path, size_t file_offset,
            size_t total_data_size, size_t one_time_access_size, void* buffer,
            gfl::base::Thread::Priority thread_priority,
            CallBackFuncOnAsyncAccessFinished callback_func = NULL, void* callback_work = NULL );

        //-------------------------------------------------------------------------------
        /**
         * @brief �񓯊��������݂��J�n����
         * @param[in]  heap                  �X���b�h�̐����Ɏg�p����q�[�v
         * @param[in]  file_path             �������ރt�@�C���̃p�X
         * @param[in]  add_write             �ǋL���邩�ǂ���
         * @param[in]  file_offset           �������ݐ�̃t�@�C���擪����̃I�t�Z�b�g
         * @param[in]  total_data_size       �������ރf�[�^�̃T�C�Y
         * @param[in]  one_time_access_size  ���̃t�@�C���A�N�Z�X�ŏ������ރf�[�^�T�C�Y
         * @param[out] buffer                �������񂾃f�[�^�̊i�[��
         * @param[in]  thread_priority       �X���b�h�̗D��x( �� 0 �` 31 �� )
         * @param[in]  callback_func         �������ݏI�����̃R�[���o�b�N�֐�
         * @param[in]  callback_work         �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( NULL�Ȃ�Ă΂�Ȃ� )
         * @return �񓯊��������݂��J�n�ł������ǂ���
         * @retval true   �J�n�ł���
         * @retval false  �J�n�ł��Ȃ�����
         *
         * @note
         * CheckAsyncAccess()�ŏ������݂̏I����҂�����,
         * EndAsyncAccess()���Ă�ł��������B
         */
        //-------------------------------------------------------------------------------
        virtual bool StartAsyncWrite(
            gfl::heap::HeapBase* heap,
            const wchar_t* file_path, bool add_write, size_t file_offset,
            size_t total_data_size, size_t one_time_access_size, void* buffer,
            gfl::base::Thread::Priority thread_priority,
            CallBackFuncOnAsyncAccessFinished callback_func = NULL, void* callback_work = NULL );

        //-------------------------------------------------------------------------------
        /**
         * @brief �񓯊��ǂݍ���( �������� )�����s�����ǂ����𒲂ׂ�
         * @retval true   ���s��
         * @retval false  ���s���Ă��Ȃ�
         */
        //-------------------------------------------------------------------------------
        virtual bool CheckAsyncAccess( void ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief ���s���̔񓯊��ǂݍ���( �������� )���L�����Z������
         *
         * @note
         * CheckAsyncAccess()�Ŕ񓯊������̏I�����m�F������,
         * EndAsyncAccess()���Ă�ł��������B
         */
        //-------------------------------------------------------------------------------
        virtual void CancelAsyncAccess( void );

        //-------------------------------------------------------------------------------
        /**
         * @brief �񓯊��ǂݍ���( �������� )����������
         *
         * @note
         * �X���b�h�̏I���҂����s���܂��B
         * CheckAsyncAccess()�ŃX���b�h�̏I����҂��Ă���Ă�ł��������B
         * CancelAsyncAccess()���Ă񂾏ꍇ�����l�ł��B
         */
        //-------------------------------------------------------------------------------
        virtual void EndAsyncAccess( void );


#if 0   // Uncomp���d���̂Ŏg��Ȃ������ŁB
        //====================================================================================
        //====================================================================================
        // ���k�t�@�C���W�J   2011/10/07 by nakahiro
        //====================================================================================
        //====================================================================================

        //-------------------------------------------------------------------------------
        /**
         * @brief ���k�f�[�^�̔񓯊��ǂݍ��݂��J�n����
         * @param[in]  heap                  �X���b�h�̐����Ɏg�p����q�[�v
         * @param[in]  buffer_heap           �f�[�^�W�J�Ɏg�p����q�[�v
         * @param[in]  file_path             �������ރt�@�C���̃p�X
         * @param[in]  file_offset           �ǂݍ��ݐ�̃t�@�C���擪����̃I�t�Z�b�g
         * @param[in]  total_data_size       �ǂݍ��ރf�[�^�̃T�C�Y
         * @param[in]  one_time_access_size  ���̃t�@�C���A�N�Z�X�œǂݍ��ރf�[�^�T�C�Y
         * @param[out] buffer                �W�J�f�[�^�i�[�ꏊ
         * @param[in]  buffer_align          �W�J�f�[�^�̃A���C��
         * @param[in]  thread_priority       �X���b�h�̗D��x( �� 0 �` 31 �� )
         * @param[in]  callback_func         �ǂݍ��ݏI�����̃R�[���o�b�N�֐�
         * @param[in]  callback_work         �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N
         * @return �񓯊��ǂݍ��݂��J�n�ł������ǂ���
         * @retval true   �J�n�ł���
         * @retval false  �J�n�ł��Ȃ�����
         *
         * @note
         * CheckCompAsyncAccess()�œǂݍ��݂̏I����҂�����,
         * EndCompAsyncAccess()���Ă�ł��������B
         */
        //-------------------------------------------------------------------------------
        virtual bool StartAsyncRead(
            gfl::heap::HeapBase* heap,
            gfl::heap::HeapBase* buffer_heap,
            const wchar_t* file_path, size_t file_offset,
            size_t total_data_size, size_t one_time_access_size, void** buffer, u32 buffer_align,
            gfl::base::Thread::Priority thread_priority,
            CallBackFuncOnAsyncAccessFinished callback_func = NULL, void* callback_work = NULL );

        //-------------------------------------------------------------------------------
        /**
         * @brief �񓯊��ǂݍ���( �������� )�����s�����ǂ����𒲂ׂ�
         * @retval true   ���s��
         * @retval false  ���s���Ă��Ȃ�
         */
        //-------------------------------------------------------------------------------
        virtual bool CheckCompAsyncAccess( void ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief ���s���̔񓯊��ǂݍ���( �������� )���L�����Z������
         *
         * @note
         * CheckCompAsyncAccess()�Ŕ񓯊������̏I�����m�F������,
         * EndCompAsyncAccess()���Ă�ł��������B
         */
        //-------------------------------------------------------------------------------
        virtual void CancelCompAsyncAccess( void );

        //-------------------------------------------------------------------------------
        /**
         * @brief �񓯊��ǂݍ���( �������� )����������
         *
         * @note
         * �X���b�h�̏I���҂����s���܂��B
         * CheckCompAsyncAccess()�ŃX���b�h�̏I����҂��Ă���Ă�ł��������B
         * CancelCompAsyncAccess()���Ă񂾏ꍇ�����l�ł��B
         */
        //-------------------------------------------------------------------------------
        virtual void EndCompAsyncAccess( void );
#endif


      protected:

        /**
         * @brief nn::fs::PositionBase �ɕϊ�����
         */
        nn::fs::PositionBase GetPositionBase( SeekBasePos base_pos ) const;



      private:

        FileAccessThread* m_file_access_thread;       // �񓯊��A�N�Z�X�X���b�h
#if 0   // Uncomp���d���̂Ŏg��Ȃ������ŁB
        CompFileReadThread* m_comp_file_read_thread;  // �񓯊��A�N�Z�X�X���b�h�i���k�f�[�^�W�J�p�j
#endif

        /**
         * @brief �񓯊������X���b�h��j������
         */
        void DeleteFileAccessThread( void );

        /**
         * @brief �񓯊������X���b�h��j������
         */
        void DeleteCompFileReadThread( void );
    };


  } // namespace fs
} // namespace gfl
#endif // __GFL_FILEBASE_H__
