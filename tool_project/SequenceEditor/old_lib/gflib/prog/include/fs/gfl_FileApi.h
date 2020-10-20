//========================================================================================
/**
 * @file  gfl_FileApi.h
 * @brief GAMEFREAK Library FileIO System ( System API )
 * @author  GAMEFREAK inc.
 * @date  2010.09.24
 */
//========================================================================================
#ifndef __GFL_FILEAPI_H__
#define __GFL_FILEAPI_H__ 
#pragma once

#include <gfl_Base.h>
#include <gfl_Heap.h>


namespace gfl {
  namespace fs {

    // ファイルアクセス優先順位
    enum{
      PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// 利用注意！！！　ドキュメントを良く読むこと
      PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// 普通
      PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// 低い
    };

    class FileSystem
    {
      public:

        // パス文字列の最大長( 終端文字を含む )
        static const u32 MAX_PATH_LENGTH = 256;


        //------------------------------------------------------------------------------------
        /**
         * @brief fsライブラリを初期化する
         *
         * @param heap            ライブラリの作業領域として使用するヒープ
         * @param device_heap     ライブラリの作業領域として使用するデバイスヒープ
         * @param rom_mount_flag  ROMアーカイブをマウントするかどうか
         *
         * @note fsライブラリにあるクラスや関数を使うためには, 先にこの関数を呼ぶ必要があります.
         */
        //------------------------------------------------------------------------------------
        static void Initialize( heap::HeapBase* heap, heap::HeapBase* device_heap, bool rom_mount_flag= true );

        //------------------------------------------------------------------------------------
        /**
         * @brief fsライブラリを終了する
         */
        //------------------------------------------------------------------------------------
        static void Finalize();

        //------------------------------------------------------------------------------------
        /**
         * @brief fsライブラリが初期化されているかどうかを調べる
         *
         * @retval true   初期化済み
         * @retval false  初期化されていない
         */
        //------------------------------------------------------------------------------------
        static bool IsInitialized();


        //------------------------------------------------------------------------------------
        /**
         * @brief "パス + ファイル名" の文字列から, "パス" と "ファイル名" をそれぞれ抽出する.
         * @param[in]  src_string      抽出対象の文字列( 文字数MAX_PATH_LENGTH以下であること )
         * @param[out] dest_path       抽出したパスの格納先バッファ
         * @param[out] dest_file_name  抽出したファイル名の格納先バッファ
         *
         * @retval true   成功
         * @retval false  失敗
         *
         * @note
         * たとえば, "rom:/test_data.bin" の場合,
         * パス名 = "rom:/"
         * ファイル名 = "test_data.bin"
         */
        //------------------------------------------------------------------------------------
        static bool GetPathAndFileName( 
            const wchar_t* src_string,
            wchar_t* dest_path,
            wchar_t* dest_file_name ); 

        //------------------------------------------------------------------------------------
        /**
         * @brief ロムアーカイブ以下にファイルが存在するかどうかを調べる
         * @param filename チェック対象のファイル名( rom:/xxxx )
         * @retval true  ファイルが存在する
         * @retval false ファイルが存在しない
         */
        //------------------------------------------------------------------------------------
        static bool CheckRomFileExist( const wchar_t* filename ); 


        //----------------------------------------------------------------------------
        /**
         *	@brief  ファイルシステム全体の読み込み優先順位を設定
         *	
         *	@param  base_priority           ファイル読み込み優先順位
         *	@param  main_thread_priority    メインスレッドでの、ファイル読み込み優先順位
         *
         *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// 利用注意！！！　ドキュメントを良く読むこと
         *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// 普通
         *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// 低い
         *
         *   main_thread_priorityは、この関数コール後にOpenされたRomFileに適応されます。
         */
        //-----------------------------------------------------------------------------
        static void SetPriority( s32 base_priority, s32 main_thread_priority );

        //----------------------------------------------------------------------------
        /**
         *	@brief  メインスレッドでのファイル読み込み優先順位を取得
         */
        //-----------------------------------------------------------------------------
        static s32 GetMainThreadPriority( void );

      private:

        /**
         * @brief ROMアーカイブをマウントする
         */
        static void MountRomArchive( gfl::heap::HeapBase* heap );


        /**
         * @brief メインスレッド優先順位
         */
        static s32 m_MainThreadPriority;
        
#if GFL_FILE_DEBUG
      public:

        /// デバッグプリントモード
        enum DebugPrintMode{
          DEBUG_PRINT_NONE,           ///< デバッグプリントなし
          DEBUG_PRINT_MAIN_THREAD,    ///< メインスレッド読み込みを表示
          DEBUG_PRINT_OTHER_THREAD,   ///< メインスレッド以外の読み込みを表示
          DEBUG_PRINT_ALL,            ///< 全読み込みを表示

