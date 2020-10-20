#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @brief  バックアップファイルの読み込みを行うスレッド
 * @file   gfl2_FsCtrBackupThread.h
 * @author obata_toshihiro
 * @date   2011.02.17
 */
//=============================================================================
#ifndef __GFL2_BACKUPTHREAD_H__
#define __GFL2_BACKUPTHREAD_H__
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_Thread.h>
#include <fs/include/CTR/gfl2_FsCtrBackupSystem.h>

namespace gfl2 {
  namespace fs { 

    class FsCtrBackupSystem;


    class FsCtrBackupThread : public gfl2::thread::ThreadInterface
    {
      public:

        /**
         * @brief 動作モード
         */
        enum Mode {
          MODE_SAVE,
          MODE_LOAD,
          MODE_FORMAT,
          MODE_CREATE,
        };

        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param heap           スレッドのスタック領域に使用するヒープ
         * @param stack_size     スレッドのスタック領域のサイズ
         * @param backup_system  バックアップシステム
         * @param mode           動作モード
         *
         * @note インスタンス生成後, 自動的にスレッドを起動する.
         */
        FsCtrBackupThread( 
            gfl2::heap::HeapBase* heap,
            u32 stack_size,
            FsCtrBackupSystem* backup_system,
            Mode mode );

        /**
         * @brief デストラクタ
         */
        virtual ~FsCtrBackupThread(); 

        //---------------------------------------------------------------------
        // フォーマット
        //---------------------------------------------------------------------
        struct FormatParam
        {
          size_t maxDirectoryCount;                // ディレクトリの最大数
          size_t maxFileCount;                     // ファイルの最大数
          FsCtrBackupSystem::DuplicateFlag isDuplicate; // 二重化するかどうか
        };

        /**
         * @brief MODE_FORMAT の動作パラメータをセットする
         * @param param フォーマット設定
         */
        void SetFormatParam( const FormatParam& param );

        //---------------------------------------------------------------------
        //  ファイル生成
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
        // 結果
        //--------------------------------------------------------------------- 
        /**
         * @brief 実行結果を取得する
         */
        FsCtrBackupSystem::Result GetResult( void ) const;

        FsCtrBackupThread::Mode GetMode( void ) const { return m_mode; }

        //--------------------------------------------------------------------------------
        /**
         * @brief 開始
         */
        //--------------------------------------------------------------------------------
        void Start( u8 thread_priority );

        //--------------------------------------------------------------------------------
        /**
         * @brief 終了
         */
        //--------------------------------------------------------------------------------
        void Kill( void );
  
        //--------------------------------------------------------------------------------
        /**
         * @brief 生存確認
         */
        //--------------------------------------------------------------------------------
        bool IsAlive( void );

      private: 

        static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1; // パス文字列の最大長( 終端文字を含む )
        wchar_t m_path[ MAX_FILE_PATH_LENGTH ];

        Mode m_mode;
        FormatParam m_format_param;
        CreateParam m_create_param;
        FsCtrBackupSystem* m_backup_system;
        FsCtrBackupSystem::Result m_result;
        gfl2::thread::Thread* m_pThread;

        virtual void Function( void );
    }; 


  }  // namespace fs
}  // namespace gfl


#endif // __GFL2_BACKUPTHREAD_H__ 
#endif
