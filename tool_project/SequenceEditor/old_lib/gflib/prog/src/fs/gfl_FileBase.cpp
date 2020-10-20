//=======================================================================================
/**
 * @brief ファイルアクセス基底クラス
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
     * @brief コンストラクタ
     */
    //-----------------------------------------------------------------------------------
    File::File( void ) :
      m_file_access_thread( NULL )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------------------
    File::~File( void )
    {
      // 非同期読み込み中は破棄できない
      // File::CancelAsyncAccess()で非同期処理をキャンセルし,
      // File::CheckAsyncAccess()で非同期処理の完了を待ってから
      // Fileインスタンスを破棄してください.
      GFL_ASSERT( !this->CheckAsyncAccess() );  //@check
      this->CancelAsyncAccess();
      while( this->CheckAsyncAccess() ) {
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
      }
      this->DeleteFileAccessThread();
    }






    //-------------------------------------------------------------------------------
    /**
     * @brief ファイルから読み込む
     *
     * @param[out] buffer  読み込んだデータの格納先
     * @param[in]  size    読み込むサイズ
     *
     * @return 実際に読み込んだサイズ
     */
    //-------------------------------------------------------------------------------
    size_t File::Read( void* buffer, u32 size )
    {
      GFL_UNUSED(buffer);
      GFL_UNUSED(size);
      GFL_PRINT( "このクラスはファイルの読み込みに対応していません.\n" );
      return 0;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief ファイルに書き込む
     *
     * @param[in] buffer  書き込むデータ
     * @param[in] size    書き込むサイズ
     *
     * @return 実際に書き込んだサイズ
     */
    //-------------------------------------------------------------------------------
    size_t File::Write( const void* buffer, u32 size )
    {
      GFL_UNUSED(buffer);
      GFL_UNUSED(size);
      GFL_PRINT( "このクラスはファイルの書き込みに対応していません.\n" );
      return 0;
    }




    //-------------------------------------------------------------------------------
    /**
     * @brief 非同期読み込みを開始する
     * @param[in]  heap                  スレッドの生成に使用するヒープ
     * @param[in]  file_path             読み込むファイルのパス
     * @param[in]  file_offset           読み込み先のファイル先頭からのオフセット
     * @param[in]  total_data_size       読み込むデータのサイズ
     * @param[in]  one_time_access_size  一回のファイルアクセスで読み込むデータサイズ
     * @param[out] buffer                読み込んだデータの格納先
     * @param[in]  thread_priority       スレッドの優先度( 高 0 ～ 31 低 )
     * @param[in]  callback_func         読み込み終了時のコールバック関数
     * @param[in]  callback_work         コールバック関数に渡す任意のワーク
     * @return 非同期読み込みが開始できたかどうか
     * @retval true   開始できた
     * @retval false  開始できなかった
     *
     * @note 
     * CheckAsyncAccess()で読み込みの終了を待った後, 
     * EndAsyncAccess()を呼んでください。
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
        return false;  // 非同期処理実行中
      }

      // 既存のスレッドを削除
      this->DeleteFileAccessThread();

      // スレッドを生成
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

      // スレッド開始
      m_file_access_thread->Start( thread_priority );
      return true;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief 非同期書き込みを開始する
     * @param[in]  heap                  スレッドの生成に使用するヒープ
     * @param[in]  file_path             書き込むファイルのパス
     * @param[in]  add_write             追記するかどうか
     * @param[in]  file_offset           書き込み先のファイル先頭からのオフセット
     * @param[in]  total_data_size       書き込むデータのサイズ
     * @param[in]  one_time_access_size  一回のファイルアクセスで書き込むデータサイズ
     * @param[out] buffer                書き込んだデータの格納先
     * @param[in]  thread_priority       スレッドの優先度( 高 0 ～ 31 低 )
     * @param[in]  callback_func         書き込み終了時のコールバック関数
     * @param[in]  callback_work         コールバック関数に渡す任意のワーク
     * @return 非同期書き込みが開始できたかどうか
     * @retval true   開始できた
     * @retval false  開始できなかった
     *
     * @note 
     * CheckAsyncAccess()で書き込みの終了を待った後, 
     * EndAsyncAccess()を呼んでください。
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
        return false;  // 非同期処理実行中
      }

      // 既存のスレッドを削除
      this->DeleteFileAccessThread();

      // スレッドを生成
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

      // スレッド開始
      m_file_access_thread->Start( thread_priority );
      return true;
    }


    //-------------------------------------------------------------------------------
    /**
     * @brief 非同期読み込み( 書き込み )が実行中かどうかを調べる
     * @retval true   実行中
     * @retval false  実行していない
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
     * @brief 実行中の非同期読み込み( 書き込み )をキャンセルする
     *
     * @note 
     * CheckAsyncAccess()で非同期処理の終了を確認した後,
     * EndAsyncAccess()を呼んでください。
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
     * @brief 非同期読み込み( 書き込み )を完了する
     *
     * @note 
     * スレッドの終了待ちを行います。
     * CheckAsyncAccess()でスレッドの終了を待ってから呼んでください。
     * CancelAsyncAccess()を呼んだ場合も同様です。
     */
    //-------------------------------------------------------------------------------
    void File::EndAsyncAccess( void )
    {
      this->DeleteFileAccessThread();
    }

    /**
     * @brief 非同期処理スレッドを破棄する
     */
    void File::DeleteFileAccessThread( void )
    {
      if( m_file_access_thread ) {
        GFL_DELETE m_file_access_thread;
        m_file_access_thread = NULL;
      }
    }




    /**
     * @brief nn::fs::PositionBase に変換する
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







#if 0		// Uncompが重いので使わない方向で。
		//====================================================================================
		//====================================================================================
		// 圧縮ファイル展開		2011/10/07 by nakahiro
		//====================================================================================
		//====================================================================================

    //-------------------------------------------------------------------------------
    /**
     * @brief 圧縮データの非同期読み込みを開始する
     * @param[in]  heap                  スレッドの生成に使用するヒープ
     * @param[in]  buffer_heap           データ展開に使用するヒープ
     * @param[in]  file_path             書き込むファイルのパス
     * @param[in]  file_offset           読み込み先のファイル先頭からのオフセット
     * @param[in]  total_data_size       読み込むデータのサイズ
     * @param[in]  one_time_access_size  一回のファイルアクセスで読み込むデータサイズ
     * @param[out] buffer                展開データ格納場所
     * @param[in]  buffer_align          展開データのアライン
     * @param[in]  thread_priority       スレッドの優先度( 高 0 ～ 31 低 )
     * @param[in]  callback_func         読み込み終了時のコールバック関数
     * @param[in]  callback_work         コールバック関数に渡す任意のワーク
     * @return 非同期読み込みが開始できたかどうか
     * @retval true   開始できた
     * @retval false  開始できなかった
     *
     * @note 
     * CheckCompAsyncAccess()で読み込みの終了を待った後, 
     * EndCompAsyncAccess()を呼んでください。
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
        return false;  // 非同期処理実行中
      }

      // 既存のスレッドを削除
      this->DeleteCompFileReadThread();

      // スレッドを生成
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

      // スレッド開始
      m_comp_file_read_thread->Start( thread_priority );
      return true;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief 非同期読み込み( 書き込み )が実行中かどうかを調べる
     * @retval true   実行中
     * @retval false  実行していない
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
     * @brief 実行中の非同期読み込み( 書き込み )をキャンセルする
     *
     * @note 
     * CheckCompAsyncAccess()で非同期処理の終了を確認した後,
     * EndCompAsyncAccess()を呼んでください。
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
     * @brief 非同期読み込み( 書き込み )を完了する
     *
     * @note 
     * スレッドの終了待ちを行います。
     * CheckCompAsyncAccess()でスレッドの終了を待ってから呼んでください。
     * CancelCompAsyncAccess()を呼んだ場合も同様です。
     */
    //-------------------------------------------------------------------------------
    void File::EndCompAsyncAccess( void )
    {
      this->DeleteCompFileReadThread();
    }

    /**
     * @brief 非同期処理スレッドを破棄する
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
