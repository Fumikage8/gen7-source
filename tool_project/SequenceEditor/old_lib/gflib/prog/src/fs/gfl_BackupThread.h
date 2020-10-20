//=============================================================================
/**
 * @brief  バックアップファイルの読み込みを行うスレッド
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
        BackupThread( 
            gfl::heap::HeapBase* heap,
            u32 stack_size,
            BackupSystem* backup_system,
            Mode mode );

        /**
         * @brief デストラクタ
         */
        virtual ~BackupThread(); 

        //---------------------------------------------------------------------
        // フォーマット
        //---------------------------------------------------------------------
        struct FormatParam
        {
          size_t maxDirectoryCount;                // ディレクトリの最大数
          size_t maxFileCount;                     // ファイルの最大数
          BackupSystem::DuplicateFlag isDuplicate; // 二重化するかどうか
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
        BackupSystem::Result GetResult( void ) const;

        BackupThread::Mode GetMode( void ) const { return m_mode; }


      private: 

        static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1; // パス文字列の最大長( 終端文字を含む )
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
