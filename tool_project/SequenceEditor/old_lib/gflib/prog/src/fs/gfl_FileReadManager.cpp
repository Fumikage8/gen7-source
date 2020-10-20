//=============================================================================
/**
 * @brief ファイル読み込み管理
 * @file gfl_FileReadManager.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include <base/gfl_ThreadManager.h>
#include <fs/gfl_ArcFile.h>
#include <fs/gfl_FileReadManager.h>

namespace gfl {
  namespace fs {

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
    FileReadManager::FileReadManager( 
        gfl::heap::HeapBase* heap_for_management,
        u32 arcfile_count,
        u32 max_request_count,
        u8 max_read_count,
        base::Thread::Priority read_thread_priority ) :
      base::ThreadManager( heap_for_management, max_read_count, max_request_count ),
      base::IThreadManageCallback(),
      m_pWorkHeap( heap_for_management ),
      m_readThreadPriority( read_thread_priority ),
      m_manageData( NULL ),
      m_maxManageDataCount( arcfile_count )
    {
      m_manageData = GFL_NEW_ARRAY( heap_for_management ) FileManageData[ arcfile_count ];

      for( u32 i=0; i<m_maxManageDataCount; i++ ) {
        this->InitManageData( &m_manageData[i] );
      }
    }

    /**
     * @brief ファイル管理データを初期化する
     * @param pManageData 初期化するデータ
     */
    void FileReadManager::InitManageData( FileManageData* pManageData )
    {
      pManageData->arcID = ArcFile::ARCID_NULL;
      pManageData->arcFile = NULL;
      pManageData->heapForArcFile = NULL;
      pManageData->requestCount = 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    FileReadManager::~FileReadManager()
    {
      // 実行中のリクエストを待つ
      this->WaitAllRunningRequest();

      // 未実行のリクエストをキャンセル
      this->CancelAllWaitingRequest();

      // すべての管理データを破棄
      for( u32 i=0; i<m_maxManageDataCount; i++ ) {
        this->DeleteManageData( &m_manageData[i] );
      }
      GFL_DELETE_ARRAY( m_manageData );
    }

    /**
     * @brief 指定した管理ファイルを破棄する
     * @param pManageData 破棄する管理ファイルデータ
     */
    void FileReadManager::DeleteManageData( FileManageData* pManageData )
    {
      GFL_SAFE_DELETE( pManageData->arcFile );
      this->InitManageData( pManageData );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief アーカイブファイルが登録済みか？
     * @param arcID チェックするアーカイブファイルのID
     * @retval true  登録済み
     * @retval false 登録されていない
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::IsFileManaged( ArcFile::ARCID arcID ) const
    {
      return ( this->GetManageData( arcID ) != NULL );
    }

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void FileReadManager::AppendManageFile( 
        gfl::heap::HeapBase* heapForArc,
        ArcFile::ARCID arcID,
        bool hio )
    {
      if( this->IsFileManaged( arcID ) ) {
        GFL_PRINT( "WARNING!! アーカイブ( %d )は既に登録済みです\n", arcID );
        GFL_ASSERT(0);  //@check
        return;
      }

      u32 init_flag = ( hio ) ? ( ArcFile::HIO ) : ( 0 );
      FileManageData* manage_data = this->GetEmptyManageData();
      if( manage_data ) {
        manage_data->arcID = arcID;
        manage_data->arcFile = GFL_NEW( heapForArc ) ArcFile( heapForArc, arcID, init_flag );
        manage_data->heapForArcFile = heapForArc;
      }
      else {
        GFL_PRINT( "ERROR!! 登録ファイル数が上限を超えています\n" );
        GFL_ASSERT_STOP(0);
      }
    }

    /**
     * @brief 指定したアーカイブの管理データを取得する
     * @param arcID 対象アーカイブのID
     * @retval NULL 指定したアーカイブが登録されていない場合
     */
    FileReadManager::FileManageData* FileReadManager::GetManageData( ArcFile::ARCID arcID ) const
    {
      for( u32 i=0; i<m_maxManageDataCount; i++ )
      {
        if( m_manageData[i].arcID == arcID ) {
          return &m_manageData[i];
        }
      }
      return NULL;
    }

    /**
     * @brief 未使用の管理データを取得する
     * @return 未使用管理データのポインタ
     * @retval NULL  空の管理データがない場合
     */
    FileReadManager::FileManageData* FileReadManager::GetEmptyManageData( void ) const
    {
      for( u32 i=0; i<m_maxManageDataCount; i++ )
      {
        if( m_manageData[i].arcFile == NULL ) {
          return &m_manageData[i];
        }
      }
      return NULL;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 登録されているアーカイブファイルを取得する
     * @param arcID  取得対象のアーカイブファイルID
     * @retval NULL  指定したアーカイブファイルが登録されていない場合
     */
    //-----------------------------------------------------------------------
    gfl::fs::ArcFile* FileReadManager::GetManageFile( ArcFile::ARCID arcID ) const
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        return NULL;
      }
      return manage_data->arcFile;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを管理対象から除外する
     * @param arcID 登録を解除するアーカイブファイルのID
     *
     * @attention
     * arcID への読み込みが終了していない場合, カレントスレッドを休止させます。
     */
    //-------------------------------------------------------------------------
    void FileReadManager::RemoveManageFile( ArcFile::ARCID arcID )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        return;
      }

      // 対象ファイルの読み込みが終了していないなら, 終了を待つ
      while( 0 < manage_data->requestCount ) 
      {
        GFL_PRINT( "WARNING!! アーカイブ( %d )は読み込み中です。読み込みの終了を待ちます。\n", arcID );
        GFL_ASSERT(0);  //@check
        this->UpdateThreadManagement();
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) ); 
      }

      // ファイルの管理情報を破棄する
      this->DeleteManageData( manage_data );
    }
    
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
    bool FileReadManager::SetupForFastMode( ArcFile::ARCID arcID, gfl::heap::HeapBase* heapForFastTable )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        GFL_ASSERT_MSG( 0, "arcID=%d はマネージャーに未登録です\n", arcID );  //@check
        return false;
      }
      ArcFile* arcfile = manage_data->arcFile;

      if( arcfile->IsHioFile() != false ){
        return false; //HIO読み込みオプション付きの場合は移行しない
      }
      if( arcfile->IsFastMode() != false ){
        return false; //既にFastMode
      }
      //同期読み込みでFastModeへ
      if( heapForFastTable == NULL ){
        heapForFastTable = manage_data->heapForArcFile;
      }

      if( arcfile->IsOpen() == false ) {
        arcfile->Open();
      }
      arcfile->SetupForFastMode( heapForFastTable );
      return true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定アーカイブのFastMode移行を開始する(非同期読み込み版)
     *
     * @param  arcID             FastModeに移行させるアーカイブID
     * @param  heapForThread     高速化移行スレッドを生成する一時作業用ヒープNULL指定ならAppendManageFileに渡したヒープの後方から確保） 
     * @param  heapForFastTable  高速化テーブルを確保するヒープ(NULL指定ならAppendManageFileに渡したヒープから確保)
     *
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
    bool FileReadManager::StartSetupForFastMode(
          ArcFile::ARCID arcID,
          gfl::heap::HeapBase* heapForThread, 
          gfl::heap::HeapBase* heapForFastTable )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        GFL_ASSERT_MSG( 0, "arcID=%d はマネージャーに未登録です\n", arcID );  //@check
        return false;
      }
      ArcFile* arcfile = manage_data->arcFile;

      if( arcfile->IsHioFile() != false ){
        return false; //HIO読み込みオプション付きの場合は高速化モード移行リクエスト自体を行わないので何もしない
      }
      if( arcfile->IsFastMode() != false ){
        return false; //既にFastMode
      }
      if( heapForFastTable == NULL ){
        heapForFastTable = manage_data->heapForArcFile;
      }
      if( heapForThread == NULL ){
        heapForThread = manage_data->heapForArcFile;
      }
      //ファイルのOpenはStartSetupForFastModeのスレッドが行ってくれるので、ここでオープンしなくていい
      arcfile->StartSetupForFastMode( heapForThread->GetLowerHandle(), heapForFastTable );
      return true;
    }
   
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
    bool FileReadManager::WaitSetupForFastMode( ArcFile::ARCID arcID )
    {
      FileManageData* manage_data = this->GetManageData( arcID );
      if( manage_data == NULL ) {
        GFL_ASSERT_MSG( 0, "arcID=%d はマネージャーに未登録です\n", arcID );  //@check
        return true;
      }
      ArcFile* arcfile = manage_data->arcFile;

      if( arcfile->IsHioFile() != false ){
        return true; //HIO読み込みオプション付きの場合は高速化モード移行リクエスト自体を行わないので何もしない
      }
      return arcfile->WaitSetupForFastMode();
    }

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    bool FileReadManager::AddReadRequest( const ReadRequest& req )
    {
      FileManageData* manage_data = this->GetManageData( req.arcID );

      // アーカイブが未登録
      if( manage_data == NULL ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager 読み込みリクエストを発行する前に、対象アーカイブを AppendManageFile() で登録してください。\n" );
        return false;
      };

      // これ以上登録できない
      if( this->GetMaxWaitingRequestCount() <= this->GetWaitingRequestCount() ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager これ以上、読み込みリクエストを登録できません。\n" );
        return false;
      }

      // 読み込みスレッドの動作を定義
      ArcFileReadThread::Description read_desc;
      read_desc.dest_buf        = req.ppDestBuffer;
      read_desc.dest_size       = req.pDestBufferSize;
      read_desc.buf_alignment   = req.destBufferAlignment;
      read_desc.arcfile         = manage_data->arcFile;
      read_desc.datID           = req.datID;
      read_desc.compressed      = req.compressed;
      read_desc.heap_for_arc    = manage_data->heapForArcFile;
      read_desc.heap_for_buf    = req.heapForBuffer;
      read_desc.heap_for_uncomp = req.heapForUncomp;

      // 読み込みスレッド生成
      // スレッド終了時のコールバックで破棄する
      ArcFileReadThread* read_thread;
      if( req.heapForWork )
      {
        read_thread = GFL_NEW( req.heapForWork ) gfl::fs::ArcFileReadThread( req.heapForWork, read_desc );
      }
      else
      {
        gfl::heap::HeapBase* lower_heap = m_pWorkHeap->GetLowerHandle();
        read_thread = GFL_NEW( lower_heap ) gfl::fs::ArcFileReadThread( lower_heap, read_desc );
      }

      // スレッドリクエストを
      // 親クラスである ThreadManager に発行する
      base::ThreadManager::Request request;
      request.thread          = read_thread;
      request.threadPriority  = m_readThreadPriority;
      request.requestPriority = req.requestPriority;
      request.ID              = this->CalcRequestID( req.ppDestBuffer );
      request.callbackTarget  = this;
      request.pUserWork       = static_cast<void*>( manage_data );
      bool add_result = this->AddThreadRequest( request );
      GFL_ASSERT_STOP( add_result );

      // アーカイブファイルのリクエストカウンタをインクリメント
      manage_data->requestCount++;

      return true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 読み込みリクエストを登録する
     * @param[out] ppDestBuffer        読み込んだデータへのポインタをセットするポインタ変数のアドレス
     * @param[out] pDestBufferSize     読み込んだデータサイズの格納先( 不要ならNULL )
     * @param[in]  heapForBuffer       読み込み先バッファを確保するヒープ
     * @param[in]  destBufferAlignment 読み込み先バッファのアラインメント
     * @param[in]  arcID               読み込むデータのアーカイブのID
     * @param[in]  datID               読み込むデータのアーカイブ内データID
     * @param[in]  compressed          true:圧縮されている, false:圧縮されていない
     * @param[in]  requestPriority     リクエストの実行優先度( 小さい方が優先度が高い )
     *
     * @retval true  リクエストを登録できた
     * @retval false リクエストを登録できなかった
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::AddReadRequest( 
        void** ppDestBuffer,
        u32* pDestBufferSize,
        gfl::heap::HeapBase* heapForBuffer,
        u32 destBufferAlignment,
        ArcFile::ARCID arcID,
        ArcFile::ARCDATID datID,
        bool compressed,
        u8 requestPriority )
    {
      ReadRequest req;
      req.ppDestBuffer        = ppDestBuffer;
      req.pDestBufferSize     = pDestBufferSize;
      req.heapForBuffer       = heapForBuffer;
      req.destBufferAlignment = destBufferAlignment;
      req.arcID               = arcID;
      req.datID               = datID;
      req.compressed          = compressed;
      req.requestPriority     = requestPriority;
      return this->AddReadRequest( req );
    }

		// ↓2012.7.4.追加 tetsu
    //-------------------------------------------------------------------------
    /**
     * @brief 読み込みリクエストを登録する(外部指定先に読み込み)
     * @param req  リクエストデータ
     * @retval true  リクエストを登録できた
     * @retval false リクエストを登録できなかった
		 *
		 * リクエストIDは読み込み先バッファと等しい ID = (u32)pDestBuffer
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::AddReadRequestStatic( const ReadRequestStatic& req )
    {
      if( ( req.pDestBuffer == NULL ) ||
          ( req.destBufferSize <= 0 ) ||
          ( ( req.compressed == true ) && ( req.heapForUncomp == NULL ) ) ) {
        GFL_ASSERT(0);  //@fix
        return false;
      }

      FileManageData* manage_data = this->GetManageData( req.arcID );

      // アーカイブが未登録
      if( manage_data == NULL ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager 読み込みリクエストを発行する前に、対象アーカイブを AppendManageFile() で登録してください。\n" );
        return false;
      };

      // これ以上登録できない
      if( this->GetMaxWaitingRequestCount() <= this->GetWaitingRequestCount() ) {
        GFL_PRINT( "WARNING!! gfl::fs::FileReadManager これ以上、読み込みリクエストを登録できません。\n" );
        return false;
      }

      // 読み込みスレッドの動作を定義
      ArcFileReadThreadStatic::Description read_desc;
      read_desc.dest_buf        = req.pDestBuffer;
      read_desc.dest_size       = req.destBufferSize;
      read_desc.arcfile         = manage_data->arcFile;
      read_desc.datID           = req.datID;
      read_desc.compressed      = req.compressed;
      read_desc.heap_for_arc    = manage_data->heapForArcFile;
      read_desc.heap_for_uncomp = req.heapForUncomp;
      read_desc.read_size       = req.pReadSize;

      // 読み込みスレッド生成
      // スレッド終了時のコールバックで破棄する
      ArcFileReadThreadStatic* read_thread;
			if( req.heapForWork ){
        read_thread = GFL_NEW( req.heapForWork ) gfl::fs::ArcFileReadThreadStatic( req.heapForWork, read_desc );
			}else{
        gfl::heap::HeapBase* lower_heap = m_pWorkHeap->GetLowerHandle();
        read_thread = GFL_NEW( lower_heap ) gfl::fs::ArcFileReadThreadStatic( lower_heap, read_desc );
      }

      // スレッドリクエストを
      // 親クラスである ThreadManager に発行する
      base::ThreadManager::Request request;
      request.thread          = read_thread;
      request.threadPriority  = m_readThreadPriority;
      request.requestPriority = req.requestPriority;
      request.ID              = (u32)req.pDestBuffer;	// キャンセル時の固有ID
																											// ありえないはずだが同時に同領域に読み込みをした際キャンセルすると
																											// 意図しない挙動が出るかもしれないので警告が必要か？
      request.callbackTarget  = this;
      request.pUserWork       = static_cast<void*>( manage_data );
      bool add_result = this->AddThreadRequest( request );
      GFL_ASSERT_STOP( add_result );

      // アーカイブファイルのリクエストカウンタをインクリメント
      manage_data->requestCount++;

      return true;
    }

		// 以下IDの指定方法の問題により個別関数作成
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
    bool FileReadManager::CancelReadRequest( u32 requestID )
    {
      if( this->IsRequestWaiting( requestID ) ) {
        this->CancelWaitingThreadRequest( requestID );
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief IDを直接指定して指定したデータの読み込みが完了したかを確認
     * @retval true  完了した
     * @retval false 完了していない
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::IsReadFinished( u32 requestID ) const
    {
      return this->IsRequestFinished( requestID );
    }

		// ↑2012.7.4.追加 tetsu


    /**
     * @brief 読み込みリクエストのIDを計算する
     * @param[in] ppDestBuffer 読込先バッファのアドレス
     */
    u32 FileReadManager::CalcRequestID( void** ppDestBuffer ) const
    {
      return (u32)( ppDestBuffer ); // 読込先アドレスをIDとする
    }

    /**
     * @brief コールバック: リクエスト終了
     * @param finished_request  終了したリクエスト
     */
    void FileReadManager::OnRequestFinished( const base::ThreadManager::Request& finished_request )
    {
      this->FinalizeRequest( finished_request ); 

#if GFLTHREADMAN_TICK_ENABLE
      // かかった時間を出力
      nn::os::Tick tick( finished_request.GetRunningTick() );
      GFL_PRINT( "ファイル読み込み終了: かかった時間=%dms\n", tick.ToTimeSpan().GetMilliSeconds() );
#endif
    }

    /**
     * @brief コールバック: リクエストが実行されずに終了した
     * @param canceled_request  実行されなかったリクエスト
     */
    void FileReadManager::OnRequestCanceled( const base::ThreadManager::Request& canceled_request )
    {
      this->FinalizeRequest( canceled_request );
    }

    /**
     * @brief リクエストの終了処理
     * @param request 対象のリクエスト
     */
    void FileReadManager::FinalizeRequest( const base::ThreadManager::Request& request )
    {
      // 読み込みカウンタをデクリメント
      FileManageData* manage_data = reinterpret_cast<FileManageData*>( request.pUserWork );
      GFL_ASSERT_STOP( 0 < manage_data->requestCount );  //@check
      manage_data->requestCount--;

      // 読み込みスレッドを破棄
      GFL_DELETE request.thread; 
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 読み込みリクエストをキャンセルする
     * @retval true   キャンセルしました
     * @retval false  キャンセルできませんでした( すでに読み込み中 )
     *
     * @note 
     * 既に読み込み中のリクエストはキャンセルできません。
     * IsReadFinished() で, 読み込みの終了を待ってください。
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::CancelReadRequest( void** ppDestBuffer )
    {
      u32 requestID = this->CalcRequestID( ppDestBuffer ); 
      if( this->IsRequestWaiting( requestID ) ) {
        this->CancelWaitingThreadRequest( requestID );
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したデータの読み込みが完了したか？
     * @param[in] ppDestBuffer 読込先バッファ
     * @retval true  完了した
     * @retval false 完了していない
     */
    //-------------------------------------------------------------------------
    bool FileReadManager::IsReadFinished( void** ppDestBuffer ) const
    {
      u32 requestID = this->CalcRequestID( ppDestBuffer );
      return this->IsRequestFinished( requestID );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 全ての読み込みが完了したか？
     * @retval true   完了した
     * @retval falsek 完了していない
     */
    //-----------------------------------------------------------------------
    bool FileReadManager::IsAllReadFinished( void ) const
    {
      return this->IsAllRequestFinished();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 更新処理
     */
    //-------------------------------------------------------------------------
    void FileReadManager::UpdateFileRead( void )
    {
      // スレッド管理処理を更新
      this->UpdateThreadManagement();
    }




  }  // namespace fs
}  // namespace gfl
