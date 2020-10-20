//=============================================================================
/**
 * @brief  GFアーカイブファイルの読み込み
 * @file   gfl_ArcFile.h
 * @author obata_toshihiro
 * @date   2011.03.19
 */
//=============================================================================
#ifndef __ARCHIVE_READER_H__
#define __ARCHIVE_READER_H__ 
#pragma once

#include <fs/gfl_RomFile.h>

#if GFL_HIO_ENABLE
#include <fs/gfl_HioFile.h>
#endif


namespace gfl {
  namespace fs {

    class IArcFileAccessor;
    class SetupThread;


    class ArcFile
    { 
      GFL_FORBID_COPY_AND_ASSIGN( ArcFile );
      friend class SetupThread;

      public: 
      typedef u32 ARCID;    // アーカイブファイルを特定するためのID 
      typedef u32 ARCDATID; // アーカイブファイル内のデータを特定するためのID 
      static const u32 ARCID_NULL = 0xffffffff;


      //-----------------------------------------------------------------------
      // クラスの静的な設定
      //----------------------------------------------------------------------- 
      /**
       * @brief 言語コードを設定する
       * @param language  設定する言語コード
       */
      static void SetLanguage( u32 language );

      /**
       * @brief アーカイブファイルの名前テーブルを設定する
       * @param filename_table  アーカイブIDに対応するファイル名の配列
       * @param arcfile_num     ファイル名配列の要素数
       */
      static void SetFileNameTable( const wchar_t** filename_table, size_t arcfile_num );

      /**
       * @brief アーカイブIDに対応するファイル名を取得する
       * @param arcid  ファイル名を取得するアーカイブID
       * @return 対応するROMアーカイブ上のファイル名
       */
      static const wchar_t* GetRomFileName( ARCID arcid );
#if GFL_FILE_DEBUG
      //SDカード読み込み機能
      /**
       * @brief ArcをSDカードから読み込むためのファイル名テーブルの設定
       * @param filename_table  アーカイブIDに対応するファイル名の配列
       */
      static void SetSDMountFileNameTable( const wchar_t** filename_table );

      /**
       * @brief ArcをSDカードから読み込むかのフラグ(ファイルが存在しなければ通常の読み込みをします)
       * @param bool 有効・無効
       */
      static void SetSDMountIsEnable( bool flg );

      /**
       * @brief ArcをSDカードから読み込むかのフラグの取得
       * @return bool 有効・無効
       */
      static bool GetSDMountIsEnable( void );

      /**
       * @brief ArcをSDカードから読み込む時に対応するファイル名を取得する
       * @param arcid  ファイル名を取得するアーカイブID
       * @return 対応するアーカイブ上のファイル名
       */
      static const wchar_t* GetSDMountFileName( ARCID arcid );
      
#endif

#if GFL_HIO_ENABLE
      /**
       * @brief HIOアクセスするためのファイル名テーブルを設定する
       * @param filename_table  アーカイブIDに対応するファイル名の配列
       */
      static void SetHioFileNameTable( const wchar_t** filename_table );

      /**
       * @brief アーカイブIDに対応するファイル名を取得する
       * @param arcid  ファイル名を取得するアーカイブID
       * @return 対応するホストファイルシステム上のファイル名
       */
      static const wchar_t* GetHioFileName( ARCID arcid );
#endif



      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //----------------------------------------------------------------------- 
      /**
       * @brief 初期化パラメータ
       */
      enum InitBit 
      {
       // ◆ファイルを開く
       // ファイルをカレントスレッドで開きます.
       // コンストラクタから返った時点でファイルが開かれています.
       // ファイルを別スレッドで開きたい場合, 
       // OPEN を指定せずに StartSetupForFastMode() を呼んでください.
        OPEN = 1 << 0,

       // ◆HIO を使用する
       // PC 上にあるファイルを HIO で読み込みます.
       // ARCID でファイルを指定した場合にのみ有効です.
        HIO  = 1 << 1,

