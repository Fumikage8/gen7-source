//=============================================================================
/**
 * @brief  バックアップファイルの読み書きを行うスレッド
 * @file   gfl_BackupThread.cpp
 * @author obata_toshihiro
 * @date   2011.02.17
 */
//=============================================================================
#include <nw.h>
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_BackupSystem.h>
#include "gfl_BackupThread.h" 

namespace gfl {
  namespace fs {



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap           スレッドのスタック領域に使用するヒープ
     * @param stack_size     スレッドのスタック領域のサイズ
     * @param backup_system  バックアップシステム
     * @param mode           動作モード
     *
     * @note インスタンス生成後, 自動的にスレッドを起動する.
     */
    //-------------------------------------------------------------------------
    BackupThread::BackupThread(
        gfl::heap::HeapBase* heap,
        u32 stack_size,
        BackupSystem* backup_system,
        Mode mode ) :
      Thread( heap, stack_size ),
      m_mode( mode ),
      m_format_param(),
      m_create_param(),
      m_backup_system( backup_system ),
      m_result( BackupSystem::RESULT_OK )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    BackupThread::~BackupThread()
    {
    }


    //-------------------------------------------------------------------------
    /**
     * @brief MODE_FORMAT の動作パラメータをセットする
     * @param param フォーマット設定
     */
    //-------------------------------------------------------------------------
    void BackupThread::SetFormatParam( const FormatParam& param )
    {
      m_format_param = param;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void BackupThread::SetCreateParam( const CreateParam& cparam )
    {
      m_create_param = cparam;
      wcsncpy( m_path, cparam.filePath, MAX_FILE_PATH_LENGTH - 1 );
      if ( wcslen(cparam.filePath) == MAX_FILE_PATH_LENGTH - 1 )
      {
        m_path[MAX_FILE_PATH_LENGTH - 1] = L'\0';
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実行結果を取得する
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupThread::GetResult( void ) const
    {
      return m_result;
    }


    /**
     * @brief スレッドメイン処理
     * @note 読み込み( 書き込み ) >> 自殺
     */
    void BackupThread::Main( void )
    {
      switch( m_mode )
      {
      case MODE_SAVE:
        m_result = m_backup_system->Save();
        break;

      case MODE_LOAD:
        m_result = m_backup_system->Load();
        break;

      case MODE_FORMAT:
        {
          m_result = m_backup_system->Format( 
              m_format_param.maxDirectoryCount,
              m_format_param.maxFileCount,
              m_format_param.isDuplicate );
        }
        break;

      case MODE_CREATE:
        {
          m_result = m_backup_system->CreateBackupFile(
              m_path,
              m_create_param.fileSize );
        }
        break;
      }

      this->Kill();
    }



  } // namespace fs
} // namespace gfl
