#ifndef __GFL_FILEBASE_H__
#define __GFL_FILEBASE_H__
//=======================================================================================
/**
 * @brief  ファイルアクセス基底クラス
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
     * @brief ファイルオープンモード
     */
    enum OpenMode {
      OPEN_MODE_READ,       // 読み込み用に開く
      OPEN_MODE_WRITE_NEW,  // 常に新規作成して書き込み用に開く(既存のファイルは削除されます)
      OPEN_MODE_WRITE_ADD,  // 追記用に開く(ファイルが存在しない場合は失敗します)
    };

    /**
     * @brief シークの起点指定
     */
    enum SeekBasePos {
      SEEK_BASE_BEGIN,    // ファイルの先頭
      SEEK_BASE_CURRENT,  // 現在位置
      SEEK_BASE_END,      // ファイルの末尾
    };


    class File
    {
      GFL_FORBID_COPY_AND_ASSIGN( File );

      public:
        //-------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ・デストラクタ
         */
        //-------------------------------------------------------------------------------
        File( void );
        virtual ~File( void );

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルを開く
         *
         * @param[in] path  ファイルパス
         * @param[in] mode  オープンモード
         *
         * @note ROM領域のファイルを開く場合は rom:/ で始まるパスを指定してください.
         *       SDカード上のファイルを開く場合は sdmc:/ で始まるパスを指定してください.
         *       PC上のファイルを開く場合は 絶対パスを指定してください.
         *
         * @retval true   オープンに成功した場合
         * @retval false  オープンに失敗した場合
         */
        //-------------------------------------------------------------------------------
        virtual bool Open( const wchar_t* path, OpenMode mode ) = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルを閉じる
         */
        //-------------------------------------------------------------------------------
        virtual void Close( void ) = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルから読み込む
         *
         * @param[out] buffer  読み込んだデータの格納先 (4バイトアラインメント)
         * @param[in]  size    読み込むサイズ
         *
         * @return 実際に読み込んだサイズ
         *
         * @note
         * デフォルトの動作として,「何もしない」処理が実装されています.
         * 派生クラスで読み込みが可能な場合のみ実装してください.
         */
        //-------------------------------------------------------------------------------
        virtual size_t Read( void* buffer, u32 size );

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルに書き込む
         *
         * @param[in] buffer  書き込むデータ (4バイトアラインメント)
         * @param[in] size    書き込むサイズ
         *
         * @return 実際に書き込んだサイズ
         *
         * @note
         * デフォルトの動作として,「何もしない」処理が実装されています.
         * 派生クラスで書き込みが可能な場合のみ実装してください.
         */
        //-------------------------------------------------------------------------------
        virtual size_t Write( const void* buffer, u32 size );

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルをシークする
         *
         * @param[in] base_pos  シークの起点となる位置
         * @param[in] offset    起点位置からのオフセット
         *
         * @note (起点位置 + オフセット) の位置に移動します。
         */
        //-------------------------------------------------------------------------------
        virtual void Seek( SeekBasePos base_pos, s32 offset ) = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルサイズを取得する
         */
        //-------------------------------------------------------------------------------
        virtual size_t GetSize( void ) const = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief 現在位置を取得する
         *
         * @return ファイル先頭からのオフセット[Byte]
         */
        //-------------------------------------------------------------------------------
        virtual u32 GetPosition( void ) const = 0;

        //----------------------------------------------------------------------------
        /**
         *	@brief  ファイル読み込み優先順位を設定する
         *
         *	@param	priority    優先順位
         *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// 利用注意！！！　ドキュメントを良く読むこと
         *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// 普通
         *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// 低い
         */
        //-----------------------------------------------------------------------------
        virtual bool SetPriority( s32 priority ) = 0;
        

        //-------------------------------------------------------------------------------
        /**
         * @brief 現在位置がファイルの末尾かどうかを調べる
         *
         * @retval true   ファイル末尾の場合
         * @retval false  ファイル末尾でない場合
         */
        //-------------------------------------------------------------------------------
        bool IsEOF( void ) const { return ( GetSize() <= GetPosition() ); }

        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルが開いているかどうかを調べる
         *
         * @retval true   ファイルを開いている
         * @retval false  ファイルを開いていない
         */
        //-------------------------------------------------------------------------------
        virtual bool IsOpen( void ) const = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief 読み込みが出来るかどうかを調べる
         *
         * @retval true   読み込み可能
         * @retval false  読み込み不可能
         */
        //-------------------------------------------------------------------------------
        virtual bool CanRead( void ) const = 0;

        //-------------------------------------------------------------------------------
        /**
         * @brief 書き込みが出来るかどうかを調べる
         *
         * @retval true   書き込み可能
         * @retval false  書き込み不可能
         */
        //-------------------------------------------------------------------------------
        virtual bool CanWrite( void ) const = 0;



        // 非同期処理

        /**
         * @brief 非同期処理が終了したときのコールバック関数
         * @param operated_data_size  実際に処理されたデータのサイズ
         * @param work                任意のワーク
         *
         * @note キャンセルした場合も呼ばれます.
         * @note operated_data_size を確認し, 正しく実行が完了したかどうかをチェックしてください.
         */
        typedef void (*CallBackFuncOnAsyncAccessFinished)( size_t operated_data_size, void* work );

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
         * @param[in]  callback_func         読み込み終了時のコールバック関数( NULLなら呼ばれない )
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
        virtual bool StartAsyncRead(
            gfl::heap::HeapBase* heap,
            const wchar_t* file_path, size_t file_offset,
            size_t total_data_size, size_t one_time_access_size, void* buffer,
            gfl::base::Thread::Priority thread_priority,
            CallBackFuncOnAsyncAccessFinished callback_func = NULL, void* callback_work = NULL );

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
         * @param[in]  callback_work         コールバック関数に渡す任意のワーク( NULLなら呼ばれない )
         * @return 非同期書き込みが開始できたかどうか
         * @retval true   開始できた
         * @retval false  開始できなかった
         *
         * @note
         * CheckAsyncAccess()で書き込みの終了を待った後,
         * EndAsyncAccess()を呼んでください。
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
         * @brief 非同期読み込み( 書き込み )が実行中かどうかを調べる
         * @retval true   実行中
         * @retval false  実行していない
         */
        //-------------------------------------------------------------------------------
        virtual bool CheckAsyncAccess( void ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief 実行中の非同期読み込み( 書き込み )をキャンセルする
         *
         * @note
         * CheckAsyncAccess()で非同期処理の終了を確認した後,
         * EndAsyncAccess()を呼んでください。
         */
        //-------------------------------------------------------------------------------
        virtual void CancelAsyncAccess( void );

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
        virtual void EndAsyncAccess( void );


#if 0   // Uncompが重いので使わない方向で。
        //====================================================================================
        //====================================================================================
        // 圧縮ファイル展開   2011/10/07 by nakahiro
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
        virtual bool StartAsyncRead(
            gfl::heap::HeapBase* heap,
            gfl::heap::HeapBase* buffer_heap,
            const wchar_t* file_path, size_t file_offset,
            size_t total_data_size, size_t one_time_access_size, void** buffer, u32 buffer_align,
            gfl::base::Thread::Priority thread_priority,
            CallBackFuncOnAsyncAccessFinished callback_func = NULL, void* callback_work = NULL );

        //-------------------------------------------------------------------------------
        /**
         * @brief 非同期読み込み( 書き込み )が実行中かどうかを調べる
         * @retval true   実行中
         * @retval false  実行していない
         */
        //-------------------------------------------------------------------------------
        virtual bool CheckCompAsyncAccess( void ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief 実行中の非同期読み込み( 書き込み )をキャンセルする
         *
         * @note
         * CheckCompAsyncAccess()で非同期処理の終了を確認した後,
         * EndCompAsyncAccess()を呼んでください。
         */
        //-------------------------------------------------------------------------------
        virtual void CancelCompAsyncAccess( void );

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
        virtual void EndCompAsyncAccess( void );
#endif


      protected:

        /**
         * @brief nn::fs::PositionBase に変換する
         */
        nn::fs::PositionBase GetPositionBase( SeekBasePos base_pos ) const;



      private:

        FileAccessThread* m_file_access_thread;       // 非同期アクセススレッド
#if 0   // Uncompが重いので使わない方向で。
        CompFileReadThread* m_comp_file_read_thread;  // 非同期アクセススレッド（圧縮データ展開用）
#endif

        /**
         * @brief 非同期処理スレッドを破棄する
         */
        void DeleteFileAccessThread( void );

        /**
         * @brief 非同期処理スレッドを破棄する
         */
        void DeleteCompFileReadThread( void );
    };


  } // namespace fs
} // namespace gfl
#endif // __GFL_FILEBASE_H__
