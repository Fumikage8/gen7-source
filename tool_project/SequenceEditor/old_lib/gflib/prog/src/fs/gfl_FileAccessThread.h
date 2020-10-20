#ifndef __GFL_FILE_ACCESS_THREAD_H__
#define __GFL_FILE_ACCESS_THREAD_H__
//========================================================================================
/**
 * @brief  ファイルへの読み書きを行うスレッド
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
         * @brief スレッドの動作を定義するパラメータ
         */
        struct OperationSetting {
          gfl::fs::File* file;          // ファイルアクセスを実行するオブジェクト
          gfl::fs::OpenMode open_mode;  // ファイルの扱い方( 読むのか, 書くのか )
          const wchar_t* file_path;     // アクセス対象ファイルのパス
          size_t file_offset;           // 読み込み先( 書き込み先 )のオフセット位置
          size_t total_data_size;       // トータルで読み込む( 書き込む )データサイズ
          size_t one_time_access_size;  // 1回のファイルアクセスで読み込むデータサイズ( 書き込む )データサイズ
          void* data_buffer;            // 読み込んだデータの格納先( 書き込むデータ )
          gfl::fs::File::CallBackFuncOnAsyncAccessFinished callback_func;  // アクセス終了時のコールバック関数( 必要ないなら NULL )
          void* callback_work;          // コールバック関数に渡す任意のワーク( 必要ないなら NULL )
        };

        //--------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param heap        スレッドのスタック領域に使用するヒープ
         * @param stack_size  スレッドのスタック領域のサイズ
         * @param setting     スレッドの動作を定義するパラメータ
         */
        //--------------------------------------------------------------------------------
        FileAccessThread( gfl::heap::HeapBase* heap, u32 stack_size, const OperationSetting& setting );

        //--------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //--------------------------------------------------------------------------------
        virtual ~FileAccessThread(); 

        //--------------------------------------------------------------------------------
        /**
         * @brief 実際に読み込んだ( 書き込んだ )サイズを取得する
         */
        //--------------------------------------------------------------------------------
        inline size_t GetOperatedSize( void ) const { return m_operated_size; }



      private:
        OperationSetting m_setting;
        size_t m_operated_size;  // 実際に読み込んだ( 書き込んだ )サイズ
        bool m_file_close_flag;  // 終了時にファイルを閉じるかどうか

        /**
         * @brief スレッド開始処理
         * @note gfl::base::Thread::Initialize()をオーバーライド
         */
        virtual void Initialize( void );

        /**
         * @brief スレッドメイン処理
         * @note ファイルアクセスが完了したら自殺する
         * @note gfl::base::Thread::Main()をオーバーライド
         */
        virtual void Main( void ); 

        /**
         * @brief 終了処理
         * @note gfl::base::Thread::Finalize()をオーバーライド
         */
        virtual void Finalize( void );

    }; 


#if 0		// Uncompが重いので使わない方向で。
		//====================================================================================
		//====================================================================================
		// 圧縮ファイル展開		2011/10/07 by nakahiro
		//====================================================================================
		//====================================================================================

    class CompFileReadThread : public gfl::base::Thread
    {
      public:

        /**
         * @brief スレッドの動作を定義するパラメータ
         */
        struct OperationSetting {
          gfl::fs::File* file;          // ファイルアクセスを実行するオブジェクト
					gfl::heap::HeapBase * heap;		// データ展開に使用するヒープ
          const wchar_t* file_path;     // アクセス対象ファイルのパス
          size_t file_offset;           // 読み込み先のオフセット位置
          size_t total_data_size;       // トータルで読み込む圧縮データサイズ
          size_t one_time_access_size;  // 1回のファイルアクセスで読み込む圧縮データサイズ
          void** data_buffer;           // 読み込んだデータの格納先
					u32	data_buffer_align;				// 展開データ格納場所のアライン
          gfl::fs::File::CallBackFuncOnAsyncAccessFinished callback_func;  // アクセス終了時のコールバック関数( 必要ないなら NULL )
          void* callback_work;          // コールバック関数に渡す任意のワーク( 必要ないなら NULL )
        };

        //--------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param heap        スレッドのスタック領域に使用するヒープ
         * @param stack_size  スレッドのスタック領域のサイズ
         * @param setting     スレッドの動作を定義するパラメータ
         */
        //--------------------------------------------------------------------------------
        CompFileReadThread( gfl::heap::HeapBase * heap, u32 stack_size, const OperationSetting& setting );

        //--------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //--------------------------------------------------------------------------------
        virtual ~CompFileReadThread(); 

        //--------------------------------------------------------------------------------
        /**
         * @brief 実際に読み込んだ( 書き込んだ )サイズを取得する
         */
        //--------------------------------------------------------------------------------
        inline size_t GetOperatedSize( void ) const { return m_operated_size; }

      private:
        OperationSetting m_setting;
        size_t m_operated_size;  // 実際に読み込んだ( 書き込んだ )サイズ
        bool m_file_close_flag;  // 終了時にファイルを閉じるかどうか

				bool comp_flag;								// 圧縮データをスレッド展開する場合のフラグ
				void * tmpBuff;								// 圧縮データ展開するためのバッファ
				u32	comp_sequence;						// 圧縮データ展開シーケンス
				nn::cx::UncompContextLZ	context;

        /**
         * @brief スレッド開始処理
         * @note gfl::base::Thread::Initialize()をオーバーライド
         */
        virtual void Initialize( void );

        /**
         * @brief スレッドメイン処理
         * @note ファイルアクセスが完了したら自殺する
         * @note gfl::base::Thread::Main()をオーバーライド
         */
        virtual void Main( void ); 

        /**
         * @brief 終了処理
         * @note gfl::base::Thread::Finalize()をオーバーライド
         */
        virtual void Finalize( void );
    }; 
#endif

  } // namespace fs
} // namespace gfl

#endif // __GFL_FILE_ACCESS_THREAD_H__ 
