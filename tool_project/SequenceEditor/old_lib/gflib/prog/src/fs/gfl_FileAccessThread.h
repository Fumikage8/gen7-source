#ifndef __GFL_FILE_ACCESS_THREAD_H__
#define __GFL_FILE_ACCESS_THREAD_H__
//========================================================================================
/**
 * @brief  �t�@�C���ւ̓ǂݏ������s���X���b�h
 * @file   gfl_FileAccessThread.h
 * @author obata_toshihiro
 * @date   2011.04.22
 */
//========================================================================================
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_FileBase.h> 

namespace gfl {
  namespace fs { 


    class FileAccessThread : public gfl::base::Thread
    {
      public:

        /**
         * @brief �X���b�h�̓�����`����p�����[�^
         */
        struct OperationSetting {
          gfl::fs::File* file;          // �t�@�C���A�N�Z�X�����s����I�u�W�F�N�g
          gfl::fs::OpenMode open_mode;  // �t�@�C���̈�����( �ǂނ̂�, �����̂� )
          const wchar_t* file_path;     // �A�N�Z�X�Ώۃt�@�C���̃p�X
          size_t file_offset;           // �ǂݍ��ݐ�( �������ݐ� )�̃I�t�Z�b�g�ʒu
          size_t total_data_size;       // �g�[�^���œǂݍ���( �������� )�f�[�^�T�C�Y
          size_t one_time_access_size;  // 1��̃t�@�C���A�N�Z�X�œǂݍ��ރf�[�^�T�C�Y( �������� )�f�[�^�T�C�Y
          void* data_buffer;            // �ǂݍ��񂾃f�[�^�̊i�[��( �������ރf�[�^ )
          gfl::fs::File::CallBackFuncOnAsyncAccessFinished callback_func;  // �A�N�Z�X�I�����̃R�[���o�b�N�֐�( �K�v�Ȃ��Ȃ� NULL )
          void* callback_work;          // �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( �K�v�Ȃ��Ȃ� NULL )
        };

        //--------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         * @param heap        �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
         * @param stack_size  �X���b�h�̃X�^�b�N�̈�̃T�C�Y
         * @param setting     �X���b�h�̓�����`����p�����[�^
         */
        //--------------------------------------------------------------------------------
        FileAccessThread( gfl::heap::HeapBase* heap, u32 stack_size, const OperationSetting& setting );

        //--------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //--------------------------------------------------------------------------------
        virtual ~FileAccessThread(); 

        //--------------------------------------------------------------------------------
        /**
         * @brief ���ۂɓǂݍ���( �������� )�T�C�Y���擾����
         */
        //--------------------------------------------------------------------------------
        inline size_t GetOperatedSize( void ) const { return m_operated_size; }



      private:
        OperationSetting m_setting;
        size_t m_operated_size;  // ���ۂɓǂݍ���( �������� )�T�C�Y
        bool m_file_close_flag;  // �I�����Ƀt�@�C������邩�ǂ���

        /**
         * @brief �X���b�h�J�n����
         * @note gfl::base::Thread::Initialize()���I�[�o�[���C�h
         */
        virtual void Initialize( void );

        /**
         * @brief �X���b�h���C������
         * @note �t�@�C���A�N�Z�X�����������玩�E����
         * @note gfl::base::Thread::Main()���I�[�o�[���C�h
         */
        virtual void Main( void ); 

        /**
         * @brief �I������
         * @note gfl::base::Thread::Finalize()���I�[�o�[���C�h
         */
        virtual void Finalize( void );

    }; 


#if 0		// Uncomp���d���̂Ŏg��Ȃ������ŁB
		//====================================================================================
		//====================================================================================
		// ���k�t�@�C���W�J		2011/10/07 by nakahiro
		//====================================================================================
		//====================================================================================

    class CompFileReadThread : public gfl::base::Thread
    {
      public:

        /**
         * @brief �X���b�h�̓�����`����p�����[�^
         */
        struct OperationSetting {
          gfl::fs::File* file;          // �t�@�C���A�N�Z�X�����s����I�u�W�F�N�g
					gfl::heap::HeapBase * heap;		// �f�[�^�W�J�Ɏg�p����q�[�v
          const wchar_t* file_path;     // �A�N�Z�X�Ώۃt�@�C���̃p�X
          size_t file_offset;           // �ǂݍ��ݐ�̃I�t�Z�b�g�ʒu
          size_t total_data_size;       // �g�[�^���œǂݍ��ވ��k�f�[�^�T�C�Y
          size_t one_time_access_size;  // 1��̃t�@�C���A�N�Z�X�œǂݍ��ވ��k�f�[�^�T�C�Y
          void** data_buffer;           // �ǂݍ��񂾃f�[�^�̊i�[��
					u32	data_buffer_align;				// �W�J�f�[�^�i�[�ꏊ�̃A���C��
          gfl::fs::File::CallBackFuncOnAsyncAccessFinished callback_func;  // �A�N�Z�X�I�����̃R�[���o�b�N�֐�( �K�v�Ȃ��Ȃ� NULL )
          void* callback_work;          // �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( �K�v�Ȃ��Ȃ� NULL )
        };

        //--------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         * @param heap        �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
         * @param stack_size  �X���b�h�̃X�^�b�N�̈�̃T�C�Y
         * @param setting     �X���b�h�̓�����`����p�����[�^
         */
        //--------------------------------------------------------------------------------
        CompFileReadThread( gfl::heap::HeapBase * heap, u32 stack_size, const OperationSetting& setting );

        //--------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //--------------------------------------------------------------------------------
        virtual ~CompFileReadThread(); 

        //--------------------------------------------------------------------------------
        /**
         * @brief ���ۂɓǂݍ���( �������� )�T�C�Y���擾����
         */
        //--------------------------------------------------------------------------------
        inline size_t GetOperatedSize( void ) const { return m_operated_size; }

      private:
        OperationSetting m_setting;
        size_t m_operated_size;  // ���ۂɓǂݍ���( �������� )�T�C�Y
        bool m_file_close_flag;  // �I�����Ƀt�@�C������邩�ǂ���

				bool comp_flag;								// ���k�f�[�^���X���b�h�W�J����ꍇ�̃t���O
				void * tmpBuff;								// ���k�f�[�^�W�J���邽�߂̃o�b�t�@
				u32	comp_sequence;						// ���k�f�[�^�W�J�V�[�P���X
				nn::cx::UncompContextLZ	context;

        /**
         * @brief �X���b�h�J�n����
         * @note gfl::base::Thread::Initialize()���I�[�o�[���C�h
         */
        virtual void Initialize( void );

        /**
         * @brief �X���b�h���C������
         * @note �t�@�C���A�N�Z�X�����������玩�E����
         * @note gfl::base::Thread::Main()���I�[�o�[���C�h
         */
        virtual void Main( void ); 

        /**
         * @brief �I������
         * @note gfl::base::Thread::Finalize()���I�[�o�[���C�h
         */
        virtual void Finalize( void );
    }; 
#endif

  } // namespace fs
} // namespace gfl

#endif // __GFL_FILE_ACCESS_THREAD_H__ 