          DEBUG_PRINT_MAX,            ///< システムで使用
        };

        /// プロファイリングパラメータ
        struct ProfilerParam{
          s32   main_thread_us;         ///< メインスレッドで読み込みを許可する時間（マイクロ秒）
          DebugPrintMode debug_print;   ///< デバッグプリントモード
          bool  main_thread_us_over_assert; ///< main_thread_usをオーバーしたら止めるか？
        };
        /// プロファイラーの設定
        static void SetProfiler( bool flag, const ProfilerParam* cp_param );

        /// プロファイラーが動作中かチェック
        static bool IsProfiling( void );
        
        /// メインループで１回実行してください。
        static void UpdateProfiler( void );

        /// １読み込みデータを監視
        static void AddProfilerData( const wchar_t& path, bit32 thread_id, s32 size, s32 offset, s32 time );

        /// 最後のプロファイル結果がエラーかチェック
        static bool IsLastProfilerError( void );
        static s32 GetLastProfilerTotalUs( void );
        

        /**
         * @brief SDカードアーカイブをマウントする
         */
        static void MountSdmcArchive( void );

      private:
        
        /// 1フレーム内の読み込み情報のバッファ
        static const u32 PR_FILE_BUFFER_COUNT   = 64;
        static const u32 PR_FILE_BUFFER_STRNUM  = 32;
        static bool m_ProfilerFlag;
        static ProfilerParam m_ProfilerParam;
        static wchar_t m_ProfilerFileNameBuffer[PR_FILE_BUFFER_COUNT][PR_FILE_BUFFER_STRNUM];
        static s32 m_ProfilerFileSizeBuffer[PR_FILE_BUFFER_COUNT];
        static s32 m_ProfilerFileOffsetBuffer[PR_FILE_BUFFER_COUNT];
        static s32 m_ProfilerFileUsBuffer[PR_FILE_BUFFER_COUNT];
        static s32 m_ProfilerTotalUs;
        static s32 m_ProfilerTotalSize;
        static s32 m_ProfilerFileCount;
        static nn::os::CriticalSection m_criticalSectionForProfilerAccess;
        static bool m_ProfilerLastError;
        static s32 m_ProfilerLastTotalUs;
#endif // GFL_FILE_DEBUG


        // HIO

#if GFL_HIO_ENABLE
      public:
        //------------------------------------------------------------------------------------
        /**
         * @brief HIOライブラリを初期化する
         * @param device_heap  HIOライブラリの作業ワークを確保するデバイスヒープ
         */
        //------------------------------------------------------------------------------------
        static void InitializeHio( gfl::heap::HeapBase* device_heap );

        //------------------------------------------------------------------------------------
        /**
         * @brief HIOライブラリを終了する
         */
        //------------------------------------------------------------------------------------
        static void FinalizeHio( void );

        //------------------------------------------------------------------------------------
        /**
         * @brief HIOアーカイブをマウントする
         * @param device_heap     HIOライブラリの作業ワークを確保するデバイスヒープ
         * @param archive_name    HIOアーカイブ名
         * @param root_directory  HIOアーカイブのルートディレクトリ
         */
        //------------------------------------------------------------------------------------
        static void MountHioArchive( 
            gfl::heap::HeapBase* device_heap, char* archive_name, char* root_directory );

        //------------------------------------------------------------------------------------
        /**
         * @brief ホストファイルシステムにファイルが存在するかどうかを調べる
         * @param filename チェック対象のファイル名( フルパスで指定 )( Mountされているアーカイブ名ならば内部でフルパスに置換する )
         * @retval true  ファイルが存在する
         * @retval false ファイルが存在しない
         */
        //------------------------------------------------------------------------------------
        static bool CheckHioFileExist( const wchar_t* filename );


      private:
        static bool m_hio_initialize_flag;                       // HIOライブラリを初期化したかどうか
        static void* m_hio_library_work;                         // HIOライブラリの作業ワーク

        static const u8 MAX_HIO_MOUNT_COUNT = 4;                 // HIOアーカイブをマウントできる数
        static u8 m_hio_mount_count;                             // HIOアーカイブをマウントした数
        static void* m_hio_mount_buffer[ MAX_HIO_MOUNT_COUNT ];  // HIOアーカイブのマウントバッファ

        static char     m_hio_mount_archive_name[ MAX_HIO_MOUNT_COUNT ][ 256 ];  // HIOアーカイブ名
        static wchar_t  m_hio_mount_root_dir[ MAX_HIO_MOUNT_COUNT ][ 256 ];   // HIOアーカイブのルートパス
#endif
    };



  } // namespace fs
} // namespace gfl 


#endif // __GFL_FILEAPI_H__
