//=============================================================================
/**
 * @brief  ファイル読み込み管理
 * @file   gfl_FileReadManager.h
 * @author obata_toshihiro
 * @date   2012.01.08
 */
//=============================================================================
#ifndef __GFL_FILEREADMANAGER_H__
#define __GFL_FILEREADMANAGER_H__
#pragma once


#include <base/gfl_ThreadManager.h>
#include <base/gfl_Queue.h>
#include <fs/gfl_ArcFile.h>

namespace gfl {
  namespace fs {

    class Thread;


    /**
     * @note 読み込み手順
     * 1. 読み込みたいアーカイブを AppendManageFile() で登録する.
     * 2. AddReadRequest() で読み込みリクエストを発行する.
     * 3. IsReadFinished(), IsAllRequestFinished() で読み込みの完了を待つ.
     * 4. 登録したアーカイブが必要なくなったら, RemoveManageFile() で登録を解除する.
     */
    class FileReadManager : 
      public base::ThreadManager,
      public base::IThreadManageCallback
    {
      GFL_FORBID_COPY_AND_ASSIGN( FileReadManager );

      public: 

      /**
       * @brief リクエストの実行優先度
       * @note 値が小さい方が優先度が高い
       */
      static const u8 REQUEST_PRIORITY_HIGHEST =  0;
      static const u8 REQUEST_PRIORITY_NORMAL  = 16;
      static const u8 REQUEST_PRIORITY_LOWEST  = 31;


      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap_for_management  インスタンスを確保するヒープ
       * @param arcfile_count        管理するアーカイブファイルの数
       * @param max_request_count    同時に受け付けることができる読み込みリクエストの数
       * @param max_read_count       同時に非同期読み込みを実行する数
       * @param read_thread_priority 読み込みスレッドの優先順位
       */
      //-----------------------------------------------------------------------
      FileReadManager( 
          gfl::heap::HeapBase* heap_for_management,
          u32 arcfile_count,
          u32 max_request_count,
          u8 max_read_count,
          base::Thread::Priority read_thread_priority );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~FileReadManager();

      //-----------------------------------------------------------------------
      /**
       * @brief 更新処理
       */
      //-----------------------------------------------------------------------
      void UpdateFileRead( void );

      //-----------------------------------------------------------------------
      /**
       * @brief アーカイブファイルが登録済みか？
       * @param arcID チェックするアーカイブファイルのID
       * @retval true  登録済み
       * @retval false 登録されていない
       */
      //-----------------------------------------------------------------------
      bool IsFileManaged( ArcFile::ARCID arcID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アーカイブファイルを登録する
       * @param heapForArc  ArcFileを置くヒープ
       * @param arcID       登録するアーカイブファイルのID
       * @param hio         true:HIOで読む, false:ROMから読む
       *
       * @attention
       * heapForArc に確保されるメモリは 
       * RemoveManageFile() を呼ぶことで解放されます。
       * ヒープの断片化に注意してください。
       */
      //-----------------------------------------------------------------------
      void AppendManageFile( 
          gfl::heap::HeapBase* heapForArc,
          ArcFile::ARCID arcID,
          bool hio = false );

      //-----------------------------------------------------------------------
      /**
       * @brief アーカイブファイルを管理対象から除外する
       * @param arcID 登録を解除するアーカイブファイルのID
       *
       * @attention
       * arcID への読み込みが終了していない場合, カレントスレッドを休止させます。
       */
      //-----------------------------------------------------------------------
      void RemoveManageFile( ArcFile::ARCID arcID ); 

      //-----------------------------------------------------------------------
      /**
       * @brief 登録されているアーカイブファイルを取得する
       * @param arcID  取得対象のアーカイブファイルID
       * @retval NULL  指定したアーカイブファイルが登録されていない場合
       */
      //-----------------------------------------------------------------------
      gfl::fs::ArcFile* GetManageFile( ArcFile::ARCID arcID ) const;

      //-------------------------------------------------------------------------
      /**
       * @brief 指定アーカイブをFastMode=高速化モードに移行する(同期読み込み版)
       *
       * @param  arcID  FastModeに移行させるアーカイブID
       * @param  heapForFastTable  高速化テーブルを確保するヒープ(NULL指定ならAppendManageFileに渡したヒープから確保)
       *
       * @retval true   FastModeに移行できた
       * @retval false  FastModeに移行出来なかった(未登録,HITオプションが有効、既にFastModeなど)
       *
       * @note
       * AppendManageFile()でアーカイブを登録した段階では、ファイルの高速化テーブルはメモリ上にロードされていない。
       *
       * AppendManageFile()と最初のAddReadRequest()の発行位置が離れていると、
       * 高速化テーブルがメモリに乗るタイミングがずれるため、ヒープの断片化要因になりえる。
       *
       * このような場合はAppendManageFile()直後に、明示的にFastModeへの移行をしておくことで断片化を防ぐ
       */
      //-------------------------------------------------------------------------
      bool SetupForFastMode( ArcFile::ARCID arcID, gfl::heap::HeapBase* heapForFastTable = NULL );
    
      //-------------------------------------------------------------------------
      /**
       * @brief 指定アーカイブのFastMode移行を開始する(非同期読み込み版)
       *
       * @param  arcID  FastModeに移行させるアーカイブID
       * @retval true  FastMode移行開始
       * @retval false FastMode開始出来なかった(未登録,HITオプションが有効、既にFastModeなど)
       *
       * 直後に必ずWaitSetupForFastModeで待つこと。
       *
       * 呼び出すArcFile::StartSetupForFastMode()が
       * 高速化モード移行に内部でスレッドを用いるため、
       * StartSetupForFastMode()呼び出し直後にWaitSetupForFastMode()で待たないと
       * 高速化テーブルが乗るアドレスが定まらない可能性がある。
       *
       * StartSetupForFastModeがfalseを返した場合に、
       * WaitSetupForFastMode()で待っても速攻でtrue(=終了)が返り、安全に使えるようにしてあります。
       *
       * ただし、指定したアーカイブが未登録の場合、GFL_ASSERT()は発生します。
       *
       * @note
       * AppendManageFile()でアーカイブを登録した段階では、ファイルの高速化テーブルはメモリ上にロードされていない。
       *
       * AppendManageFile()と最初のAddReadRequest()の発行位置が離れていると、
       * 高速化テーブルがメモリに乗るタイミングがずれるため、ヒープの断片化要因になりえる。
       *
       * このような場合はAppendManageFile()直後に、明示的にFastModeへの移行をしておくことで断片化を防ぐ
       *
       * 一応、ArcFileクラスの内部挙動として、高速化モード移行スレッドが生きている間に
       * ArcFileクラスが破棄された場合、スレッドはArcFileクラスのデストラクタに殺されるが
       * 基本的な使い方としてはリクエストを出した後はWaitで待つべきである。
       */
      //-------------------------------------------------------------------------
       bool StartSetupForFastMode( ArcFile::ARCID arcID,
                                   gfl::heap::HeapBase* heapForThread = NULL, 
                                   gfl::heap::HeapBase* heapForFastTable = NULL );
  
      //-------------------------------------------------------------------------
      /**
       * @brief 指定アーカイブのFastMode移行を待つ(非同期読み込み版)
       *
       * @retval true  移行完了
       * @retval false まだ移行中
       *
       * StartSetupForFastMode()で開始し、WaitSetupForFastModeで待つ
       * true が返るまで繰り返し呼び, 移行の完了を待ってください.
       *
       * 指定したアーカイブが未登録の場合などは、アサート抜けでtrueが返ります
       *
       * @note
       * AppendManageFile()でアーカイブを登録した段階では、ファイルの高速化テーブルはメモリ上にロードされていない。
       *
       * AppendManageFile()と最初のAddReadRequest()の発行位置が離れていると、
       * 高速化テーブルがメモリに乗るタイミングがずれるため、ヒープの断片化要因になりえる。
       *
       * このような場合はAppendManageFile()直後に、明示的にFastModeへの移行をしておくことで断片化を防ぐ
       *
       * 非同期版のこちらは、呼び出すArcFile::StartSetupForFastMode()が
       * 高速化モード移行に内部でスレッドを用いるため、
       * StartSetupForFastMode()呼び出し直後にWaitSetupForFastMode()で待たないと
       * 高速化テーブルが乗るアドレスが定まらない可能性があるので注意。
       *
       * 一応、ArcFileクラスの内部挙動として、高速化モード移行スレッドが生きている間に
       * ArcFileクラスが破棄された場合、スレッドはArcFileクラスのデストラクタに殺されるが
       * 基本的な使い方としてはリクエストを出した後はWaitで待つべきである。
       */
      //-------------------------------------------------------------------------
      bool WaitSetupForFastMode( ArcFile::ARCID arcID );

      /**
       * @brief 読み込みリクエスト
       */
      struct ReadRequest
      {
        u8 requestPriority;                  // [in] リクエストの実行優先度( 小さい方が優先度が高い )
        ArcFile::ARCID arcID;                // [in] 読み込むデータのアーカイブのID
        ArcFile::ARCDATID datID;             // [in] 読み込むデータのアーカイブ内データID
        bool compressed;                     // [in] 読み込むデータが圧縮されているか？( true:圧縮されている, false:圧縮されていない )
        gfl::heap::HeapBase* heapForWork;    // [in] スレッドのインスタンス確保ヒープ ※後方確保推奨！
        gfl::heap::HeapBase* heapForUncomp;  // [in] 圧縮データの解凍作業に使用するヒープ
                                             //      compressed が true の場合にのみ使用します。
                                             //      NULL なら heapForBuffer を使用します。
        gfl::heap::HeapBase* heapForBuffer;  // [in]読み込み先バッファを確保するヒープ
        u32 destBufferAlignment;             // [in] 読み込み先バッファのアラインメント
        void** ppDestBuffer;                 // [out] 読み込んだデータへのポインタをセットするポインタ変数のアドレス
        u32* pDestBufferSize;                // [out] 読み込んだデータサイズの格納先( 不要ならNULL )

        ReadRequest() :
          requestPriority( REQUEST_PRIORITY_NORMAL ),
          arcID( 0 ),
          datID( 0 ),
          compressed( false ),
          heapForWork( NULL ),
          heapForUncomp( NULL ),
          heapForBuffer( NULL ),
          destBufferAlignment( 0 ),
          ppDestBuffer( NULL ),
          pDestBufferSize( NULL )
        { }
      };

      //-----------------------------------------------------------------------
      /**
       * @brief 読み込みリクエストを登録する
       * @param req  リクエストデータ
       * @retval true  リクエストを登録できた
       * @retval false リクエストを登録できなかった
       *
       * @note
       * リクエストが実行されると,
       * 対象のデータサイズのバッファが heapForBuffer から確保され, データを読み込みます.
       * *ppDestBuffer に確保したバッファへのポインタをセットします.
       * *pDestBufferSize に読み込んだサイズをセットします.
       */
      //-----------------------------------------------------------------------
      bool AddReadRequest( const ReadRequest& req );


			// ↓ 2012.7.4.追加 tetsu
      /**
       * @brief 読み込みリクエスト(外部指定先に読み込み)
       */
      struct ReadRequestStatic
      {
        u8 requestPriority;										// [in] リクエストの実行優先度( 小さい方が優先度が高い )
        ArcFile::ARCID arcID;									// [in] 読み込むデータのアーカイブのID
        ArcFile::ARCDATID datID;							// [in] 読み込むデータのアーカイブ内データID
        bool compressed;											// [in] 読み込むデータが圧縮されているか？( true:圧縮されている, false:圧縮されていない )
        gfl::heap::HeapBase* heapForWork;     // [in] スレッドのインスタンス確保ヒープ ※後方確保推奨！
        gfl::heap::HeapBase* heapForUncomp;		// [in] 圧縮データの解凍作業に使用するヒープ
																							//      compressed が true の場合にのみ使用します。
        void* pDestBuffer;										// [in] 読み込み先ポインタ
        u32		destBufferSize;                 // [in] 読読み込み先データサイズ
				u32* pReadSize;                       // [out] 実際に読み込んだデータサイズ 2012/07/26 by nakahiro

        ReadRequestStatic() :
          requestPriority( REQUEST_PRIORITY_NORMAL ),
          arcID( 0 ),
          datID( 0 ),
          compressed( false ),
          heapForWork( NULL ),
          heapForUncomp( NULL ),
          pDestBuffer( NULL ),
          destBufferSize( 0 ),
          pReadSize( NULL )
        { }
      };

      //-----------------------------------------------------------------------
      /**
       * @brief 読み込みリクエストを登録する(外部指定先に読み込み)
       * @param req  リクエストデータ
       * @retval true  リクエストを登録できた
       * @retval false リクエストを登録できなかった
			 * 
			 * リクエストIDは読み込み先バッファと等しい ID = (u32)pDestBuffer
       */
      //-----------------------------------------------------------------------
      bool AddReadRequestStatic( const ReadRequestStatic& req );

			//-----------------------------------------------------------------------
			/**
			 * @brief IDを直接指定して読み込みリクエストをキャンセルする
			 * @retval true   キャンセルしました
			 * @retval false  キャンセルできませんでした( すでに読み込み中 )
       *
       * @note 
       * 既に読み込み中のリクエストはキャンセルできません。
       * IsReadFinished() で, 読み込みの終了を待ってください。
       */
      //-----------------------------------------------------------------------
			bool CancelReadRequest( u32 requestID );

			//-------------------------------------------------------------------------
      /**
       * @brief IDを直接指定して指定したデータの読み込みが完了したかを確認
       * @retval true  完了した
       * @retval false 完了していない
       */
      //-------------------------------------------------------------------------
			bool IsReadFinished( u32 requestID ) const;

			// ↑2012.7.4.追加 tetsu

      //-----------------------------------------------------------------------
      /**
       * @brief 読み込みリクエストをキャンセルする
       * @param[in] ppDestBuffer 読み込んだデータへのポインタをセットするポインタ変数のアドレス
       * @retval true   キャンセルしました
       * @retval false  キャンセルできませんでした( すでに読み込み中 )
       *
       * @note 
       * 既に読み込み中のリクエストはキャンセルできません。
       * IsReadFinished() で, 読み込みの終了を待ってください。
       */
      //-----------------------------------------------------------------------
      bool CancelReadRequest( void** ppDestBuffer );

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したデータの読み込みが完了したか？
       * @param[in] ppDestBuffer 読み込んだデータへのポインタをセットするポインタ変数のアドレス
       * @retval true  完了した
       * @retval false 完了していない
       */
      //-----------------------------------------------------------------------
      bool IsReadFinished( void** ppDestBuffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 全ての読み込みが完了したか？
       * @retval true   完了した
       * @retval falsek 完了していない
       */
      //-----------------------------------------------------------------------
      bool IsAllReadFinished( void ) const;






      /**
       * @brief 読み込みリクエストを登録する
       *
       * @attention この関数は使用しないでください。
       */
      NW_DEPRECATED_FUNCTION( bool AddReadRequest( 
          void** ppDestBuffer,
          u32* pDestBufferSize,
          gfl::heap::HeapBase* heapForBuffer,
          u32 destBufferAlignment,
          ArcFile::ARCID arcID,
          ArcFile::ARCDATID datID,
          bool compressed = false,
          u8 requestPriority = REQUEST_PRIORITY_NORMAL ) );






      private:

      // ファイル管理情報
      struct FileManageData
      {
        ArcFile::ARCID arcID;
        ArcFile* arcFile;
        gfl::heap::HeapBase* heapForArcFile;  // アーカイブファイル自体を置くヒープ
        u32 requestCount;                     // アーカイブファイルに対するリクエストカウンタ
      };

      gfl::heap::HeapBase* m_pWorkHeap;
      base::Thread::Priority m_readThreadPriority;
      FileManageData* m_manageData;
      u32 m_maxManageDataCount;

      void InitManageData( FileManageData* );
      void DeleteManageData( FileManageData* );
      FileManageData* GetManageData( ArcFile::ARCID ) const;
      FileManageData* GetEmptyManageData( void ) const;
      u32 CalcRequestID( void** ) const;
      void FinalizeRequest( const base::ThreadManager::Request& );

      /**
       * @brief リクエスト終了時のコールバック
       * @param finished_request  終了したリクエスト
       */
      virtual void OnRequestFinished( const base::ThreadManager::Request& finished_request );

      /**
       * @brief リクエストが実行されずに終了した際のコールバック
       * @param canceled_request  実行されなかったリクエスト
       */
      virtual void OnRequestCanceled( const base::ThreadManager::Request& canceled_request );
    };


  }  //namespace fs
}  //namespace gfl

#endif  //__GFL_FILEREADMANAGER_H__
