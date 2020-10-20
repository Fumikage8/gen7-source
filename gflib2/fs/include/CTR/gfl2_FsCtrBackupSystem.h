#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @file   gfl2_FsCtrBackupSystem.h
 * @brief  バックアップシステム
 * @author obata_toshihiro
 * @date   2011.02.16
 *
 * @note セーブデータアーカイブを "data:" でマウントします。
 */
//=============================================================================
#ifndef __GFL2_BACKUPSYSTEM_H__
#define __GFL2_BACKUPSYSTEM_H__
#pragma once
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <base/include/gfl2_Singleton.h>
#include <base/include/gfl2_Queue.h>
#include <fs/include/CTR/gfl2_FsCtrBackupFile.h>

namespace gfl2 {
  namespace fs {

    class FsCtrBackupThread;


    class FsCtrBackupSystem : public gfl2::base::Singleton<FsCtrBackupSystem>
    {
      public:

        /**
         * @brief セーブデータを二重化するかどうかの指定
         */
        enum DuplicateFlag {
          DUPLICATE_ON = true,    // 二重化する
          DUPLICATE_OFF = false,  // 二重化しない
        };

        /**
         * @brief 実行結果
         */
        enum Result {
          RESULT_OK,                     // 成功
          RESULT_CONTINUE,               // 未完了
          RESULT_NG_MEDIA_NOT_EXIST,     // 失敗：メディアが存在しない
          RESULT_NG_NOT_FORMATTED,       // 失敗：セーブデータ領域がフォーマットされていない
          RESULT_NG_BAD_FORMAT,          // 失敗：フォーマットが不正な状態になっている
          RESULT_NG_BROKEN_DATA,         // 失敗：データが壊れているか, 改竄されている
          RESULT_NG_ERROR,               // 失敗：接触不良など, ハードウェア要因
          RESULT_NG_PATH_NOT_EXIST,      // 失敗：指定したパスが存在しない
          RESULT_NG_PATH_ALREADY_EXIST,  // 失敗：指定したパスが既に存在する
          RESULT_NG_LACK_OF_SPACE,       // 失敗：メディアの空き容量不足
          RESULT_NG_UNKNOWN,             // 失敗：想定外のエラー
          RESULT_NG_UNMOUNT,             // 失敗：セーブデータアーカイブがマウントされていない
          RESULT_NG_ALREADY,             // 失敗：( セーブ or ロードを )既に実行中
          RESULT_NG_NOT_START,           // 失敗：( セーブ or ロードを )実行していない
        };

        enum
        {
          MAX_PATH_LENGTH = 256,
        };

        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         *
         * @param heap  システムの作業領域に使用するヒープ
         */
        FsCtrBackupSystem( gfl2::heap::HeapBase* heap );

        /**
         * @brief デストラクタ
         */
        virtual ~FsCtrBackupSystem( void );


        //---------------------------------------------------------------------
        // セーブデータの初期化
        //
        // バックアップ領域を "data:" というアーカイブ名でマウントし,
        // "data:" 以下にフォルダ・ファイルを作成します.
        //---------------------------------------------------------------------
        /**
         * @brief セーブデータ領域をフォーマットする
         *
         * @param max_dir_count   ディレクトリの最大数
         * @param max_file_count  ファイルの最大数
         * @param duplicate       二重化するかどうか
         * @retval true   フォーマットに成功
         * @retval false  フォーマットに失敗
         *
         * @attention 既存のバックアップファイルは全て削除されます.
         */
        Result Format( size_t max_directories, size_t max_files, DuplicateFlag duplicate );

        /**
         * @brief セーブデータ領域の非同期フォーマットを開始する
         *
         * @param thread_priority  フォーマットスレッドのプライオリティ
         * @param max_dir_count    ディレクトリの最大数
         * @param max_file_count   ファイルの最大数
         * @param duplicate        二重化するかどうか
         *
         * @retval RESULT_OK       フォーマット開始
         * @retval RESULT_NG_XXXX  フォーマット開始に失敗
         *
         * @attention 既存のバックアップファイルは全て削除されます.
         */
        Result StartAsyncFormat( 
            u8 thread_priority, 
            size_t max_directories, size_t max_files, DuplicateFlag duplicate );