       // ◆ファイルを開き、高速動作モードにします。
       // SetupForFastModeをコンストラクタ内で行います。
       // ファイルを別スレッドで開きたい場合, 
       // OPEN_FASTMODE を指定せずに StartSetupForFastMode() を呼んでください.
        OPEN_FASTMODE  = (1 << 2) | OPEN,
      };

      /**
       * @brief コンストラクタ
       *
       * @param heap       作業ワークに使用するヒープ
       * @param arcID      関連付けるアーカイブファイル
       * @param init_flag  コンストラクタで行う処理を InitBit の論理和で指定
       */
      ArcFile( gfl::heap::HeapBase* heap, ARCID arcID, u32 init_flag );

      /**
       * @brief コンストラクタ
       *
       * @param heap       作業ワークに使用するヒープ
       * @param filename   関連付けるアーカイブファイル
       * @param init_flag  コンストラクタで行う処理を InitBit の論理和で指定
       */
      ArcFile( gfl::heap::HeapBase* heap, const wchar_t* filename, u32 init_flag );

      /**
       * @brief デストラクタ
       */
      virtual ~ArcFile( void ); 



      //----------------------------------------------------------------------- 
      // ファイルオープン
      // コンストラクタで OPEN を指定している場合は必要ありません.
      //----------------------------------------------------------------------- 
      /**
       * @brief ファイルを開く
       */
      void Open( void );

      /**
       * @brief ファイルを開いているかどうかを調べる
       * @retval true
       * @retval false
       */
      bool IsOpen( void ) const;

      /**
       * @brief ファイルが開いた時点でシグナル状態になるイベントオブジェクトを取得する
       * @note 一度シグナル状態になったら、以降リセットはされません。
       * @attention 取得したイベントオブジェクトの状態を変更しないでください。
       */
      nn::os::LightEvent* GetOpenEvent( void );


      //-----------------------------------------------------------------------
      // 高速モードへの移行
      //-----------------------------------------------------------------------
      /**
       * @brief 高速モードへの移行を開始する
       * @param heap_for_thread_work       スレッドの作業領域として使用するヒープ
       * @param heap_for_fastaccess_table  高速アクセス用データの確保に使用するヒープ
       *
       * @note WaitSetupForFastMode()で移行の完了を待ってください.
       */
      void StartSetupForFastMode( 
          gfl::heap::HeapBase* heap_for_thread_work,
          gfl::heap::HeapBase* heap_for_fastaccess_table );

      /**
       * @breif 高速モードへの移行完了を待つ
       * @retval true   完了した
       * @retval false  未完了
       *
       * @attention
       * true が返るまで繰り返し呼び, 移行の完了を待ってください.
       */
      bool WaitSetupForFastMode( void ); 

      /**
       * @brief 高速モードかどうかを調べる
       * @retval true   高速モード
       * @retval false  通常モード
       */
      bool IsFastMode( void ) const;

      /**
       * @brief 高速モードへの移行が完了した時点でシグナル状態になるイベントオブジェクトを取得する
       * @note 一度シグナル状態になったら、以降リセットはされません。
       * @attention 取得したイベントオブジェクトの状態を変更しないでください。
       */
      nn::os::LightEvent* GetFastSetupEvent( void );

      //=======================================================================
      /**
       * @brief 高速モードへ移行する( 同期処理 )
       * @param heap  高速アクセスするためのデータ確保に使用するヒープ
       */
      //=======================================================================
      void SetupForFastMode( gfl::heap::HeapBase* heap ); 



      //-----------------------------------------------------------------------
      // アーカイブファイルについての情報取得
      //-----------------------------------------------------------------------
      /**
       * @brief アーカイブに含まれるデータの数を取得する
       *
       * @note 通常モードの場合
       * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
       */
      u16 GetDataCount( void );

      /**
       * @brief データ1件分の( パディングを含む )サイズを取得する
       * @param data_id  アーカイブファイル内データID
       *
       * @note 通常モードの場合
       * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
       */
      size_t GetDataSize( ARCDATID data_id ); 

