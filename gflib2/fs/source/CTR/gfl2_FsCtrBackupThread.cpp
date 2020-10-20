#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @brief  バックアップファイルの読み書きを行うスレッド
 * @file   gfl2_FsCtrBackupThread.cpp
 * @author obata_toshihiro
 * @date   2011.02.17
 */
//=============================================================================
#include "gfl2_FsCtrBackupThread.h" 
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <string.h>
namespace gfl2 {
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
    FsCtrBackupThread::FsCtrBackupThread(
        gfl2::heap::HeapBase* heap,
        u32 stack_size,
        FsCtrBackupSystem* backup_system,
        Mode mode ) :
      m_mode( mode ),
      m_format_param(),
      m_create_param(),
      m_backup_system( backup_system ),
      m_result( FsCtrBackupSystem::RESULT_OK ),
      m_pThread(NULL)
    {
      m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, stack_size);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    FsCtrBackupThread::~FsCtrBackupThread()
    {
      GFL_SAFE_DELETE( m_pThread );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief MODE_FORMAT の動作パラメータをセットする
     * @param param フォーマット設定
     */
    //-------------------------------------------------------------------------
    void FsCtrBackupThread::SetFormatParam( const FormatParam& param )
    {
      m_format_param = param;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void FsCtrBackupThread::SetCreateParam( const CreateParam& cparam )
    {
      m_create_param = cparam;
      ::std::wcsncpy( m_path, cparam.filePath, MAX_FILE_PATH_LENGTH - 1 );
      if ( ::std::wcslen(cparam.filePath) == MAX_FILE_PATH_LENGTH - 1 )
      {
        m_path[MAX_FILE_PATH_LENGTH - 1] = L'\0';
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実行結果を取得する
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupThread::GetResult( void ) const
    {
      return m_result;
    }


    /**
     * @brief スレッドメイン処理
     * @note 読み込み( 書き込み ) >> 自殺
     */
    void FsCtrBackupThread::Function( void )
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
          m_result = m_backup_system->CreateFsCtrBackupFile(
              m_path,
              m_create_param.fileSize );
        }
        break;
      }

      this->Kill();
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief 開始
     */
    //--------------------------------------------------------------------------------
    void FsCtrBackupThread::Start( u8 thread_priority )
    {
      m_pThread->Start( thread_priority );
    }
    //--------------------------------------------------------------------------------
    /**
      * @brief 終了
      */
    //--------------------------------------------------------------------------------
    void FsCtrBackupThread::Kill( void )
    { 
      m_pThread->Kill(); 
    }
  
    //--------------------------------------------------------------------------------
    /**
      * @brief 生存確認
      */
    //--------------------------------------------------------------------------------
    bool FsCtrBackupThread::IsAlive( void )
    { 
      return m_pThread->IsAlive(); 
    }

  } // namespace fs
} // namespace gfl
#endif