        /**
         * @brief セーブデータ領域の非同期フォーマットの完了を待つ
         *
         * @retval RESULT_CONTINUE  フォーマット中
         * @retval RESULT_OK        フォーマット終了
         * @retval RESULT_NG_XXXX   フォーマットに失敗
         */
        Result WaitAsyncFormat( void );

        /**
         * @brief バックアップファイルを作成する
         *
         * @param file_path  作成するファイルのパス
         * @param file_size  作成するファイルのサイズ
         * @retval RESULT_OK       ファイルの作成に成功
         * @retval RESULT_NG_XXXX  ファイルの作成に失敗
         */
        Result CreateFsCtrBackupFile( const wchar_t* file_path, size_t file_size );

        /**
         * @brief バックアップファイルを非同期で作成する
         *
         * @param thread_priority  スレッドのプライオリティ
         * @param file_path  作成するファイルのパス
         * @param file_size  作成するファイルのサイズ
         * @retval RESULT_OK       ファイルの作成開始
         * @retval RESULT_NG_XXXX  ファイルの作成に失敗
         */
        Result StartAsyncCreateFile( u8 thread_priority, const wchar_t* file_path, size_t file_size );

        /**
         * @brief バックアップファイルの作成を待つ
         *
         * @param file_path  作成するファイルのパス
         * @param file_size  作成するファイルのサイズ
         * @retval RESULT_OK       ファイルの作成に成功
         * @retval RESULT_NG_XXXX  ファイルの作成に失敗
         */
        Result WaitAsyncCreateFile( void );

        /**
         * @brief バックアップファイルを削除する
         *
         * @param file_path  削除するファイルのパス
         * @retval RESULT_OK       ファイルの削除に成功
         * @retval RESULT_OK 以外  ファイルの削除に失敗
         */
        Result DeleteFsCtrBackupFile( const wchar_t* file_path );

        /**
         * @brief バックアップファイルの有無を調べる
         *
         * @param file_path   調べるファイルのパス
         * @retval true   ファイルは存在する
         * @retval false  ファイルは存在しない
         */
        bool CheckFsCtrBackupFileExist( const wchar_t* file_path );


        //---------------------------------------------------------------------
        // バックアップデータの登録
        //---------------------------------------------------------------------
        /**
         * @brief 操作対象のバックアップデータを登録する
         *
         * @param[in]     file_path  読み書きするファイルへのパス
         * @param[in,out] data       書き込む or 読み込んだデータの格納先
         * @param[in]     datasize   書き込む or 読み込むデータのサイズ
         * @retval true   登録成功
         * @retval false  登録失敗
         */
        bool AddBackupEntry( const wchar_t* file_path, void* data, u32 datasize );

        /**
         * @brief 登録したバックアップデータをクリアする
         */
        void ClearBackupEntry( void );


        //---------------------------------------------------------------------
        // ロード
        //---------------------------------------------------------------------
        /**
         * @brief バックアップファイルを読み込む
         *
         * @retval RESULT_OK       読み込み成功
         * @retval RESULT_NG_XXXX  読み込み失敗
         *
         * @attention 読み込みが完了するまで処理が返りません.
         */
        Result Load( void );

        /**
         * @brief バックアップファイルの非同期読み込みを開始する
         *
         * @param thread_priority  バックアップスレッドのプライオリティ
         * @retval RESULT_OK       読み込み開始
         * @retval RESULT_NG_XXXX  読み込み開始に失敗
         *
         * @note
         * この関数から RESULT_OK が返った際には,
         * WaitAsyncLoad() を RESULT_OK が返るまで繰り返し呼んでください.
         */
        Result StartAsyncLoad( u8 thread_priority );

        /**
         * @brief バックアップファイルの非同期読み込みの完了を待つ
         *
         * @retval RESULT_CONTINUE  読み込み中
         * @retval RESULT_OK        読み込み終了
         * @retval RESULT_NG_XXXX   読み込みに失敗
         */
        Result WaitAsyncLoad( void );