      /**
       * @brief データ1件分の( パディングを含まない )サイズを取得する
       * @param data_id  アーカイブファイル内データID
       *
       * @note 通常モードの場合
       * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
       *
       * @note データを読み込むために必要なバッファサイズは, パディングを含んだサイズです。
       * データを読み込むためのデータサイズは GetDataSize() で取得してください。
       *
       * @attention
       * このメソッドは, ver.4.0 以降のアーカイブファイルでのみ有効となります。
       * ver.3.x までのアーカイブファイルではパディングを含むデータサイズが返ります。
       */
      size_t GetRealDataSize( ARCDATID data_id ); 

      /**
       * @brief アーカイブに含まれるデータのうち, 最も大きなデータのサイズを取得する
       *
       * @note 
       * 通常モードの場合, 
       * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
       *
       * @attention
       * このメソッドは, ver.3.0 以降のアーカイブファイルでのみ有効となります。
       * ver.2.x までのアーカイブファイルでは必ず0が返ります。
       */
      size_t GetMaxDataSize( void );



      //-----------------------------------------------------------------------
      // 同期読み込み
      //-----------------------------------------------------------------------
      /**
       * @brief データを1件分読み込む
       * @param[in]  data_id      アーカイブファイル内データID
       * @param[out] dest_buffer  読み込んだデータの格納先( 4 byte alignment )
       * @retval 実際に読み込んだバイト数
       */
      size_t LoadData( ARCDATID data_id, void* dest_buffer );

      /**
       * @brief データ1件の一部を読み込む
       * @param[in]  data_id           アーカイブファイル内データID
       * @param[in]  offset_from_data  読み込み開始オフセット(byte, データ先頭からのオフセット)
       * @param[in]  read_size         読み込みサイズ(byte)
       * @param[out] dest_buffer       読み込んだデータの格納先( 4 byte alignment )
       */
      void LoadDataPiece( 
          ARCDATID data_id, u32 offset_from_data, u32 read_size, void* dest_buffer ); 

      /**
       * @brief データ1件の読み込みポインタをシークする( 取り扱い注意！)
       * @param[in] data_id            アーカイブファイル内データID
       * @param[in] offset_from_data   シークサイズ( byte, データ先頭からのオフセット)
       */
      void SeekDataPtr( ARCDATID data_id, u32 offset_from_data );

      /**
       * @brief SeekDataPtr()でシークしたポインタからデータ読み込む( 取り扱い注意！)
       * @param[in]  size         読み込みサイズ
       * @param[out] dest_buffer  読み込んだデータの格納先( 4 byte alignment )
       * @retval 実際に読み込んだバイト数
       */
      size_t LoadDataPtr( u32 size, void* dest_buffer );

			/**
			 * @brief	圧縮データを1件分展開して読み込む
       * @param[in]		  data_id	          アーカイブファイル内データID
       * @param[in]		  heap		          解凍作業用ヒープ
       * @param[in,out]	buffer	          解凍データの展開先バッファ
       * @param[in]	    buffer_size       解凍データの展開先バッファのサイズ
       * @param[out]	  uncompressed_size 展開データのサイズ
			 */
			void LoadCompressedData( 
          ARCDATID data_id, 
          gfl::heap::HeapBase* heap,
          void* buffer,
          size_t buffer_size,
          size_t* uncompressed_size = NULL ); 

      /**
       * @brief	圧縮データを1件分展開して読み込む
       * @param[in]		datID			  アーカイブファイル内データID
       * @param[in]		heapForWork	解凍作業に使用するヒープ
       * @param[in]		heapForData	解凍データを確保するヒープ
       * @param[in]		align				展開データのアライン
       * @param[out]	pSize				展開データのサイズ
       * @return	展開したデータ
       */
      void * LoadCompressedDataAlloc( 
          ARCDATID datID, 
          gfl::heap::HeapBase * heapForWork,
          gfl::heap::HeapBase * heapForData,
          int align,
          size_t * pSize );

			/**
			 * @brief	圧縮データを1件分展開して読み込む
       *
       * @attention この関数は使用しないでください。
			 */
			void LoadData( 
          ARCDATID data_id, 
          gfl::heap::HeapBase* heap,
          void* buffer,
          size_t buffer_size,
          size_t* uncompressed_size = NULL );

