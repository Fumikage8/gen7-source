#if defined(GF_PLATFORM_CTR)
// ============================================================================
/*
 * @file GameSyncSaveDataSyncRequestSequence.cpp
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncRequestSequence.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/http/gflnet2_HttpSystem.h>
#include <System/include/HeapDefine.h>
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>

#include <Savedata/include/GameSyncSave.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/MysteryGiftSave.h>

#include <gflnet2/include/util/gflnet2_Base64Utility.h>
#include  <niji_conv_header/message/msg_mystery_card.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


GameSyncSaveDataSyncRequestSequence::GameSyncSaveDataSyncRequestSequence()
: m_pWork( NULL )
, m_pGameSyncSaveDataSyncResponseListener( NULL )
, m_pPglConnector( NULL )
, m_pPglBase64Savedata(NULL)   //!< BASE64エンコード済みセーブデータ
, m_pPglBase64PlayerIcon(NULL) //!< BASE64エンコード済みトレイナーアイコンデータ
, m_pTex(NULL)
, m_pTrainerIconUtility(NULL)
, m_pReplacePane(NULL)
, m_isSkipError( false )
// @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
, m_tempIconBuffer(NULL)
{
  //@fix [#5693 ゲームシンク時のセーブが、巻き戻し対応されていない]
  std::memset( &m_GiftSaveCoreDataClone , 0 , sizeof(m_GiftSaveCoreDataClone) );
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  pGameData->GetSaveData()->GetMysteryGiftSave()->SetupClone( m_GiftSaveCoreDataClone );                //  commitSave巻き戻し用Cloneのセット
}


GameSyncSaveDataSyncRequestSequence::~GameSyncSaveDataSyncRequestSequence()
{
  GFL_SAFE_DELETE( m_pTex );
  if( m_pPglConnector )
  {
    m_pPglConnector->RemovePGLConnectorListener();
    m_pPglConnector->Finalize();
  }
  GFL_SAFE_DELETE( m_pPglConnector );
  GflHeapSafeFreeMemory(m_pPglBase64Savedata);
  GflHeapSafeFreeMemory(m_pPglBase64PlayerIcon);
  GflHeapSafeFreeMemory(m_tempIconBuffer);

}

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GameSyncSaveDataSyncRequestSequence::Update()
{
  const char*         authToken   = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->GetAuthenticationToken();
  Savedata::MyStatus* pMystatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();

  u64                 nexUniqueId = pMystatus->GetNexUniqueID();

  switch( GetSequenceNo() )
  {
  case SEQ_INIT:
    {
      if( m_pTrainerIconUtility )
      {
        m_pTex  = GFL_NEW( m_pWork->GetDeviceHeap()->GetLowerHandle() ) app::tool::AppToolTrainerIconTexture( m_pWork->GetDeviceHeap()->GetLowerHandle() );
        m_pTrainerIconUtility->SetIconObjectDataByOutputTexture( 0 , pMystatus->GetIcon() , m_pReplacePane , NULL , false , m_pTex );
      }

      SetSequenceNo( SEQ_ICON_SETUP );
    }
  break;
  case SEQ_ICON_SETUP:
    {
      if( m_pTrainerIconUtility )
      {
        if( m_pTrainerIconUtility->IsEndIconObjectData( 0 ) == false )
        {
          break;
        }
      }
      // @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
      u32 pglPlayerIconSize = WIDTH_AND_HEIGHT*WIDTH_AND_HEIGHT*2;
      m_tempIconBuffer  = GflHeapAllocMemoryAlign( m_pWork->GetDeviceHeap()->GetLowerHandle(), pglPlayerIconSize  ,128  );

      //アイコンフォーマット変換
      if( m_pTex )
      {
        u32 width  = WIDTH_AND_HEIGHT;
        u32 height = WIDTH_AND_HEIGHT;

        nn::gr::CommandBufferJumpHelper *jh;
        jh = gfl2::gfx::GFGL::GetJumpHelper();
        gfl2::gfx::ctr::CTRCommandList *p_cmd = gfl2::gfx::ctr::CTRGL::GetCurrentCommandList();
        gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperBefore( p_cmd, jh );

        // @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
        p_cmd->AddB2LTransferCommand( m_pTex->GetTexData(), 128, 128, GL_RGBA8_OES,
          m_tempIconBuffer,                         64, 64, GL_RGB5_A1,
          NN_GX_ANTIALIASE_2x2, GL_FALSE, 8 );

        gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperAfter( p_cmd, jh );
      }

      SetSequenceNo( SEQ_ICON_SETUP_WAIT );
    }
    break;
  case SEQ_ICON_SETUP_WAIT://@fix[#5650 ゲームシンク：プレイヤーアイコン変換待ちが１フレーム足りない]
    SetSequenceNo( SEQ_GAMESYNC_INIT );
    break;
  case SEQ_GAMESYNC_INIT:
    {
      //----------ゲームシンクセットアップ
      gflnet2::http::HttpSystem::Finalize();

      u32 pglPlayerIconSize = WIDTH_AND_HEIGHT*WIDTH_AND_HEIGHT*2;
      u32 pglSavedataSize   = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GerSaveDataBufferSize( true );

      u32 playerIconEncodeSize = gflnet2::util::Base64Utility::GetNeedEncodeBufferSize( pglPlayerIconSize );
      u32 savedataEncodeSize   = gflnet2::util::Base64Utility::GetNeedEncodeBufferSize( pglSavedataSize );

      m_pPglConnector = GFL_NEW( m_pWork->GetDeviceHeap()->GetLowerHandle() ) NetLib::Pgl::PGLConnector();
      m_pPglBase64Savedata   = GflHeapAllocMemory( m_pWork->GetDeviceHeap()->GetLowerHandle(), savedataEncodeSize   );
      m_pPglBase64PlayerIcon = GflHeapAllocMemory( m_pWork->GetDeviceHeap()->GetLowerHandle(), playerIconEncodeSize );
      void* savedataBuffer  = GflHeapAllocMemory( m_pWork->GetDeviceHeap()->GetLowerHandle(), pglSavedataSize    );


      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->WriteSaveDataToBuffer( (u8*)savedataBuffer, m_pWork->GetDeviceHeap()->GetLowerHandle(), true );


      gflnet2::util::Base64Utility::Encode( m_pPglBase64Savedata,   savedataBuffer, pglSavedataSize     );
      gflnet2::util::Base64Utility::Encode( m_pPglBase64PlayerIcon, m_tempIconBuffer,     pglPlayerIconSize );

      GflHeapSafeFreeMemory(savedataBuffer);
      GflHeapSafeFreeMemory(m_tempIconBuffer);

      gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );
      nn::Result result = gflnet2::http::HttpSystem::Initialize( heap->GetLowerHandle(), GAMESYNC_BUFFSIZE );//HTTPシステムをゲームシンク用に初期化
      if( result.IsFailure() )
      {
        GFL_ASSERT( 0 );//@fix
        GFL_PRINT("GameSyncProc::_CreateSaveData : Initialize HttpSystem Failed\n");
        if( m_pGameSyncSaveDataSyncResponseListener )
        {
          m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError(0);
        }
        Cancel();
        break;
      }

      //受け取れるアイテム数
      Savedata::MysteryGiftSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetMysteryGiftSave();

      u32 emptyNum = pSave->GetGiftDataVacancies();

      u32 itemSlotFreeNum = emptyNum * GIFT_ITEM_SLOT_NUM;
      //------------------------

      if( m_pPglConnector->Initialize( m_pWork->GetDeviceHeap() , nexUniqueId, authToken, static_cast<nn::cfg::CTR::CfgRegionCode>( pMystatus->GetRegionCode() ) ) )
      {
        m_pPglConnector->SetPGLConnectorListener( this );

        //@fix[#5583 【ゲームシンク】インターネット接続時のゲームシンクで、プレイヤーアイコンの画像が正しく送信されない]
        if( m_pTrainerIconUtility == NULL )
        {//テクスチャ生成できないときはサイズ0にしておく
          pglPlayerIconSize = 0;
        }

        bool retSync = m_pPglConnector->SyncFullSaveData( (const char*)m_pPglBase64Savedata,   pglSavedataSize,
                                                        (const char*)m_pPglBase64PlayerIcon, pglPlayerIconSize,
                                                        itemSlotFreeNum,
                                                        NetLib::NijiNetworkSystem::IsParentalMode() , NULL );
        if( retSync )
        {
          SetSequenceNo( SEQ_WAIT );
        }
        else
        {//エラー
          GFL_PRINT("GameSGameSyncConfirmPlayStatusRequestSequenceyncProc : GetPlayStatus Failed -----------------------------\n");
          GFL_ASSERT( 0 );//@fix
          if( m_pGameSyncSaveDataSyncResponseListener )
          {
            m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError(0);
          }
          Cancel();
        }
      }
      else
      {
        GFL_ASSERT( 0 );//@fix
        if( m_pGameSyncSaveDataSyncResponseListener )
        {
          m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError(0);
        }
        Cancel();
      }
    }
  break;
  case SEQ_WAIT:
    {
    }
  break;
  case SEQ_SAVE_WAIT:
    {
      //@fix [#5693 ゲームシンク時のセーブが、巻き戻し対応されていない]
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        break;
      }

      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( pGameData->FirstSaveMain( &saveResult ) )
      {
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          if( m_pPglConnector->CommitGameSync() )
          {
            GFL_PRINT("LastSaveStart\n");
            pGameData->LastSaveStart( m_pWork->GetDeviceHeap()->GetLowerHandle() );
          }
          else
          {
            GFL_ASSERT(0);//@fix
            if( m_pGameSyncSaveDataSyncResponseListener )
            {
              m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError(0);
            }
            Cancel();
          }
        }
        else
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_ASSERT(0);//@fix
          if( m_pGameSyncSaveDataSyncResponseListener )
          {
            m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError(0);
          }
          Cancel();
        }

        SetSequenceNo( SEQ_LAST_SAVE_WAIT );
      }
    }
  break;
  case SEQ_LAST_SAVE_WAIT:
    {
      if( m_pPglConnector )
      {
        if( m_pPglConnector->IsConnecting() )
        {//通信中
          break;
        }
      }

      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( pGameData->LastSaveMain( &saveResult ) )
      {
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          if( m_pGameSyncSaveDataSyncResponseListener )
          {
            m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncSuccess();
          }
        }
        else
        if( saveResult == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_ASSERT( 0 );//@fix
          if( m_pGameSyncSaveDataSyncResponseListener )
          {
            m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError(0);
          }
        }

        SetSequenceNo( SEQ_END );
        Cancel();
      }
    }
  break;
  };

  return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
}






gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GameSyncSaveDataSyncRequestSequence::UpdateCancel()
{
  if( GetSequenceNo() == SEQ_ICON_SETUP_WAIT )
  {//1フレーム待つ
    SetSequenceNo( SEQ_GAMESYNC_INIT );
    return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
  }

  if( m_pPglConnector )
  {
    if( m_pPglConnector->IsConnecting() ){
      m_pPglConnector->CancelConnecting(); //キャンセル命令を発行
      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;//終了待ち
    }

    m_pPglConnector->RemovePGLConnectorListener();
    m_pPglConnector->Finalize();
    GFL_SAFE_DELETE( m_pPglConnector );

    return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;//NexのDispatchのために一回抜ける
  }

  switch( GetSequenceNo() )
  {
  case SEQ_SAVE_WAIT:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( pGameData->FirstSaveMain( &saveResult ) )
      {
        if( saveResult != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE )
        {
          pGameData->FirstSaveCancel();
          //@fix [#5693 ゲームシンク時のセーブが、巻き戻し対応されていない]
          pGameData->GetSaveData()->GetMysteryGiftSave()->WriteBackClone( &m_GiftSaveCoreDataClone );                //  commitSave巻き戻し

          GFL_PRINT("WriteBackClone\n");

          break;
        }
      }

      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;//終了待ち
    }
  //break;
  case SEQ_LAST_SAVE_WAIT:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      gfl2::fs::FsCtrBackupSystem::Result saveResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

      if( pGameData->LastSaveMain( &saveResult ) )
      {
        if( saveResult != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE )
        {
          break;
        }
      }

      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;//終了待ち
    }
  //break;
  }

  // HttpSystemをもとに戻す
  gflnet2::http::HttpSystem::Finalize();
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
  nn::Result result = gflnet2::http::HttpSystem::Initialize( heap, NetLib::Validation::PokemonValidation::POST_BUFF_SIZE );
  if( result.IsFailure() )
  {
    GFL_ASSERT( 0 );//@fix
    GFL_PRINT("GameSyncProc::_FinalizeGameSync : Initialize HttpSystem Failed\n");
  }

  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}



//------------------------------------------------------------------
/**
* @brief セーブデータ同期通信の完了イベントを検知
*
* @param  isSignIn  連携サイトへの登録有無（true:登録済み）
* @param  itemCount 持ち帰ったアイテム数
* @param  itemInf   持ち帰ったアイテム情報配列(itemCountが0の時はNULL)
* @param  crc       CRC値。持ち帰ったアイテム数が0の場合は0。
*/
//------------------------------------------------------------------
void GameSyncSaveDataSyncRequestSequence::OnPGLSyncSaveDataSuccess( bool isSignIn, u8 itemCount, NetLib::Pgl::ResultItemInfo* itemInf )
{
  GFL_PRINT("GameSyncProc::OnPGLSyncSaveDataSuccess : [%d][%d][0x%x]\n",isSignIn , itemCount , itemInf );

  // 取得アイテムをセーブデータに格納
  if( itemCount > 0 )
  {
    Savedata::MysteryGiftSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetMysteryGiftSave();

    GFL_ASSERT( itemCount <= pSave->GetGiftDataVacancies() * GIFT_ITEM_SLOT_NUM );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* pTemp = GFL_NEW( m_pWork->GetDeviceHeap()->GetLowerHandle() ) Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA();

    u32 loopCount = (itemCount/GIFT_ITEM_SLOT_NUM) + 1;
    u32 addNum = 0;
    for( u8 i = 0; i < loopCount ; i++ )
    {
      std::memset( pTemp , 0 , sizeof(Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA) );
      pTemp->dataBody.eventID = Savedata::MysteryGiftSave::PGL_ITEM_EVENT_ID;//末尾IDにしておく
      pTemp->dataBody.cardMessageID = mystery_card_txt_055_a;
      pTemp->dataBody.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_item;
      pTemp->dataBody.eventName[0] = ' ';//NULLだと空扱いになってしまうのでスペース入れておく
      Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItemdata = reinterpret_cast<Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*>(pTemp->dataBody.body);
      do
      {
        pItemdata->itemNo1 = itemInf[addNum].itemId;
        pItemdata->itemNum1 = itemInf[addNum].itemNum;
        addNum++;

        if( addNum == itemCount )
          break;

        pItemdata->itemNo2 = itemInf[addNum].itemId;
        pItemdata->itemNum2 = itemInf[addNum].itemNum;
        addNum++;

        if( addNum == itemCount )
          break;

        pItemdata->itemNo3 = itemInf[addNum].itemId;
        pItemdata->itemNum3 = itemInf[addNum].itemNum;
        addNum++;

        if( addNum == itemCount )
          break;

        pItemdata->itemNo4 = itemInf[addNum].itemId;
        pItemdata->itemNum4 = itemInf[addNum].itemNum;
        addNum++;

        if( addNum == itemCount )
          break;

        pItemdata->itemNo5 = itemInf[addNum].itemId;
        pItemdata->itemNum5 = itemInf[addNum].itemNum;
        addNum++;

        if( addNum == itemCount )
          break;

        pItemdata->itemNo6 = itemInf[addNum].itemId;
        pItemdata->itemNum6 = itemInf[addNum].itemNum;
        addNum++;

        if( addNum == itemCount )
          break;
      }while(0);

      pSave->PushBackGiftData( pTemp );
    }

    GFL_SAFE_DELETE(pTemp);
  }

  //ゲームシンク済みフラグON
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveDataDirect().m_config.m_isExecuteGameSync = true;

  //ゲームシンクした時間を更新
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->UpdateDateTime();

  {//セーブ開始
    GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

    // 前回の保存日付を退避
    pPlayTime->PushSaveTime();
    pPlayTime->SetSaveTime( pGameData->GetGameTime() );

    // Firstセーブの開始
    GFL_PRINT("FirstSaveStart\n");
    pGameData->FirstSaveStart( m_pWork->GetDeviceHeap()->GetLowerHandle() );
  }

  SetSequenceNo( SEQ_SAVE_WAIT );
}