        //---------------------------------------------------------------------
        // セーブ
        //---------------------------------------------------------------------
        /**
         * @brief バックアップファイルに書き込む
         * @attention 書き込みが完了するまで処理が返りません.
         *
         * @retval RESULT_OK       書き込み成功
         * @retval RESULT_NG_XXXX  書き込み失敗
         */
        Result Save( void );

        /**
         * @brief バックアップファイルへの非同期書き込みを開始する
         *
         * @param thread_priority  バックアップスレッドのプライオリティ
         * @retval RESULT_OK       書き込み開始
         * @retval RESULT_NG_XXXX  書き込み開始に失敗
         */
        Result StartAsyncSave( u8 thread_priority );

        /**
         * @brief バックアップファイルへの非同期書き込みの完了を待つ
         *
         * @retval RESULT_CONTINUE  書き込み中
         * @retval RESULT_OK        書き込み開始
         * @retval RESULT_NG_XXXX   書き込み開始に失敗
         */
        Result WaitAsyncSave( void );


        //---------------------------------------------------------------------
        // その他
        //---------------------------------------------------------------------
        /**
         * @brief 空き容量を取得する
         * @param[out] free_bytes  空き容量の格納先
         * @retval RESULT_OK       成功。free_bytes に有効な値が格納されます。
         * @retval RESULT_NG_XXXX  失敗。free_bytes は有効な値ではありません。
         */
        Result GetFreeBytes( s64* free_bytes ) const;

        /**
         * @brief セーブ・ロード対象のファイル数を取得する
         */
        u32 GetBackupEntryCount( void ) const;

        /**
         * @brief セーブ・ロードが完了したファイルの数を取得する
         */
        u32 GetBackupFinishedCount( void ) const;

        Result MountSaveData( void );
        Result UnMountSaveData( void );
        Result CommitSaveData( void );

        nn::Result GetLastNNResult( void ) const { return m_last_result; }

        //----------------------------------------------------------------------------------------
        /**
         * @brief "パス + ファイル名" の文字列から, "パス" と "ファイル名" をそれぞれ抽出する.
         *
         * @param[in]  src_string      抽出対象の文字列
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
        //----------------------------------------------------------------------------------------
        bool GetPathAndFileName( 
            const wchar_t* src_string,
            wchar_t* dest_path,
            wchar_t* dest_file_name );

      private:

        static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1; // パス文字列の最大長( 終端文字を含む )
        static const u32 MAX_BACKUP_ENTRY_NUM = 128;   // 登録可能なバックアップ対象の数
        static const char* SAVEDATA_ARCHIVE_NAME;      // セーブデータアーカイブ名
        static const u32 STACK_SIZE_FOR_FORMAT_THREAD; // フォーマットスレッドのスタックサイズ
        static const u32 STACK_SIZE_FOR_LOAD_THREAD;   // 読み込みスレッドのスタックサイズ
        static const u32 STACK_SIZE_FOR_SAVE_THREAD;   // 書き込みスレッドのスタックサイズ
        static const u32 STACK_SIZE_FOR_CREATE_THREAD; // ファイル生成スレッドんおスタックサイズ
        static Result GetSystemResult( nn::Result nn_result );
        static Result GetSystemResult( FsCtrBackupFile::Result file_result );

        // バックアップ処理の操作対象
        struct BackupEntry {
          wchar_t file_path[ MAX_FILE_PATH_LENGTH ];
          void* data;
          u32 datasize;
        };

        gfl2::heap::HeapBase* m_heap;
        BackupEntry m_backup_entries[ MAX_BACKUP_ENTRY_NUM ];
        u32 m_backup_entry_count;
        u32 m_backup_finish_count;
        gfl2::fs::FsCtrBackupFile m_file;
        FsCtrBackupThread* m_backup_thread;

        nn::Result m_last_result;
        bool m_is_mount;

        bool IsBackupEntryFull( void ) const;
        Result CreateFile( const wchar_t* file_path, size_t file_size );
        Result DeleteFile( const wchar_t* file_path );
        Result WaitAsync( u32 wait_mode );
    };


  }  // namespace fs
}  // namespace gfl


#endif  // __GFL2_BACKUPSYSTEM_H__
#endif