      /**
       * @brief	圧縮データを1件分展開して読み込む
       *
       * @attention この関数は使用しないでください。
       */
      void * LoadData( 
          ARCDATID datID,
          gfl::heap::HeapBase * heap,
          int align = 4,
          size_t * pSize = NULL );



      //-----------------------------------------------------------------------
      // 非同期読み込み
      //-----------------------------------------------------------------------
      /**
       * @brief 非同期読み込みを開始する
       *
       * @param[in]  heap             スレッドの生成に使用するヒープ
       * @param[in]  data_id          アーカイブファイル内データID
       * @param[out] buffer           読み込んだデータの格納先( 4 byte alignment )
       * @param[in]  thread_priority  スレッドの優先度( 高 0 ～ 31 低 )
       * @param[in]  callback_func    読み込み終了時のコールバック関数( NULLなら呼ばれない )
       * @param[in]  callback_work    コールバック関数に渡す任意のワーク( 不要ならNULL )
       *
       * @return 非同期読み込みが開始できたかどうか
       * @retval true   開始できた
       * @retval false  開始できなかった
       *
       * @attention 
       * ArcFileインスタンス1つにつき, 同時に2つ以上の非同期読み込みを実行することは出来ません.
       * すでに実行中の非同期読み込みがある場合, この関数は失敗します( false が返ります ).
       *
       * @note
       * WaitAsyncRead()で読み込みの完了を待ってください.
       *
       * @note コールバックを関数ではなくリスナクラスを渡すようにしたい
       */
      bool StartAsyncRead( 
          gfl::heap::HeapBase* heap, 
          ARCDATID data_id,
          void* buffer, 
          gfl::base::Thread::Priority thread_priority,
          File::CallBackFuncOnAsyncAccessFinished callback_func = NULL, 
          void* callback_work = NULL );

      /**
       * @brief 非同期読み込みを開始する( オフセット指定ver. )
       *
       * @param[in]  heap             スレッドの生成に使用するヒープ
       * @param[in]  data_id          アーカイブファイル内データID
       * @param[in]  read_offset      指定したデータの先頭からのオフセット
       * @param[in]  read_size        読み込むデータサイズ
       * @param[out] buffer           読み込んだデータの格納先( 4 byte alignment )
       * @param[in]  thread_priority  スレッドの優先度( 高 0 ～ 31 低 )
       * @param[in]  callback_func    読み込み終了時のコールバック関数( NULLなら呼ばれない )
       * @param[in]  callback_work    コールバック関数に渡す任意のワーク( 不要ならNULL )
       *
       * @return 非同期読み込みが開始できたかどうか
       * @retval true   開始できた
       * @retval false  開始できなかった
       *
       * @attention 
       * ArcFileインスタンス1つにつき, 同時に2つ以上の非同期読み込みを実行することは出来ません.
       * すでに実行中の非同期読み込みがある場合, この関数は失敗します( false が返ります ).
       *
       * @note
       * WaitAsyncRead()で読み込みの完了を待ってください.
       *
       * @note コールバックを関数ではなくリスナクラスを渡すようにしたい
       */
      bool StartAsyncRead( 
          gfl::heap::HeapBase* heap, 
          ARCDATID data_id,
          u32 read_offset,
          u32 read_size,
          void* buffer, 
          gfl::base::Thread::Priority thread_priority,
          File::CallBackFuncOnAsyncAccessFinished callback_func = NULL, 
          void* callback_work = NULL );

      /**
       * @brief 非同期読み込みの完了を待つ
       * @retval true   未完了
       * @retval false  完了
       *
       * @attention
       * false が返るまで繰り返し呼び, 移行の完了を待ってください.
       */
      bool WaitAsyncRead( void ); 

      /**
       * @brief 実行中の非同期読み込みをキャンセルする
       *
       * @note
       * WaitAsyncRead()で読み込みの完了を待ってください.
       */
      void CancelAsyncRead( void );


