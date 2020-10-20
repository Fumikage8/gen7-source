//=======================================================================================
/**
 * @brief �t�@�C���A�N�Z�X���N���X
 * @file  gfl_File.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include <gflib_core.h>
#include <fs/gfl_FileBase.h>
#include "gfl_FileAccessThread.h"


namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    File::File( void ) :
      m_file_access_thread( NULL )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-----------------------------------------------------------------------------------
    File::~File( void )
    {
      // �񓯊��ǂݍ��ݒ��͔j���ł��Ȃ�
      // File::CancelAsyncAccess()�Ŕ񓯊��������L�����Z����,
      // File::CheckAsyncAccess()�Ŕ񓯊������̊�����҂��Ă���
      // File�C���X�^���X��j�����Ă�������.
      GFL_ASSERT( !this->CheckAsyncAccess() );  //@check
      this->CancelAsyncAccess();
      while( this->CheckAsyncAccess() ) {
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
      }
      this->DeleteFileAccessThread();
    }






    //-------------------------------------------------------------------------------
    /**
     * @brief �t�@�C������ǂݍ���
     *
     * @param[out] buffer  �ǂݍ��񂾃f�[�^�̊i�[��
     * @param[in]  size    �ǂݍ��ރT�C�Y
     *
     * @return ���ۂɓǂݍ��񂾃T�C�Y
     */
    //-------------------------------------------------------------------------------
    size_t File::Read( void* buffer, u32 size )
    {
      GFL_UNUSED(buffer);
      GFL_UNUSED(size);
      GFL_PRINT( "���̃N���X�̓t�@�C���̓ǂݍ��݂ɑΉ����Ă��܂���.\n" );
      return 0;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �t�@�C���ɏ�������
     *
     * @param[in] buffer  �������ރf�[�^
     * @param[in] size    �������ރT�C�Y
     *
     * @return ���ۂɏ������񂾃T�C�Y
     */
    //-------------------------------------------------------------------------------
    size_t File::Write( const void* buffer, u32 size )
    {
      GFL_UNUSED(buffer);
      GFL_UNUSED(size);
      GFL_PRINT( "���̃N���X�̓t�@�C���̏������݂ɑΉ����Ă��܂���.\n" );
      return 0;
    }




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
     * @param[in]  callback_func         �ǂݍ��ݏI�����̃R�[���o�b�N�֐�
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
    bool File::StartAsyncRead( 
        gfl::heap::HeapBase* heap,
        const wchar_t* file_path, size_t file_offset, 
        size_t total_data_size, size_t one_time_access_size, void* buffer, 
        gfl::base::Thread::Priority thread_priority,
        CallBackFuncOnAsyncAccessFinished callback_func, void* callback_work )
    {
      if( this->CheckAsyncAccess() ) {
        return false;  // �񓯊��������s��
      }

      // �����̃X���b�h���폜
      this->DeleteFileAccessThread();

      // �X���b�h�𐶐�
      FileAccessThread::OperationSetting thread_setting;
      thread_setting.file = this;
      thread_setting.open_mode = gfl::fs::OPEN_MODE_READ;
      thread_setting.file_path = file_path;
      thread_setting.file_offset = file_offset;
      thread_setting.total_data_size = total_data_size;
      thread_setting.one_time_access_size = one_time_access_size;
      thread_setting.data_buffer = buffer;
      thread_setting.callback_func = callback_func;
      thread_setting.callback_work = callback_work;
      m_file_access_thread = GFL_NEW_LOW( heap ) FileAccessThread( heap, 4096, thread_setting );

      // �X���b�h�J�n
      m_file_access_thread->Start( thread_priority );
      return true;
    }

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
     * @param[in]  callback_work         �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N
     * @return �񓯊��������݂��J�n�ł������ǂ���
     * @retval true   �J�n�ł���
     * @retval false  �J�n�ł��Ȃ�����
     *
     * @note 
     * CheckAsyncAccess()�ŏ������݂̏I����҂�����, 
     * EndAsyncAccess()���Ă�ł��������B
     */
    //-------------------------------------------------------------------------------
    bool File::StartAsyncWrite( 
        gfl::heap::HeapBase* heap,
        const wchar_t* file_path, bool add_write, size_t file_offset, 
        size_t total_data_size, size_t one_time_access_size, void* buffer,
        gfl::base::Thread::Priority thread_priority,
        CallBackFuncOnAsyncAccessFinished callback_func, void* callback_work )
    {
      if( this->CheckAsyncAccess() ) {
        return false;  // �񓯊��������s��
      }

      // �����̃X���b�h���폜
      this->DeleteFileAccessThread();

      // �X���b�h�𐶐�
      FileAccessThread::OperationSetting thread_setting;
      thread_setting.file = this;
      thread_setting.open_mode = add_write ? gfl::fs::OPEN_MODE_WRITE_ADD : gfl::fs::OPEN_MODE_WRITE_NEW;
      thread_setting.file_path = file_path;
      thread_setting.file_offset = file_offset;
      thread_setting.total_data_size = total_data_size;
      thread_setting.one_time_access_size = one_time_access_size;
      thread_setting.data_buffer = buffer;
      thread_setting.callback_func = callback_func;
      thread_setting.callback_work = callback_work;
      m_file_access_thread = GFL_NEW_LOW( heap ) FileAccessThread( heap, 4096, thread_setting );

      // �X���b�h�J�n
      m_file_access_thread->Start( thread_priority );
      return true;
    }


    //-------------------------------------------------------------------------------
    /**
     * @brief �񓯊��ǂݍ���( �������� )�����s�����ǂ����𒲂ׂ�
     * @retval true   ���s��
     * @retval false  ���s���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------------
    bool File::CheckAsyncAccess( void ) const
    {
      if( m_file_access_thread && m_file_access_thread->IsAlive() )
      {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief ���s���̔񓯊��ǂݍ���( �������� )���L�����Z������
     *
     * @note 
     * CheckAsyncAccess()�Ŕ񓯊������̏I�����m�F������,
     * EndAsyncAccess()���Ă�ł��������B
     */
    //-------------------------------------------------------------------------------
    void File::CancelAsyncAccess( void )
    {
      if( this->CheckAsyncAccess() ) {
        m_file_access_thread->Kill();
      }
    }

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
    void File::EndAsyncAccess( void )
    {
      this->DeleteFileAccessThread();
    }

    /**
     * @brief �񓯊������X���b�h��j������
     */
    void File::DeleteFileAccessThread( void )
    {
      if( m_file_access_thread ) {
        GFL_DELETE m_file_access_thread;
        m_file_access_thread = NULL;
      }
    }




    /**
     * @brief nn::fs::PositionBase �ɕϊ�����
     */
    nn::fs::PositionBase File::GetPositionBase( SeekBasePos base_pos ) const
    {
      switch( base_pos ) 
      {
      case SEEK_BASE_BEGIN: 
        return nn::fs::POSITION_BASE_BEGIN;

      case SEEK_BASE_CURRENT: 
        return nn::fs::POSITION_BASE_CURRENT;

      case SEEK_BASE_END: 
        return nn::fs::POSITION_BASE_END;

      default:
        GFL_ASSERT(0); //@check
        return nn::fs::POSITION_BASE_BEGIN;
      }
    }







#if 0		// Uncomp���d���̂Ŏg��Ȃ������ŁB
		//====================================================================================
		//====================================================================================
		// ���k�t�@�C���W�J		2011/10/07 by nakahiro
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
    bool File::StartAsyncRead( 
        gfl::heap::HeapBase* heap,
        gfl::heap::HeapBase* buffer_heap,
        const wchar_t* file_path, size_t file_offset, 
        size_t total_data_size, size_t one_time_access_size, void** buffer, u32 buffer_align,
        gfl::base::Thread::Priority thread_priority,
        CallBackFuncOnAsyncAccessFinished callback_func, void* callback_work )
    {
      if( this->CheckCompAsyncAccess() ) {
        return false;  // �񓯊��������s��
      }

      // �����̃X���b�h���폜
      this->DeleteCompFileReadThread();

      // �X���b�h�𐶐�
      CompFileReadThread::OperationSetting thread_setting;
      thread_setting.file = this;
			thread_setting.heap = buffer_heap;
      thread_setting.file_path = file_path;
      thread_setting.file_offset = file_offset;
      thread_setting.total_data_size = total_data_size;
      thread_setting.one_time_access_size = one_time_access_size;
      thread_setting.data_buffer = buffer;
      thread_setting.data_buffer_align = buffer_align;
      thread_setting.callback_func = callback_func;
      thread_setting.callback_work = callback_work;
      m_comp_file_read_thread = GFL_NEW_LOW( heap ) CompFileReadThread( heap, 4096, thread_setting );

      // �X���b�h�J�n
      m_comp_file_read_thread->Start( thread_priority );
      return true;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �񓯊��ǂݍ���( �������� )�����s�����ǂ����𒲂ׂ�
     * @retval true   ���s��
     * @retval false  ���s���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------------
    bool File::CheckCompAsyncAccess( void ) const
    {
      if( m_comp_file_read_thread && m_comp_file_read_thread->IsAlive() )
      {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief ���s���̔񓯊��ǂݍ���( �������� )���L�����Z������
     *
     * @note 
     * CheckCompAsyncAccess()�Ŕ񓯊������̏I�����m�F������,
     * EndCompAsyncAccess()���Ă�ł��������B
     */
    //-------------------------------------------------------------------------------
    void File::CancelCompAsyncAccess( void )
    {
      if( this->CheckCompAsyncAccess() ) {
        m_comp_file_read_thread->Kill();
      }
    }

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
    void File::EndCompAsyncAccess( void )
    {
      this->DeleteCompFileReadThread();
    }

    /**
     * @brief �񓯊������X���b�h��j������
     */
    void File::DeleteCompFileReadThread( void )
    {
      if( m_comp_file_read_thread ) {
        GFL_DELETE m_comp_file_read_thread;
        m_comp_file_read_thread = NULL;
      }
    }
#endif

  } // namespace fs
} // namespace gfl
