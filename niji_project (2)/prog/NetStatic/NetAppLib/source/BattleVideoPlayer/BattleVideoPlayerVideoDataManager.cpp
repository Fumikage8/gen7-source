//==============================================================================
/**
 @file    BattleVideoPlayerVideoDataManager.cpp
 @date    2015.09.24
 */
//==============================================================================
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "Print/include/wordset.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

/**
  Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
    ダウンロードしたビデオデータの登録日時を強制的に上書きする
*/
#if PM_DEBUG
  #include  "./Debug/BattleVideoPlayerDebug.h"
#endif


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)





  //  ポケモンパラメータ出力
  //  左配置用
  bool  VIDEO_DATA::OutputLeftPosPokeParam( PokeTool::SimpleParam* pOutputParam, const u32 idx )
  {
    if( pOutputParam == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( GetPokeMemberCount() <= idx )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    u32 nowIdx  = idx;
    pOutputParam->monsNo     = (MonsNo)sdCardData.upload.header.mons_no[ nowIdx ];
    if( pOutputParam->monsNo == MONSNO_NULL )
    {
      return  false;
    }

    pOutputParam->formNo     = sdCardData.upload.header.form_no[ nowIdx ];
    pOutputParam->sex        = (pml::Sex)sdCardData.upload.header.gender[ nowIdx ];
    pOutputParam->isRare     = sdCardData.upload.header.rare[ nowIdx ];

    return true;
  }

  //  右配置用
  bool  VIDEO_DATA::OutputRightPosPokeParam( PokeTool::SimpleParam* pOutputParam, const u32 idx )
  {
    if( pOutputParam == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( GetPokeMemberCount() <= idx )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    u32 nowIdx  = GetPokeMemberCount() + idx;
    pOutputParam->monsNo     = (MonsNo)sdCardData.upload.header.mons_no[ nowIdx ];
    if( pOutputParam->monsNo == MONSNO_NULL )
    {
      return  false;
    }

    pOutputParam->formNo     = sdCardData.upload.header.form_no[ nowIdx ];
    pOutputParam->sex        = (pml::Sex)sdCardData.upload.header.gender[ nowIdx ];
    pOutputParam->isRare     = sdCardData.upload.header.rare[ nowIdx ];

    return true;
  }

  //  プレイヤーアイコンオブジェクトデータ出力
  bool  VIDEO_DATA::OutputIconObjectParam( app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA* pOutputParam, PLAYER_DATA& rPlayerData )
  {
    if( pOutputParam == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    // @fix GFNMCat[1314] バトルのトレーナーidからアイコンロードデータを生成
    if( IsPlayerTrainerIcon( rPlayerData ) == true )
    {
      pOutputParam->ConvFromMyStatusIconData( rPlayerData.iconData );
    }
    else
    {
      pOutputParam->trainerId = rPlayerData.trainerId;
    }

    return  true;
  }

  //  主人公アイコンか?
  bool  VIDEO_DATA::IsPlayerTrainerIcon( PLAYER_DATA& rPlayerData )
  {
    // @fix GFNMCat[1314] バトルのトレーナーidからアイコンロードデータを生成
    if( ( rPlayerData.trainerId == trainer::TRTYPE_HERO ) || ( rPlayerData.trainerId == trainer::TRTYPE_HEROINE ) )
    {
      return  true;
    }

    return  false;
  }

  VideoDataManager::VideoDataManager( gfl2::heap::HeapBase* pHeap, const NetMode mode )
  {
    GFL_ASSERT( pHeap );

    _Clear();

    m_pHeap   = pHeap;
    m_netMode = mode;

    m_pTmpStrBuf  = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( 256, m_pHeap );

    //  アプリ内で参照するビデオデータリストバッファ作成
    {
      u32 loopNum = GFL_NELEMS( m_aBufferData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        _CreateDataBuffer( &m_aBufferData[ i ], m_pHeap );
        m_pListData[ i ]  = &m_aBufferData[ i ];
      }
    }

    //  シリアルNOのデータバッファ作成
    {
      _CreateDataBuffer( &m_serialNoData, m_pHeap );
    }

    //  バトルレコーダーSDカードセーブ作成
    {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      pHeap->PrintHeapInfo( "befor ExtSavedata::BattleRecorderSaveData\n" );
#endif
      m_pBtlRecorderSaveData = GFL_NEW( m_pHeap )  ExtSavedata::BattleRecorderSaveData( pHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
#ifdef DEBUG_ONLY_FOR_uchida_yuto
      pHeap->PrintHeapInfo( "after ExtSavedata::BattleRecorderSaveData\n" );
#endif
    }

    {
      //  オンライン用の通信クラスを作成
      m_pNetRequestClient = GFL_NEW( m_pHeap ) NetApp::BattleVideo::BattleVideoRequestClient( this, pHeap );

      //  リクエストパラメータでメモリ確保が必要なのはあらかじめ作成
      {
        m_netDeleteReqeustParam.indexArray = GFL_NEW_ARRAY( m_pHeap ) u32[ VIDEO_DATA_MAX ];
        m_netDeleteReqeustParam.dataArray = GFL_NEW_ARRAY( m_pHeap ) ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA*[ VIDEO_DATA_MAX ];
      }
    }

    /**
      Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
        デバッグ機能の追加
    */
#if PM_DEBUG
    {
      gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

      m_pDebug  = GFL_NEW(pHeapBase) Debug::VideoPlayerDataDebug();
    }
#endif
  }

  VideoDataManager::~VideoDataManager()
  {
    /**
      Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
        デバッグ機能の追加
    */
#if PM_DEBUG
    {
      GFL_SAFE_DELETE(m_pDebug);
    }
#endif

    GFL_SAFE_DELETE_ARRAY( m_netDeleteReqeustParam.dataArray )
    GFL_SAFE_DELETE_ARRAY( m_netDeleteReqeustParam.indexArray );
    GFL_SAFE_DELETE( m_pNetRequestClient );

    //  アプリ内で参照するビデオデータリストバッファ削除
    {
      u32 loopNum = GFL_NELEMS( m_aBufferData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        //  バッファ開放
        _DeleteDataBuffer( &m_aBufferData[ i ] );
      }

      _DeleteDataBuffer( &m_serialNoData );
    }

    GFL_SAFE_DELETE( m_pTmpStrBuf );
    GFL_SAFE_DELETE( m_pBtlRecorderSaveData );
  }

  //  クラス破棄する前の終了処理(破棄する前に必ず呼ぶ)
  bool  VideoDataManager::EndFunc()
  {
    ExtSavedata::NijiExtSaveSystem* pExtSaveSystem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();
    if( pExtSaveSystem == NULL )
    {
      GFL_ASSERT( false );
      return  true;
    }
#if defined(GF_PLATFORM_CTR)
    return  pExtSaveSystem->FinalizeSaveForShutdown();
#else
    return  true;
#endif
  }

  //  ビデオデータ初期化
  void VideoDataManager::InitVideoData()
  {
    m_listDataNum   = 0;
    m_tmpLoadChkIdx = 0;

    u32 loopNum = GFL_NELEMS( m_aBufferData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      VIDEO_DATA* pCurrentVideoData = &m_aBufferData[ i ];

      //  タイトル名
      pCurrentVideoData->pTitleName01->Clear();
      pCurrentVideoData->pTitleName02->Clear();

      //  プレイヤーデータ
      u32 player_num  = GFL_NELEMS( pCurrentVideoData->aPlayerData );
      for( u32 player_idx = 0; player_idx < player_num; ++player_idx )
      {
        PLAYER_DATA*  pPlayerData = &pCurrentVideoData->aPlayerData[ player_idx ];
        pPlayerData->pName->Clear();
      }

      m_pListData[ i ]  = NULL;
    }

#if PM_DEBUG
    m_bDummyData = false;
    if( m_bDummyData == true )
    {
      _CreateDummyData( m_pHeap );
    }
#endif
  }

  //  ビデオデータロード
  VideoDataManager::LoadResult VideoDataManager::LoadVideoData()
  {
#if PM_DEBUG
      if( m_bDummyData == true )
      {
        return  LOAD_RESULT_SUCCESS;
      }

#endif
    //  データ作成
    {
      if( m_tmpLoadChkIdx < VIDEO_DATA_MAX )
      {
        bool  bLoadFlg  = false;
        // SDカードからデータをロード
        ExtSavedata::BattleRecorderSaveData::Result result = m_pBtlRecorderSaveData->LoadData( m_tmpLoadChkIdx );

        // 処理結果
      
        // データがあって正常に読み込まれたら、ビデオ情報作成処理に入る
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_SUCCESS )
        {
          bLoadFlg = true;
        }
        else
        // データが壊れているなら削除
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_DATA_BROKEN )
        {
          m_bDeleteBrokeData = true;

          UCHIDA_PRINT( "バトルビデオ読み込み時に壊れていたので削除\n" );

          // FatalErroのみ検知
          if( m_pBtlRecorderSaveData->DeleteData( m_tmpLoadChkIdx ) == ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR )
          {
            // FatalError
            return  LOAD_RESULT_FATAL;
          }
        }
        else
        // FatalError
        if( result == ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR )
        {
          // FatalError
          return  LOAD_RESULT_FATAL;
        }

        if( bLoadFlg == true )
        {
          //  ロード成功なのでリスト追加
          const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pRecData = m_pBtlRecorderSaveData->GetBattleRecData();

          VIDEO_DATA*  pSettingVideoData = &m_aBufferData[ m_listDataNum ];
          // 必要なデータを設定
          {
            //  SDカードのデータを丸ごとコピーする（ひとつずつしてロードしているのでポインタでもたせることはできないから）
            pSettingVideoData->sdCardData       = *pRecData;
            pSettingVideoData->bErrorVersionFlg = ( pRecData->upload.header.server_version != BTL_NET_SERVER_VERSION );    // バージョン違いかどうか
            pSettingVideoData->saveDataListIdx  = m_tmpLoadChkIdx;    ///<セーブデータのidx

            //  アップロードデータからVIDEO_DATAを出力
            _CreateVideoDataFromUploadData( pSettingVideoData, m_pHeap, &pRecData->upload );
          }

          m_pListData[ m_listDataNum ] = pSettingVideoData;
          ++m_listDataNum;

          // ロード状態を破棄する
          m_pBtlRecorderSaveData->ResetLoadData();
        }

        ++m_tmpLoadChkIdx;
        if( m_tmpLoadChkIdx < VIDEO_DATA_MAX )
        {
          //  まだ全ロードしていないのであれば継続
          return  LOAD_RESULT_RUN;
        }
      }
    }

    //  日付を新しいのに並べ替える(おそいソート整列法なので O(n^))
    {
      // 有効データを日付で並び替える
      for( u32 i = 0; i < m_listDataNum; ++i )
      {
        VIDEO_DATA* pData1 = m_pListData[ i ];

        for( u32 j = i + 1; j < m_listDataNum; ++j )
        {
          VIDEO_DATA* pData2 = m_pListData[ j ];

          // 日付が新しいかチェック
          if( pData1->createDate.GetDateTimeToSecond() < pData2->createDate.GetDateTimeToSecond() )
          {
            UCHIDA_PRINT(" %d と %d 交換\n", i, j );

            UCHIDA_PRINT("作成日付 %d/%d/%d %d:%d:%d \n",
                        pData1->createDate.GetYear(), pData1->createDate.GetMonth(), pData1->createDate.GetDay(),
                        pData1->createDate.GetHour(), pData1->createDate.GetMinute(), pData1->createDate.GetSecond() );

            UCHIDA_PRINT("作成日付 %d/%d/%d %d:%d:%d \n",
                        pData2->createDate.GetYear(), pData2->createDate.GetMonth(), pData2->createDate.GetDay(),
                        pData2->createDate.GetHour(), pData2->createDate.GetMinute(), pData2->createDate.GetSecond() );

            VIDEO_DATA  tmp;
            gfl2::std::MemCopy( pData1, &tmp, sizeof(VIDEO_DATA) );
            gfl2::std::MemCopy( pData2, pData1, sizeof(VIDEO_DATA) );
            gfl2::std::MemCopy( &tmp, pData2, sizeof(VIDEO_DATA) );
          }
        }
      }
    }

    return  LOAD_RESULT_SUCCESS;
  }

  //  オンライン初期化
  bool VideoDataManager::InitOnline()
  {
    //  オフラインなので処理しない
    if( m_netMode == NET_MODE_OFFLINE )
    {
      return  false;
    }

    GFL_ASSERT( m_resultSyncOnlineData != LOAD_RESULT_RUN );

    //  オンラインなので処理をする
    //  SDカードのセーブデータを見てサーバーデータを削除する
    NetApp::BattleVideo::BattleVideoRequestClient::SYNC_REQUEST_PARAM  param;
    param.pBattleRecorderSaveData = m_pBtlRecorderSaveData;
    param.pDataList = m_pListData;
    param.dataListNum = m_listDataNum;

    m_pNetRequestClient->SyncBattleVideoRequest( param );

    //  オンライン処理開始！
    m_resultSyncOnlineData  = LOAD_RESULT_RUN;

    return  true;
  }

  //  オンラインデータロード
  VideoDataManager::LoadResult VideoDataManager::LoadOnline()
  {
    return  m_resultSyncOnlineData;
  }

  //  削除フラグがついてるデータを削除開始
  //  削除開始成功 -> true / 失敗 -> false
  bool  VideoDataManager::StartDeleteVideoData()
  {
    //  削除開始!

    m_deleteVideoDataSeqData.Reset();

    //  削除フラグがついているVideoデータ配列を用意
    {
      u32   cnt = 0;
      u32   loopNum = m_listDataNum;
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_pListData[ i ]->bDelFlg == 1 )
        {
          m_deleteVideoDataSeqData.pDeleteListData[ cnt ]  = m_pListData[ i ];
          ++cnt;
        }
      }

      //  一つも削除するビデオデータがないので終了
      if( cnt <= 0 )
      {
        return  false;
      }

      m_deleteVideoDataSeqData.delDataNum = cnt;
    }

    return true;
  }

  //  削除フラグがついているデータから削除処理更新
  VideoDataManager::DeleteResult  VideoDataManager::UpdateDeleteVideoData()
  {
    DeleteResult  deleteResult  = DELETE_RESULT_RUN;

#if PM_DEBUG
      if( m_bDummyData == true )
      {
        deleteResult  = DELETE_RESULT_SUCCESS;
        m_deleteVideoDataSeqData.seq = DELETE_VIDEO_DATA_SEQ_END;
      }
#endif

    if( m_deleteVideoDataSeqData.seq == DELETE_VIDEO_DATA_SEQ_NET_START )
    {
      //  サーバーデータを削除
      GFL_ASSERT( m_pNetRequestClient );

      //  パラメータ作成
      {
        u32 deleteNum  = m_deleteVideoDataSeqData.delDataNum;
        GFL_ASSERT( m_netDeleteReqeustParam.deleteNum < VIDEO_DATA_MAX );
        for( u32 i = 0; i < deleteNum; ++i )
        {
          m_netDeleteReqeustParam.indexArray[ i ]  = m_deleteVideoDataSeqData.pDeleteListData[ i ]->saveDataListIdx;
          m_netDeleteReqeustParam.dataArray[ i ]   = &m_deleteVideoDataSeqData.pDeleteListData[ i ]->sdCardData;
        }

        m_netDeleteReqeustParam.isOnline  = ( m_netMode == NET_MODE_ONLINE );
        m_netDeleteReqeustParam.deleteNum = deleteNum;
        m_netDeleteReqeustParam.pBattleRecorderSaveData = m_pBtlRecorderSaveData;
      }

      //  サーバーに削除依頼開始！
      //  内部でSDカードの処理もしている
      m_pNetRequestClient->DeleteBattleVideoRequest( m_netDeleteReqeustParam );
      m_deleteVideoDataSeqData.seq = DELETE_VIDEO_DATA_SEQ_NET_WAIT;
    }
    else
    if( m_deleteVideoDataSeqData.seq == DELETE_VIDEO_DATA_SEQ_NET_WAIT )
    {
      //  サーバーデータ削除待ち
      GFL_ASSERT( m_pNetRequestClient );
      deleteResult  = m_deleteVideoDataSeqData.netRequestResult;
    }

    if( deleteResult == DELETE_RESULT_SUCCESS )
    {
      //  削除成功の場合はリスト整理
      //  データリスト上では削除フラグがついているのははずす
      _RegulationList();
    }

    return deleteResult;
  }

  //  シリアルNOからデータロードの準備
  bool VideoDataManager::InitSerialNoData()
  {
    m_pResultSerialNoData = NULL;

    //  オフラインなので処理しない
    if( m_netMode == NET_MODE_OFFLINE )
    {
      return  false;
    }

    m_createSerirlaNoVideoDataSeqData.Reset();

    return true;
  }

  //  シリアルNOからデータロード
  VideoDataManager::SerialNoLoadResult VideoDataManager::LoadSerialNoData( const u64 no, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
  {
    if( m_createSerirlaNoVideoDataSeqData.seq == CREATE_DATA_SERIAL_NO_SEQ_CHK )
    {
      //  自分のデータかチェック
      VIDEO_DATA* pPublicData = GetSerialPublicData( no );
      if( pPublicData != NULL )
      {
        //  公開しているデータなので終了
        m_pResultSerialNoData  = pPublicData;
        m_pResultSerialNoData->bServerDataFlg = false;

        m_createSerirlaNoVideoDataSeqData.seq     = CREATE_DATA_SERIAL_NO_SEQ_END;

        // @fix NMCat[3783] バージョンチェック
        // NMCat[3783] >>> NMCat[4279] バージョンは完全一致で確認するように変更します k.ohno
        if( pPublicData->sdCardData.upload.header.server_version == BTL_NET_SERVER_VERSION )
        {
          m_createSerirlaNoVideoDataSeqData.result  = SERIAL_NO_LOAD_RESULT_SUCCESS;          
        }
        else
        {
          m_createSerirlaNoVideoDataSeqData.result  = SERIAL_NO_LOAD_RESULT_ERROR_SERVER_VERSION;
        }

        // @fix GFNMCat[1241] ローカルデータを参照フラグをつける
        m_bUploadSelfData = true;
      }
      else
      {
        //  シリアルNOが不正かチェック
        m_bUploadSelfData = false;

        //  前回のエラーnoをロードしているデータかチェック
        if( m_serialNoNotFound == no )
        {
          m_createSerirlaNoVideoDataSeqData.result = SERIAL_NO_LOAD_RESULT_NOT_FOUND;
          m_createSerirlaNoVideoDataSeqData.seq    = CREATE_DATA_SERIAL_NO_SEQ_END;
        }
        else
        {
          //  検索開始
          NetApp::BattleVideo::BattleVideoRequestClient::DOWNLOAD_REQUEST_PARAM reqParam;
          reqParam.dataID     = no;
          m_serialNoNotFound  = reqParam.dataID;

          /**
            Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
              デバッグ機能の追加
          */
#if PM_DEBUG
          reqParam.pDebug  = m_pDebug;
#endif
          m_pNetRequestClient->DownloadBattleVideoRequest( reqParam );
          m_createSerirlaNoVideoDataSeqData.seq = CREATE_DATA_SERIAL_NO_SEQ_DOWN_LOAD;
        }
      }
    }
    else
    if( m_createSerirlaNoVideoDataSeqData.seq == CREATE_DATA_SERIAL_NO_SEQ_DOWN_LOAD )
    {
      if( m_createSerirlaNoVideoDataSeqData.result != SERIAL_NO_LOAD_RESULT_RUN )
      {
        m_createSerirlaNoVideoDataSeqData.seq = CREATE_DATA_SERIAL_NO_SEQ_END;
      }
    }
    else
    if( m_createSerirlaNoVideoDataSeqData.seq == CREATE_DATA_SERIAL_NO_SEQ_END )
    {
      //  成功時の共通処理
      if( m_createSerirlaNoVideoDataSeqData.result == SERIAL_NO_LOAD_RESULT_SUCCESS )
      {
        m_serialNoNotFound = 0;

        // @fix GFNMCat[1241] ローカルデータでない他人のデータの場合は別途設定
        if( m_bUploadSelfData == false )
        {
          m_serialNoData.sdCardData.dataID  = no;
          m_pResultSerialNoData  = &m_serialNoData;

          //  タイトル更新
          {
            _CreateTitleName( m_pResultSerialNoData, pMsgData, pWordSet );
          }

          m_pResultSerialNoData->bServerDataFlg = true;
        }

        //  アップロードフラグを立てる(アップロード情報を見せるため)
        m_pResultSerialNoData->sdCardData.upload_flag = true;
        m_bUploadSelfData = false;
      }
      else
      {
      }

      return m_createSerirlaNoVideoDataSeqData.result;
    }

    return SERIAL_NO_LOAD_RESULT_RUN;
  }

  //  アップロード開始
  bool VideoDataManager::StartUpload( VIDEO_DATA* pUploadVideoData )
  {
    m_uploadVideoDataSeqData.Reset();

    if( pUploadVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_netMode == NET_MODE_OFFLINE )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  参照するビデオデータ
    m_uploadVideoDataSeqData.pUploadData  = pUploadVideoData;

    return true;
  }

  //  アップロード中
  VideoDataManager::UploadResult VideoDataManager::WaitUpload()
  {
    UploadVideoDataSeq  seq = m_uploadVideoDataSeqData.seq;
    if( seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_START )
    {
      //  アップロード開始

      NetApp::BattleVideo::BattleVideoRequestClient::UPLOAD_REQUEST_PARAM  request;
      request.pBattleRecorderSaveData = m_pBtlRecorderSaveData;
      request.pUploadData             = &m_uploadVideoDataSeqData.pUploadData->sdCardData;
      request.index                   = m_uploadVideoDataSeqData.pUploadData->saveDataListIdx;

      //  アップロード開始（SDカードセーブもまとめてやる）
      m_pNetRequestClient->UploadBattleVideoRequest( request );

      m_uploadVideoDataSeqData.seq  = UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT;
    }
    else
    if( seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
    }
    else
    if( seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_END )
    {
      //  サーバーアップロード完了

      //  登録日時を設定
      m_uploadVideoDataSeqData.pUploadData->UpdateRegistDate();
      m_uploadVideoDataSeqData.seq  = UPLOAD_VIDEO_DATA_SEQ_END;

      return UPLOAD_RESULT_SUCCESS;
    }
    else if( seq == UPLOAD_VIDEO_DATA_SEQ_END )
    {
      //エラー
      return m_uploadVideoDataSeqData.result;
    }

    return UPLOAD_RESULT_RUN;
  }

  //  アップロードキャンセル開始
  bool VideoDataManager::StartUploadCancel( VIDEO_DATA* pUploadCancelVideoData )
  {
    m_uploadCancelVideoDataSeqData.Reset();

    if( pUploadCancelVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_netMode == NET_MODE_OFFLINE )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  参照するビデオデータ
    m_uploadCancelVideoDataSeqData.pUploadCancelData  = pUploadCancelVideoData;

    return true;
  }

  //  アップロードキャンセル中
  VideoDataManager::UploadCancelResult  VideoDataManager::WaitUploadCancel()
  {
    UploadCancelVideoDataSeq  seq = m_uploadCancelVideoDataSeqData.seq;
    if( seq == UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_START )
    {
      NetApp::BattleVideo::BattleVideoRequestClient::CANCEL_UPLOAD_REQUEST_PARAM requestParam;

      requestParam.index                    = m_uploadCancelVideoDataSeqData.pUploadCancelData->saveDataListIdx;
      requestParam.pBattleRecorderSaveData  = m_pBtlRecorderSaveData;
      requestParam.pCancelUploadData        = &m_uploadCancelVideoDataSeqData.pUploadCancelData->sdCardData;

      //  サーバーに削除依頼開始！
      //  SDカードもまとめてやる
      m_pNetRequestClient->CancelUploadBattleVideoRequest( requestParam );

      m_uploadCancelVideoDataSeqData.seq  = UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_WAIT;
    }
    else
    if( seq == UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
    }
    else
    if( seq == UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_END )
    {
      //  サーバーアップロードキャンセル完了
      return  m_uploadCancelVideoDataSeqData.result;
    }

    return  UPLOAD_CANCEL_RESULT_RUN;
  }

  //  再生回数のアップ開始
  bool  VideoDataManager::StartIncPlayCountVideoData( VIDEO_DATA* pVideoData )
  {
    if( pVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_netMode == NET_MODE_OFFLINE )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    m_incPlayCountVideoDataSeqData.Reset();
    m_incPlayCountVideoDataSeqData.pVideoData = pVideoData;

    return  true;
  }

  //  再生回数のアップ処理待ち
  VideoDataManager::IncPlayCountResult VideoDataManager::UpdateIncPlayCountVideoData()
  {
    if( m_incPlayCountVideoDataSeqData.seq == INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_START )
    {
      NetApp::BattleVideo::BattleVideoRequestClient::INC_REQUEST_PARAM  param;
      param.dataID  = m_incPlayCountVideoDataSeqData.pVideoData->sdCardData.dataID;

      m_pNetRequestClient->IncPlayCountRequest( param );

      m_incPlayCountVideoDataSeqData.seq  = INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_WAIT;
    }
    else
    if( m_incPlayCountVideoDataSeqData.seq == INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
    }
    else
    if( m_incPlayCountVideoDataSeqData.seq == INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_END )
    {
      if( m_incPlayCountVideoDataSeqData.result == INC_PLAY_COUNT_RESULT_SUCCESS )
      {
      }

      return  m_incPlayCountVideoDataSeqData.result;
    }

    return  INC_PLAY_COUNT_RESULT_RUN;
  }

  //  再生完走の通知
  bool  VideoDataManager::StartSendPlayComplete( VIDEO_DATA* pVideoData )
  {
    if( pVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_netMode == NET_MODE_OFFLINE )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    m_sendPlayComplateVideoDataSeqData.Reset();
    m_sendPlayComplateVideoDataSeqData.pVideoData = pVideoData;

    return  true;
  }

  //  再生完走の通知処理待ち
  VideoDataManager::SendPlayComplateResult  VideoDataManager::UpdateSendPlayComplate()
  {
    if( m_sendPlayComplateVideoDataSeqData.seq == SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_START )
    {
      NetApp::BattleVideo::BattleVideoRequestClient::COMPLETE_REQUEST_PARAM  param;
      param.dataID  = m_sendPlayComplateVideoDataSeqData.pVideoData->sdCardData.dataID;

      m_pNetRequestClient->SendPlayCompleteRequest( param );

      //  待ち状態に遷移
      m_sendPlayComplateVideoDataSeqData.seq = SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_WAIT;
    }
    else
    if( m_sendPlayComplateVideoDataSeqData.seq == SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
    }
    else
    if( m_sendPlayComplateVideoDataSeqData.seq == SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_END )
    {
      return  m_sendPlayComplateVideoDataSeqData.result;
    }

    return  SEND_PLAY_COMPLATE_RESULT_RUN;
  }

  //  データ取得
  VIDEO_DATA* VideoDataManager::GetListData( const u32 idx )
  {
    if( m_listDataNum <= idx )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    return  m_pListData[ idx ];
  }

  //  指定シリアルNOのデータが公開データ取得
  VIDEO_DATA* VideoDataManager::GetSerialPublicData( const u64 no )
  {
    u32 loopNum = m_listDataNum;
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pListData[ i ]->sdCardData.dataID == no )
      {
        return  m_pListData[ i ];
      }
    }

    return  NULL;
  }

  //  指定したビデオデータのidxを取得
  u32 VideoDataManager::GetVideoDataListIdx( const VIDEO_DATA* pVideoData )
  {
    if( pVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return  (u32)LIST_IDX_ERROR;
    }

    u32 loopNum = m_listDataNum;
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pListData[ i ] == pVideoData )
      {
        return  i;
      }
    }

    return  (u32)LIST_IDX_ERROR;
  }

  //  削除フラグ取得・設定
  void VideoDataManager::SetDataDelFlg( const u32 idx, const bool bFlg )
  {
    if( m_listDataNum <= idx )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_pListData[ idx ]->bDelFlg  = bFlg;
  }

  bool VideoDataManager::GetDataDelFlg( const u32 idx )
  {
    if( m_listDataNum <= idx )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    return  ( m_pListData[ idx ]->bDelFlg == 1 );
  }

  //  削除フラグをクリア
  void VideoDataManager::ClearDataDelFlg()
  {
    for( u32 i = 0; i < m_listDataNum; ++i )
    {
      SetDataDelFlg( i, false );
    }
  }

  //  削除フラグがついている個数を取得
  u32 VideoDataManager::GetDataDelFlgCount()
  {
    u32   cnt = 0;
    u32   loopNum = m_listDataNum;
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pListData[ i ]->bDelFlg == 1 )
      {
        ++cnt;
      }
    }

    return  cnt;
  }

  //  タイトル名更新
  void VideoDataManager::UpdateTitleName( gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
  {
    GFL_ASSERT( pMsgData );
    GFL_ASSERT( pWordSet );

    u32 loopNum = m_listDataNum;
    for( u32 i = 0; i < loopNum; ++i )
    {
      _CreateTitleName( m_pListData[ i ], pMsgData, pWordSet );
    }
  }

//  通信処理時のコールバック
  //--------------------------------------------------------------------------------------------
  /**
    * @brief   通信エラー
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnError( BattleVideoResponseListener::ERROR_TYPE errorType )
  {
    //  データ同期中か
    if( m_resultSyncOnlineData == LOAD_RESULT_RUN )
    {
      m_resultSyncOnlineData  = LOAD_RESULT_ERROR;
    }

    //  削除リクエスト中か
    if( m_deleteVideoDataSeqData.seq == DELETE_VIDEO_DATA_SEQ_NET_WAIT )
    {
      if( errorType == BattleVideoResponseListener::ERROR_TYPE_fatal )
      {
        m_deleteVideoDataSeqData.netRequestResult = DELETE_RESULT_FATAL;
      }
      else
      {
        m_deleteVideoDataSeqData.netRequestResult = DELETE_RESULT_ERROR;
      }
    }

    //  シリアルNOからダウンロード中か
    if( m_createSerirlaNoVideoDataSeqData.seq == CREATE_DATA_SERIAL_NO_SEQ_DOWN_LOAD )
    {
      m_createSerirlaNoVideoDataSeqData.result  = SERIAL_NO_LOAD_RESULT_ERROR;
    }

    //  アップロードキャンセルに失敗
    if( m_uploadCancelVideoDataSeqData.seq  == UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_uploadCancelVideoDataSeqData.seq      = UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_END;
      m_uploadCancelVideoDataSeqData.result   = UPLOAD_CANCEL_RESULT_ERROR;
    }

    //  再生回数のアップに失敗
    if( m_incPlayCountVideoDataSeqData.seq == INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_incPlayCountVideoDataSeqData.seq = INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_END;
      m_incPlayCountVideoDataSeqData.result = INC_PLAY_COUNT_RESULT_ERROR;
    }

    //  再生完走通知のアップに失敗
    if( m_sendPlayComplateVideoDataSeqData.seq == SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_sendPlayComplateVideoDataSeqData.seq  = SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_END;
      m_sendPlayComplateVideoDataSeqData.result = SEND_PLAY_COMPLATE_RESULT_ERROR;
    }

    if( errorType == NetApp::BattleVideo::BattleVideoResponseListener::ERROR_TYPE_uploadFailed )
    {
      // @fix GFNMCat[1131] アップロード最大数を超えた場合のエラー
      //                    この時にはシーケンスはアップロードサーバー待ち状態でないとアウト
      GFL_ASSERT( m_uploadVideoDataSeqData.seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT );

      m_uploadVideoDataSeqData.result = UPLOAD_RESULT_OVERLIMIT;
      m_uploadVideoDataSeqData.seq    = UPLOAD_VIDEO_DATA_SEQ_END;      
    }
    else if( m_uploadVideoDataSeqData.seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT )
    {//  アップロード失敗
      m_uploadVideoDataSeqData.result = UPLOAD_RESULT_ERROR;
      m_uploadVideoDataSeqData.seq    = UPLOAD_VIDEO_DATA_SEQ_END;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   同期成功
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnSyncSuccess( void )
  {
    if( m_resultSyncOnlineData == LOAD_RESULT_RUN )
    {
      m_resultSyncOnlineData  = LOAD_RESULT_SUCCESS;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   削除成功
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnDeleteSuccess( void )
  {
    //  データ削除が完了
    if( m_deleteVideoDataSeqData.seq == DELETE_VIDEO_DATA_SEQ_NET_WAIT )
    {
      m_deleteVideoDataSeqData.netRequestResult = DELETE_RESULT_SUCCESS;
    }

    //  アップロードキャンセルに成功
    if( m_uploadCancelVideoDataSeqData.seq  == UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_uploadCancelVideoDataSeqData.seq      = UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_END;
      m_uploadCancelVideoDataSeqData.result   = UPLOAD_CANCEL_RESULT_SUCCESS;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   公開取り消し失敗（公開当日だったら取り消しに失敗する）
  */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnCancelUploadFailed( void )
  {
    //  アップロードキャンセルに失敗
    if( m_uploadCancelVideoDataSeqData.seq  == UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_uploadCancelVideoDataSeqData.seq      = UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_END;
      m_uploadCancelVideoDataSeqData.result   = UPLOAD_CANCEL_RESULT_NOW_UPLOAD_DATA;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   アップロード成功
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnUploadSuccess( void )
  {
    if( m_uploadVideoDataSeqData.seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_uploadVideoDataSeqData.seq    = UPLOAD_VIDEO_DATA_SEQ_SERVER_END;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   バトルビデオが不正
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnUploadInvalid( void )
  {
    if( m_uploadVideoDataSeqData.seq == UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_uploadVideoDataSeqData.result = UPLOAD_RESULT_NG_DATA;
      m_uploadVideoDataSeqData.seq    = UPLOAD_VIDEO_DATA_SEQ_END;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   ダウンロード成功
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnDownloadSuccess( ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pDownloadData , u32 playCount , ExtSavedata::BattleRecorderSaveData::REC_DATE* pDate )
  {
    GFL_ASSERT( pDownloadData );

    //  シリアルNOからダウンロード中か
    if( m_createSerirlaNoVideoDataSeqData.seq == CREATE_DATA_SERIAL_NO_SEQ_DOWN_LOAD )
    {
      //  ダウンロードデータ作成
        // NMCat[4279] バージョンは完全一致で確認するように変更します k.ohno
      if( pDownloadData->header.server_version == BTL_NET_SERVER_VERSION )
      {
        //  サーバーバージョンOK
        m_createSerirlaNoVideoDataSeqData.result  = SERIAL_NO_LOAD_RESULT_SUCCESS;

        // 登録日時と再生数を取得する必要がある。
        m_serialNoData.sdCardData.playCount   = playCount;
        if( pDate != NULL )
        {
          m_serialNoData.sdCardData.registDate  = *pDate;
        }

        //  シリアル用のサーバーデータ作成
        _CreateVideoDataFromUploadData( &m_serialNoData, m_pHeap, pDownloadData );
      }
      else
      {
        //  サーバーバージョンが足りない
        m_createSerirlaNoVideoDataSeqData.result  = SERIAL_NO_LOAD_RESULT_ERROR_SERVER_VERSION;
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   ダウンロードしたいファイルが見つからない
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnDownloadNotFound( void )
  {
    //  シリアルNOからダウンロード中か
    if( m_createSerirlaNoVideoDataSeqData.seq == CREATE_DATA_SERIAL_NO_SEQ_DOWN_LOAD )
    {
      m_createSerirlaNoVideoDataSeqData.result  = SERIAL_NO_LOAD_RESULT_NOT_FOUND;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   再生回数インクリメント成功
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnIncPlayCountSuccess( void )
  {
    if( m_incPlayCountVideoDataSeqData.seq == SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_incPlayCountVideoDataSeqData.result = INC_PLAY_COUNT_RESULT_SUCCESS;
      m_incPlayCountVideoDataSeqData.seq    = INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_END;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   再生完走通知成功
    */
  //--------------------------------------------------------------------------------------------
  void VideoDataManager::OnPlayCompleteSuccess( void )
  {
    if( m_sendPlayComplateVideoDataSeqData.seq == SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_WAIT )
    {
      m_sendPlayComplateVideoDataSeqData.seq = SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_END;
      m_sendPlayComplateVideoDataSeqData.result = SEND_PLAY_COMPLATE_RESULT_SUCCESS;
    }
  }

  //  ビデオデータのバッファ作成
  void VideoDataManager::_CreateDataBuffer( VIDEO_DATA* pOutputVideoData, gfl2::heap::HeapBase* pHeap )
  {
    if( pOutputVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }
    else
    if( pHeap == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

#if PM_DEBUG
    u32 size = 0;
#endif

    VIDEO_DATA* pCurrentVideoData = pOutputVideoData;
    //  タイトル名
    if( pCurrentVideoData->pTitleName01 == NULL )
    {
      pCurrentVideoData->pTitleName01 = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( 18 * 3, m_pHeap );
#if PM_DEBUG
      size  += sizeof( gfl2::str::StrBuf );
      size  += sizeof( gfl2::str::STRCODE ) * pCurrentVideoData->pTitleName01->GetSize();

      UCHIDA_PRINT( "videodata01 size %d\n", size );
#endif
    }

    if( pCurrentVideoData->pTitleName02 == NULL )
    {
      pCurrentVideoData->pTitleName02 = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( 18 * 3, m_pHeap );

#if PM_DEBUG
      size  += sizeof( gfl2::str::StrBuf );
      size  += sizeof( gfl2::str::STRCODE ) * pCurrentVideoData->pTitleName02->GetSize();

      UCHIDA_PRINT( "videodata02 size %d\n", size );
#endif
    }

    //  プレイヤーデータ
    u32 player_num  = GFL_NELEMS( pCurrentVideoData->aPlayerData );
    for( u32 player_idx = 0; player_idx < player_num; ++player_idx )
    {
      PLAYER_DATA*  pPlayerData = &pCurrentVideoData->aPlayerData[ player_idx ];
      if( pPlayerData->pName == NULL )
      {
        pPlayerData->pName      = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( 6 * 3, m_pHeap );
#if PM_DEBUG
        size  += sizeof( gfl2::str::StrBuf );
        size  += sizeof( gfl2::str::STRCODE ) * pPlayerData->pName->GetSize();

        UCHIDA_PRINT( "videodata03 size %d\n", size );
#endif
      }
    }

#if PM_DEBUG
    UCHIDA_PRINT( "pCurrentVideoData->sdCardData size %d\n", sizeof( pCurrentVideoData->sdCardData ) );
    UCHIDA_PRINT( "pCurrentVideoData->sdCardData.upload size %d\n", sizeof( pCurrentVideoData->sdCardData.upload ) );
    UCHIDA_PRINT( "pCurrentVideoData->sdCardData.upload.header size %d\n", sizeof( pCurrentVideoData->sdCardData.upload.header ) );
    UCHIDA_PRINT( "pCurrentVideoData->sdCardData.upload.body size %d\n", sizeof( pCurrentVideoData->sdCardData.upload.body ) );

    UCHIDA_PRINT( "videodata size %d\n", sizeof( VIDEO_DATA ) + size );
#endif
  }

  //  ビデオデータのバッファ削除
  void VideoDataManager::_DeleteDataBuffer( VIDEO_DATA* pOutputVideoData )
  {
    if( pOutputVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    VIDEO_DATA* pCurrentVideoData = pOutputVideoData;

    //  タイトル名
    GFL_SAFE_DELETE( pCurrentVideoData->pTitleName01 );
    GFL_SAFE_DELETE( pCurrentVideoData->pTitleName02 );

    //  プレイヤーデータ
    u32 player_num  = GFL_NELEMS( pCurrentVideoData->aPlayerData );
    for( u32 player_idx = 0; player_idx < player_num; ++player_idx )
    {
      PLAYER_DATA*  pPlayerData = &pCurrentVideoData->aPlayerData[ player_idx ];
      GFL_SAFE_DELETE( pPlayerData->pName );
    }
  }

  //  タイトル名更新
  void VideoDataManager::_CreateTitleName( VIDEO_DATA* pOutputVideoData, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
  {
    if( pOutputVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    VIDEO_DATA* pData = pOutputVideoData;
    pData->pTitleName01->SetStr( L"" );
    pData->pTitleName02->SetStr( L"" );

    //  番号からタイトル名を設定

    Regulation::BATTLE_CATEGORY category  = (Regulation::BATTLE_CATEGORY)pData->sdCardData.upload.header.category;
    Regulation::BATTLE_RULE     rule      = (Regulation::BATTLE_RULE)pData->sdCardData.upload.header.rule;

    pData->selectMsgId  = msg_bvp_sys_low_03;
    if( category == Regulation::BATTLE_CATEGORY_NET_BTL )
    {
      //  通信対戦
      pMsgData->GetString( msg_bvp_battle_title_01_01, *pData->pTitleName01 );
      pData->selectMsgId  = msg_bvp_sys_low_03;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND )
    {
      //  バトルスポットフリーバトル伝説あり
      pMsgData->GetString( msg_bvp_battle_title_01_02, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_01, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_04;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_SPOT_FREE )
    {
      //  バトルスポットフリーバトル伝説なし
      pMsgData->GetString( msg_bvp_battle_title_01_02, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_01, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_04;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_SPOT_RATE )
    {
      //  バトルスポットレーティングバトル
      pMsgData->GetString( msg_bvp_battle_title_01_02, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_02, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_04;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_SPOT_SP )
    {
      //  バトルスポットスペシャルバトル
      pMsgData->GetString( msg_bvp_battle_title_01_02, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_03, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_04;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_TREE_NORMAL )
    {
      //  バトルツリーノーマル
      pMsgData->GetString( msg_bvp_battle_title_01_03, *pData->pTitleName01 );

      if( rule == Regulation::BATTLE_RULE_SINGLE )
      {
        //  シングル
        pMsgData->GetString( msg_bvp_battle_title_02_04, *pData->pTitleName02 );
      }
      else
      if( rule == Regulation::BATTLE_RULE_DOUBLE )
      {
        //  ダブル
        pMsgData->GetString( msg_bvp_battle_title_02_05, *pData->pTitleName02 );
      }
      else
      if( rule == Regulation::BATTLE_RULE_MULTI )
      {
        //  マルチ
        pMsgData->GetString( msg_bvp_battle_title_02_06, *pData->pTitleName02 );
      }
      else
      {
        GFL_ASSERT( 0 );
      }

      pData->selectMsgId  = msg_bvp_sys_low_05;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_TREE_SUPER )
    {
      //  バトルツリースーパー
      pMsgData->GetString( msg_bvp_battle_title_01_03, *pData->pTitleName01 );

      if( rule == Regulation::BATTLE_RULE_SINGLE )
      {
        //  シングル
        pMsgData->GetString( msg_bvp_battle_title_02_07, *pData->pTitleName02 );
      }
      else
      if( rule == Regulation::BATTLE_RULE_DOUBLE )
      {
        //  ダブル
        pMsgData->GetString( msg_bvp_battle_title_02_08, *pData->pTitleName02 );
      }
      else
      if( rule == Regulation::BATTLE_RULE_MULTI )
      {
        //  マルチ
        pMsgData->GetString( msg_bvp_battle_title_02_09, *pData->pTitleName02 );
      }
      else
      {
        GFL_ASSERT( 0 );
      }

      pData->selectMsgId  = msg_bvp_sys_low_05;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL )
    {
      //  バトルロイヤル施設ノーマルランク
      pMsgData->GetString( msg_bvp_battle_title_01_04, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_10, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_06;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_SUPER )
    {
      //  バトルロイヤル施設スーパーランク
      pMsgData->GetString( msg_bvp_battle_title_01_04, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_11, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_06;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_HYPER )
    {
      //  バトルロイヤル施設ハイパーランク
      pMsgData->GetString( msg_bvp_battle_title_01_04, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_12, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_06;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_MASTER )
    {
      //  バトルロイヤル施設マスターランク
      pMsgData->GetString( msg_bvp_battle_title_01_04, *pData->pTitleName01 );
      pMsgData->GetString( msg_bvp_battle_title_02_13, *pData->pTitleName02 );

      pData->selectMsgId  = msg_bvp_sys_low_06;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_LIVE_CUP )
    {
      //  ライブ大会

      //  大会1
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname1 );
        pMsgData->GetString( msg_bvp_battle_title_01_05, *pData->pTitleName01 );
        pWordSet->Expand( pData->pTitleName01, m_pTmpStrBuf );
      }

      //  大会2
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname2 );
        pMsgData->GetString( msg_bvp_battle_title_02_14, *pData->pTitleName02 );
        pWordSet->Expand( pData->pTitleName02, m_pTmpStrBuf );
      }

      pData->selectMsgId  = msg_bvp_sys_low_08;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_NET_CUP )
    {
      //  インターネット大会
      //  大会1
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname1 );
        pMsgData->GetString( msg_bvp_battle_title_01_05, *pData->pTitleName01 );
        pWordSet->Expand( pData->pTitleName01, m_pTmpStrBuf );
      }

      //  大会2
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname2 );
        pMsgData->GetString( msg_bvp_battle_title_02_14, *pData->pTitleName02 );
        pWordSet->Expand( pData->pTitleName02, m_pTmpStrBuf );
      }

      pData->selectMsgId  = msg_bvp_sys_low_07;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_GROUP_CUP )
    {
      //  なかま大会

      //  大会1
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname1 );
        pMsgData->GetString( msg_bvp_battle_title_01_05, *pData->pTitleName01 );
        pWordSet->Expand( pData->pTitleName01, m_pTmpStrBuf );
      }

      //  大会2
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname2 );
        pMsgData->GetString( msg_bvp_battle_title_02_14, *pData->pTitleName02 );
        pWordSet->Expand( pData->pTitleName02, m_pTmpStrBuf );
      }

      pData->selectMsgId  = msg_bvp_sys_low_09;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_DELIVERY )
    {
      //  配信レギュレーション

      //  通信対戦
      {
        pMsgData->GetString( msg_bvp_battle_title_01_01, *pData->pTitleName01 );
      }

      //  大会2
      {
        m_pTmpStrBuf->SetStr( pData->sdCardData.upload.header.cupname2 );
        pMsgData->GetString( msg_bvp_battle_title_02_14, *pData->pTitleName02 );
        pWordSet->Expand( pData->pTitleName02, m_pTmpStrBuf );
      }

      pData->selectMsgId  = msg_bvp_sys_low_03;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_WCS )
    {
      //! ランダムマッチ[レーティング]WCSルール

      //  バトルスポット
      {
        pMsgData->GetString( msg_bvp_battle_title_01_02, *pData->pTitleName01 );
      }

      //  WCSルール
      {
        pMsgData->GetString( msg_bvp_battle_title_02_15, *pData->pTitleName02 );
      }

      pData->selectMsgId  = msg_bvp_sys_low_04;
    }
    else
    if( category == Regulation::BATTLE_CATEGORY_BATTLE_FES )
    {
      //! バトルフェス
      pMsgData->GetString( msg_bvp_battle_title_01_06, *pData->pTitleName01 );
//      pMsgData->GetString( msg_bvp_battle_title_01_03, *pData->pTitleName02 );
      pData->selectMsgId  = msg_bvp_battle_title_01_06;
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //  ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATAデータからVIDEO_DATAの中身を出力
  void VideoDataManager::_CreateVideoDataFromUploadData( VIDEO_DATA* pOutputVideoData, gfl2::heap::HeapBase* pHeap, const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pUploadData )
  {
    if( pOutputVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    pOutputVideoData->sdCardData.upload  = *pUploadData;

    _CreatePlayerData( pOutputVideoData, &pUploadData->header );

    //  ローカル内で必要データを更新
    {
      //  登録データ日時更新
      {
        pOutputVideoData->UpdateRegistDate();
      }

      //  作成日時更新
      {
        pOutputVideoData->UpdateCreateDate();
      }

    }
  }

  //  プレイヤーのデータ構築
  void VideoDataManager::_CreatePlayerData( VIDEO_DATA* pOutputVideoData, const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_HEADER* pUploadHeaderData )
  {
    if( pOutputVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( pUploadHeaderData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

     // 左側のプレイヤー
    {
      {
        PLAYER_DATA*  pDataBuf = &pOutputVideoData->aPlayerData[ BTL_CLIENT_PLAYER ];
        pDataBuf->pName->SetStr( pUploadHeaderData->name[ BTL_CLIENT_PLAYER ] );
        pDataBuf->iconData  = pUploadHeaderData->pssIcon[ BTL_CLIENT_PLAYER ];
        pDataBuf->trainerId = pUploadHeaderData->trainer_id[ BTL_CLIENT_PLAYER ];
      }
      
      {
        PLAYER_DATA*  pDataBuf = &pOutputVideoData->aPlayerData[ BTL_CLIENT_PARTNER ];
        pDataBuf->pName->SetStr( pUploadHeaderData->name[ BTL_CLIENT_PARTNER ] );
        pDataBuf->iconData  = pUploadHeaderData->pssIcon[ BTL_CLIENT_PARTNER ];
        pDataBuf->trainerId = pUploadHeaderData->trainer_id[ BTL_CLIENT_PARTNER ];
      }
    }

    //  右側のプレイヤー
    {
      {
        PLAYER_DATA*  pDataBuf  = &pOutputVideoData->aPlayerData[ BTL_CLIENT_ENEMY1 ];
        pDataBuf->pName->SetStr( pUploadHeaderData->name[ BTL_CLIENT_ENEMY1] );
        pDataBuf->iconData  = pUploadHeaderData->pssIcon[ BTL_CLIENT_ENEMY1 ];
        pDataBuf->trainerId = pUploadHeaderData->trainer_id[ BTL_CLIENT_ENEMY1 ];
      }

      {
        PLAYER_DATA*  pDataBuf  = &pOutputVideoData->aPlayerData[ BTL_CLIENT_ENEMY2 ];
        pDataBuf->pName->SetStr( pUploadHeaderData->name[ BTL_CLIENT_ENEMY2] );
        pDataBuf->iconData  = pUploadHeaderData->pssIcon[ BTL_CLIENT_ENEMY2 ];
        pDataBuf->trainerId = pUploadHeaderData->trainer_id[ BTL_CLIENT_ENEMY2 ];
      }
    }

    //  きせかえチェック(パラメータすべてに-1が入っている場合はアイコンデータがないと判断する)
    {
      u32 loopNum = GFL_NELEMS( pOutputVideoData->aPlayerData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        poke_3d::model::DressUpParam dressUpParam;
        System::DressUpParamSaveData::ConvertToDressUpParam( &dressUpParam, pOutputVideoData->aPlayerData[ i ].iconData.dressUpParam );

        {
          u32 bodyLoopNum = GFL_NELEMS( dressUpParam.bodyParams );
          for( u32 bodyIdx = 0; bodyIdx < bodyLoopNum; ++bodyIdx )
          {
            if( dressUpParam.bodyParams[ bodyIdx ] != -1 )
            {
              pOutputVideoData->aPlayerData[ i ].bUseIconData = true;
              break;
            }
          }
        }

        if( pOutputVideoData->aPlayerData[ i ].bUseIconData == false )
        {
          u32 itemLoopNum = GFL_NELEMS( dressUpParam.itemParams );
          for( u32 itemIdx = 0; itemIdx < itemLoopNum; ++itemIdx )
          {
            if( dressUpParam.itemParams[ itemIdx ] != -1 )
            {
              pOutputVideoData->aPlayerData[ i ].bUseIconData = true;
              break;
            }
          }
        }
      }
    }
  }

  //  リスト整理(削除フラグがついてるのを削除)
  void VideoDataManager::_RegulationList()
  {
    VIDEO_DATA*  paTmpBufferData[ VIDEO_DATA_MAX ];
    gfl2::std::MemClear( paTmpBufferData, sizeof( paTmpBufferData ) );

    u32 loopNum = m_listDataNum;
    u32 dataCnt = 0;
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pListData[ i ]->bDelFlg == false )
      {
        paTmpBufferData[ dataCnt ] = m_pListData[ i ];
        ++dataCnt;
      }
    }

    gfl2::std::MemCopy( paTmpBufferData, m_pListData, sizeof( paTmpBufferData ) );
    m_listDataNum = dataCnt;
  }

#if PM_DEBUG
  //  テスト用のダミーデータ作成
  void VideoDataManager::_CreateDummyData( gfl2::heap::HeapBase* pHeap )
  {
    static ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA aDummyData[ 10 ];
    /*
      u8                category;   ///<対戦施設種別 regulation::Regulation::CATEGORY_TYPE
      u8                rule;       ///<対戦モード   regulation::Regulation::BATTLE_TYPE
      u16               magic_key;  ///<マジックキー

      bool              sakasaBattle; ///<さかさバトルかどうか（true:さかさバトル）

      gfl2::str::STRCODE cupname1[ Regulation::WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN ];  ///<大会名１
      gfl2::str::STRCODE cupname2[ Regulation::WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN ];  ///<大会名２
      Savedata::MyStatus::ICON_DATA   pssIcon[ BTL_CLIENT_NUM ];                              ///<録画用プレイヤーICONデータ
      gfl2::str::STRCODE name[ BTL_CLIENT_NUM ][ pml::PERSON_NAME_BUFFER_SIZE ]; ///<プレーヤー名
      u16               mons_no[ pml::PokeParty::MAX_MEMBERS * 2 ];             ///<モンスターナンバー
      u8                form_no[ pml::PokeParty::MAX_MEMBERS * 2 ];             ///<フォルムナンバー
      u8                gender[ pml::PokeParty::MAX_MEMBERS * 2 ];              ///<性別
      bool              rare[ pml::PokeParty::MAX_MEMBERS * 2 ];                ///<レアフラグ

      u16               battle_counter;       ///<連勝数
      u16               end_turn_num;         ///<バトルビデオが終了した際の経過ターン数
      u8                server_version;       ///<バトルサーババージョン
      u8                background;           ///<背景タイプ regulation::Regulation::BACKGROUND_TYPE

      u8                patch_version[3];     ///<パッチのバージョン
      u8                battle_result1;       ///<対戦結果１　０＝勝利、１＝敗北、２＝引き分け
      u8                battle_result2;       ///<対戦結果２　０＝全てのポケモンがひんし、１＝制限時間切れ、２＝降参


      u8                padding_work[ 5 ];   ///<予備

      u32               crc;                  ///<CRC
    */
    u32 loopNum = GFL_NELEMS( aDummyData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pUploadData    = &aDummyData[ i ];

      //  プレイヤーアイコンはデフォルト
      {
        for( u32 player_i = 0; player_i < BTL_CLIENT_NUM; ++player_i )
        {
          app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( 0, &pUploadData->header.pssIcon[ player_i ] );
        }
      }

      ExtSavedata::BattleRecorderSaveData::BATTLE_REC_HEADER* pSetData  = &pUploadData->header;

      pSetData->battle_result1  = BTL_RECORD_RESULT_1_WIN;
      pSetData->battle_result2  = BTL_RECORD_RESULT_2_DEAD;

      pSetData->category  = Regulation::BATTLE_CATEGORY_NET_BTL;
      pSetData->rule      = Regulation::BATTLE_RULE_SINGLE;

      if( i == 0 )
      {
        //  味方
        for( u32 j = 0; j < pml::PokeParty::MAX_MEMBERS; ++j )
        {
          ///<モンスターナンバー
          if( j == 0 )
          {
            pSetData->mons_no[ j ]  = MONSNO_HUSIGIDANE;
          }
          else
          if( j == ( pml::PokeParty::MAX_MEMBERS - 1 ) )
          {
            pSetData->mons_no[ j ]  = MONSNO_KOKUUN;
          }
          else
          {
            pSetData->mons_no[ j ]  = MONSNO_RIZAADO;
          }

          ///<フォルムナンバー
          pSetData->form_no[ j ]  = 0;
          ///<性別
          pSetData->gender[ j ] = 0;
          ///<レアフラグ
          pSetData->rare[ j ] = true;
        }

        for( u32 j = pml::PokeParty::MAX_MEMBERS; j < pml::PokeParty::MAX_MEMBERS * 2; ++j )
        {
          ///<モンスターナンバー
          if( j == pml::PokeParty::MAX_MEMBERS )
          {
            pSetData->mons_no[ j ]  = MONSNO_SANDO;
          }
          else
          if( j == (  pml::PokeParty::MAX_MEMBERS * 2 -1 ) )
          {
            pSetData->mons_no[ j ]  = MONSNO_PURIN;
          }
          else
          {
            pSetData->mons_no[ j ]  = MONSNO_NIDORIINO;
          }

          ///<フォルムナンバー
          pSetData->form_no[ j ]  = 0;
          ///<性別
          pSetData->gender[ j ] = 0;
          ///<レアフラグ
          pSetData->rare[ j ] = true;
        }

        {
          for( u32 name_idx = 0; name_idx < BTL_CLIENT_NUM; ++name_idx )
          {
            if( name_idx == 0 )
            {
              m_pTmpStrBuf->SetStr( L"uchida" );
            }
            else
            if( name_idx == 1 )
            {
              m_pTmpStrBuf->SetStr( L"zako" );
            }
            else
            if( name_idx == 2 )
            {
              m_pTmpStrBuf->SetStr( L"tako" );
            }
            else
            {
              m_pTmpStrBuf->SetStr( L"lost" );
            }

            m_pTmpStrBuf->PutStr( pSetData->name[ name_idx ], pml::PERSON_NAME_BUFFER_SIZE );
          }
        }
      }
      else
      if( i == 1 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND;
        pSetData->rule      = Regulation::BATTLE_RULE_MULTI;
        pSetData->battle_result1  = BTL_RECORD_RESULT_2_DEAD;
        pSetData->battle_result2  = BTL_RECORD_RESULT_2_TIMEOVER;

        {
          for( u32 name_idx = 0; name_idx < BTL_CLIENT_NUM; ++name_idx )
          {
            if( name_idx == 0 )
            {
              m_pTmpStrBuf->SetStr( L"takashi" );
            }
            else
            if( name_idx == 1 )
            {
              m_pTmpStrBuf->SetStr( L"yamada" );
            }
            else
            if( name_idx == 2 )
            {
              m_pTmpStrBuf->SetStr( L"to" );
            }
            else
            {
              m_pTmpStrBuf->SetStr( L"aoi" );
            }

            m_pTmpStrBuf->PutStr( pSetData->name[ name_idx ], pml::PERSON_NAME_BUFFER_SIZE );
          }
        }

        //  味方
        for( u32 j = 0; j < pml::PokeParty::MAX_MEMBERS; ++j )
        {
          ///<モンスターナンバー
          if( j == 0 )
          {
            pSetData->mons_no[ j ]  = MONSNO_ZENIGAME;
          }
          else
          if( j == ( pml::PokeParty::MAX_MEMBERS - 1 ) )
          {
            pSetData->mons_no[ j ]  = MONSNO_ONISUZUME;
          }
          else
          {
            pSetData->mons_no[ j ]  = MONSNO_NIDORAN_F;
          }

          ///<フォルムナンバー
          pSetData->form_no[ j ]  = 0;
          ///<性別
          pSetData->gender[ j ] = 0;
          ///<レアフラグ
          pSetData->rare[ j ] = false;
        }

        for( u32 j = pml::PokeParty::MAX_MEMBERS; j < pml::PokeParty::MAX_MEMBERS * 2; ++j )
        {
          ///<モンスターナンバー
          if( j == pml::PokeParty::MAX_MEMBERS )
          {
            pSetData->mons_no[ j ]  = MONSNO_ROKON;
          }
          else
          if( j == (  pml::PokeParty::MAX_MEMBERS * 2 -1 ) )
          {
            pSetData->mons_no[ j ]  = MONSNO_NIDORAN_M;
          }
          else
          {
            pSetData->mons_no[ j ]  = MONSNO_PARASU;
          }

          ///<フォルムナンバー
          pSetData->form_no[ j ]  = 0;
          ///<性別
          pSetData->gender[ j ] = 0;
          ///<レアフラグ
          pSetData->rare[ j ] = true;
        }
      }
      else
      if( i == 2 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_NET_CUP;
        pSetData->rule      = Regulation::BATTLE_RULE_DOUBLE;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
        pSetData->battle_result2  = BTL_RECORD_RESULT_2_DEAD;

        m_pTmpStrBuf->SetStr( L"test" );
        m_pTmpStrBuf->PutStr( pSetData->cupname1, 4 );

        m_pTmpStrBuf->SetStr( L"test2" );
        m_pTmpStrBuf->PutStr( pSetData->cupname2, 5 );
      }
      else
      if( i == 3 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND;
        pSetData->rule      = Regulation::BATTLE_RULE_ROYAL;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
      }
      else
      if( i == 4 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL;
        pSetData->rule      = Regulation::BATTLE_RULE_SINGLE;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
      }
      else
      if( i == 5 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_TREE_SUPER;
        pSetData->rule      = Regulation::BATTLE_RULE_SINGLE;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
      }
      else
      if( i == 6 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_TREE_NORMAL;
        pSetData->rule      = Regulation::BATTLE_RULE_SINGLE;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
      }
      else
      if( i == 7 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_TREE_NORMAL;
        pSetData->rule      = Regulation::BATTLE_RULE_MULTI;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
      }
      else
      if( i == 8 )
      {
        pSetData->category  = Regulation::BATTLE_CATEGORY_NET_BTL;
        pSetData->rule      = Regulation::BATTLE_RULE_ROYAL;
        pSetData->battle_result1  = BTL_RECORD_RESULT_1_DRAW;
      }


      pSetData->sakasaBattle  = true;
      pSetData->end_turn_num    = 3;
    }

    m_listDataNum = GFL_NELEMS( aDummyData );

    for( u32 i = 0; i < m_listDataNum; ++i )
    {
      VIDEO_DATA* pCurrentVideoData = &m_aBufferData[ i ];
      m_pListData[ i ]  = pCurrentVideoData;

      if( i == 0 )
      {
        m_pListData[ i ]->sdCardData.upload_flag  = false;
      }
      else
      {
        m_pListData[ i ]->registDate.SetYear( 2015 );
        m_pListData[ i ]->registDate.SetMonth( 11 );
        m_pListData[ i ]->registDate.SetDay( 15 );

        m_pListData[ i ]->sdCardData.upload_flag  = true;
        m_pListData[ i ]->sdCardData.dataID  = 1234324;
        m_pListData[ i ]->sdCardData.playCount = 34243;
      }

      if( i == 2 )
      {
        m_pListData[ i ]->bErrorVersionFlg  = true;
      }

      _CreateVideoDataFromUploadData( m_pListData[ i ], pHeap, &aDummyData[ i ] );
    }

    _CreateVideoDataFromUploadData( &m_serialNoData, pHeap, &aDummyData[ 0 ] );
  }
#endif

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetAppLib)