      //-----------------------------------------------------------------------
      // HIO
      //-----------------------------------------------------------------------
      /**
       * @brief HIO による読み込みか？
       * @retval true   HIO 読み込み
       * @retval false  HIO 読み込みでない
       */
      bool IsHioFile( void ) const;


      //-----------------------------------------------------------------------
      // アーカイブ内のデータにアクセスするための情報
      //-----------------------------------------------------------------------
      /**
       * @brief アーカイブ内のデータにアクセスするための情報
       */
      struct AccessInfo 
      {
        u32 dataSize;             // データサイズ( パディングを含む )
        u32 realSize;             // データサイズ( パディングを含まない )
        u32 offsetFromArchiveTop; // アーカイブファイル先頭からのオフセット
      };
      /**
       * @brief アーカイブ内データへのアクセス情報を適切な場所から取得する
       * @param[in]  data_id      ファイル内データID
       * @param[out] access_info  取得したアクセス情報の格納先
       *
       * @note 通常モードの場合
       * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
       */
      void GetAccessInfo( ARCDATID data_id, AccessInfo* access_info );



      private:
      static const u32 STACK_SIZE_OF_SETUP_THREAD_FOR_FAST_MODE;
      static u32 LANGUAGE_CODE;  // 動作言語コード
      static size_t ARCFILE_NUM;
      static const wchar_t** FILENAME_TABLE;
      static const wchar_t* GetFileName( ARCID , const wchar_t**, u32 );

      gfl::heap::HeapBase* m_workHeap;     // 作業ワーク確保用のヒープ
      gfl::fs::RomFile m_file;             // ファイル操作するオブジェクト
      const wchar_t* m_filename;           // 対象アーカイブのファイル名
      u16 m_dataNum;                       // アーカイブ内のデータ数
      u32 m_maxDataSize;                   
      AccessInfo* m_fastAccessTable;       // 高速アクセスのための索引テーブル
      SetupThread* m_setupThread;          // 高速モードへ移行するためのスレッド
      nn::os::LightEvent m_fileOpenEvent;  // オープン時にシグナル状態になるイベント
      nn::os::LightEvent m_fastSetupEvent; // 高速モードへの移行が完了した時にシグナル状態になるイベント
      nn::os::CriticalSection m_criticalSectionForFileOpen;
      nn::os::CriticalSection m_criticalSectionForFileRead;
      nn::os::CriticalSection m_criticalSectionForFastSetup;

#if GFL_FILE_DEBUG
      static const wchar_t** SD_FILENAME_TABLE;
      static bool m_isEnableSd;
#endif

#if GFL_HIO_ENABLE
      static const wchar_t** HIO_FILENAME_TABLE;
      bool m_isHioFile;
#endif

#if GFL_DEBUG
      u8 m_majorVersion;
      u8 m_minorVersion;
#endif



      void InitExclusiveObjects( void );

      bool OpenFile( const wchar_t* );
      void SeekFile( size_t ); 
      void ExclusiveRead( u32 readPos, u32 readSize, void* pDestBuffer, u32* pReadSize = NULL );
      
      bool IsSetupForFastModeFinished( void ) const; 
      bool IsSettingUpForFastMode( void ) const; 
      void EndSetupForFastMode( void ); 

      bool CheckAsyncRead( void ) const;
      void EndAsyncRead( void );
      size_t ReadTotalBlockSize( void );

      IArcFileAccessor* CreateArcFileAccessor( gfl::heap::HeapBase* );
      //void GetAccessInfoFromFile_Slow( ARCDATID, AccessInfo* );
      void GetAccessInfoFromFile_Fast( ARCDATID, AccessInfo*, size_t, void* );
      void GetAccessInfoFromTable( ARCDATID, AccessInfo* );
      AccessInfo* CreateDataAccessTable( gfl::heap::HeapBase*, const IArcFileAccessor* ) const;

      u16 GetFileCountFromFile( void ); 
      void Uncompress( void*, size_t, void*, size_t, size_t* );
    };


  }  //namespace fs
}  //namespace gfl 

#endif  //__ARCHIVE_READER_H__