//------------------------------------------------------------------
/**
* @brief セーブデータ同期確定通信の完了イベントを検知
*
* @param  none
*/
//------------------------------------------------------------------
void GameSyncSaveDataSyncRequestSequence::OnPGLCommitGameSyncSuccess( void )
{
  GFL_PRINT("GameSyncSaveDataSyncRequestSequence::OnPGLCommitGameSyncSuccess : \n");
}


//------------------------------------------------------------------
/**
 * @brief PGL通信のエラーイベントを検知
 *
 * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
 *
 * @param commandType 完了したPGL通信コマンド
 * @param httpStatus サーバから返却されたHTTPステータスコード
 */
//------------------------------------------------------------------
void GameSyncSaveDataSyncRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, httpStatus );

  if( m_isSkipError )
  {
    if( m_pGameSyncSaveDataSyncResponseListener )
    {
      m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError( httpStatus , NetLib::Pgl::E_RET_COMMON_OK );
    }
  }
  else
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultPGL5xx( httpStatus );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    if( m_pGameSyncSaveDataSyncResponseListener )
    {
      m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError( httpStatus );
    }
  }

  Cancel();
}

//------------------------------------------------------------------
/**
 * @brief PGL通信のレスポンスデータのエラーコードを検知
 *
 * @param  commandType  完了したPGL通信コマンド
 * @param  resultCode   PGLサーバから受信したレスポンスエラーコード
 */
//------------------------------------------------------------------
void GameSyncSaveDataSyncRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, resultCode );

  if( m_isSkipError )
  {
    if( m_pGameSyncSaveDataSyncResponseListener )
    {
      m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError( resultCode , resultCode );
    }
  }
  else
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultPGL( resultCode );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    if( m_pGameSyncSaveDataSyncResponseListener )
    {
      m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError( resultCode );
    }
  }

  Cancel();
}

//------------------------------------------------------------------
/**
 * @brief PGL通信のエラーイベントを検知
 *
 * @param  commandType  完了したPGL通信コマンド
 * @param  result       エラー内容
 * @param  errorCode    ネットワークエラーコード
 */
//------------------------------------------------------------------
void GameSyncSaveDataSyncRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  GFL_PRINT("OnPGLConnectError %d \n", errorCode );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  if( m_pGameSyncSaveDataSyncResponseListener )
  {
    m_pGameSyncSaveDataSyncResponseListener->OnSaveDataSyncError( errorCode );
  }

  Cancel();
}


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif