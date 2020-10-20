#if defined(GF_PLATFORM_CTR)

// ============================================================================
/*
 * @file GameSyncCreateSaveDataRequestSequence.cpp
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/GameSync/GameSyncCreateSaveDataRequestSequence.h"
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

#include <gflnet2/include/util/gflnet2_Base64Utility.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


GameSyncCreateSaveDataRequestSequence::GameSyncCreateSaveDataRequestSequence()
: m_pWork( NULL )
, m_pGameSyncCreateSaveDataResponseListener( NULL )
, m_pPglConnector( NULL )
, m_pPglBase64Savedata(NULL)   //!< BASE64エンコード済みセーブデータ
, m_pPglBase64PlayerIcon(NULL) //!< BASE64エンコード済みトレイナーアイコンデータ
, m_pTex(NULL)
, m_pTrainerIconUtility(NULL)
, m_pReplacePane(NULL)
, m_isSkipError(false)
// @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
, m_tempIconBuffer(NULL)
{
}


GameSyncCreateSaveDataRequestSequence::~GameSyncCreateSaveDataRequestSequence()
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

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GameSyncCreateSaveDataRequestSequence::Update()
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
      m_tempIconBuffer  = GflHeapAllocMemoryAlign( m_pWork->GetDeviceHeap()->GetLowerHandle(), pglPlayerIconSize  ,16  );

      //アイコンフォーマット変換
      if( m_pTex )
      {
        u32 width  = WIDTH_AND_HEIGHT;
        u32 height = WIDTH_AND_HEIGHT;

        nn::gr::CommandBufferJumpHelper *jh;
        jh = gfl2::gfx::GFGL::GetJumpHelper();
        gfl2::gfx::ctr::CTRCommandList *p_cmd = gfl2::gfx::ctr::CTRGL::GetCurrentCommandList();
        gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperBefore( p_cmd, jh );

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
        if( m_pGameSyncCreateSaveDataResponseListener )
        {
          m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataError();
        }
        Cancel();
        break;
      }
      //------------------------

      if( m_pPglConnector->Initialize( m_pWork->GetDeviceHeap() , nexUniqueId, authToken, static_cast<nn::cfg::CTR::CfgRegionCode>( pMystatus->GetRegionCode() ) ) )
      {
        m_pPglConnector->SetPGLConnectorListener( this );
        bool retSync = m_pPglConnector->CreateSaveData( (const char*)m_pPglBase64Savedata,   pglSavedataSize,
                                                        (const char*)m_pPglBase64PlayerIcon, pglPlayerIconSize,
                                                        NetLib::NijiNetworkSystem::IsParentalMode() );
        if( retSync )
        {
          SetSequenceNo( SEQ_WAIT );
        }
        else
        {//エラー
          GFL_PRINT("GameSGameSyncConfirmPlayStatusRequestSequenceyncProc : GetPlayStatus Failed -----------------------------\n");
          GFL_ASSERT( 0 );//@fix
          if( m_pGameSyncCreateSaveDataResponseListener )
          {
            m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataError();
          }
          Cancel();
        }
      }
      else
      {
        GFL_ASSERT( 0 );//@fix
        if( m_pGameSyncCreateSaveDataResponseListener )
        {
          m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataError();
        }
        Cancel();
      }
    }
  break;
  case SEQ_WAIT:
    {
    }
  break;
  };

  return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
}




gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GameSyncCreateSaveDataRequestSequence::UpdateCancel()
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
* @brief セーブデータ登録通信の完了イベントを検知
*
* @param  none
*/
//------------------------------------------------------------------
void GameSyncCreateSaveDataRequestSequence::OnPGLCreateSavedataSuccess( void )
{
  GFL_PRINT("GameSyncCreateSaveDataRequestSequence::OnPGLCreateSavedataSuccess\n");
  
  if( m_pGameSyncCreateSaveDataResponseListener )
  {
    m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataSuccess();
  }
  Cancel();
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
void GameSyncCreateSaveDataRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, httpStatus );

  if( m_isSkipError == false )
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultPGL5xx( httpStatus );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
  }

  if( m_pGameSyncCreateSaveDataResponseListener )
  {
    m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataError();
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
void GameSyncCreateSaveDataRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, resultCode );

  if( resultCode == NetLib::Pgl::E_RET_SAVEDATA_EXISTS )
  {//ID作成で作成済みが返ってきた場合成功として扱う
    if( m_pGameSyncCreateSaveDataResponseListener )
    {
      m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataSuccess();
    }
  }
  else
  {
    if( m_isSkipError == false )
    {
      NetLib::Error::NijiNetworkError error;
      error.SetResultPGL( resultCode );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    }

    if( m_pGameSyncCreateSaveDataResponseListener )
    {
      m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataError();
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
void GameSyncCreateSaveDataRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  GFL_PRINT("OnPGLConnectError %d \n", errorCode );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  if( m_pGameSyncCreateSaveDataResponseListener )
  {
    m_pGameSyncCreateSaveDataResponseListener->OnCreateSaveDataError();
  }

  Cancel();
}


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)
#endif