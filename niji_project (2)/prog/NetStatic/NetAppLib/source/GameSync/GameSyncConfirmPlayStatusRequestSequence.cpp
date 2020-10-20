#if defined(GF_PLATFORM_CTR)
// ============================================================================
/*
 * @file GameSyncConfirmPlayStatusRequestSequence.cpp
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusRequestSequence.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include <Savedata/include/GameSyncSave.h>
#include <Savedata/include/MyStatus.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


GameSyncConfirmPlayStatusRequestSequence::GameSyncConfirmPlayStatusRequestSequence() :
  m_pWork( NULL ),
  m_pGameSyncConfirmPlayStatusResponseListener( NULL ),
  m_pPglConnector( NULL ),
  m_mode(0),
  m_isSkipError(false)
{
}


GameSyncConfirmPlayStatusRequestSequence::~GameSyncConfirmPlayStatusRequestSequence()
{
  if( m_pPglConnector )
  {
    m_pPglConnector->RemovePGLConnectorListener();
    m_pPglConnector->Finalize();
  }
  GFL_SAFE_DELETE( m_pPglConnector );
}

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GameSyncConfirmPlayStatusRequestSequence::Update()
{
  enum
  {
    SEQ_INIT = 0,
    SEQ_WAIT,
  };

  const char*         authToken   = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->GetAuthenticationToken();
  Savedata::MyStatus* pMystatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();

  u64                 nexUniqueId = pMystatus->GetNexUniqueID();

  switch( GetSequenceNo() )
  {
  case SEQ_INIT:
    {
      //@fix[NMCat3939][#3939 ゲームシンク前にレーティングバトルやインターネット大会に入ろうとするとエラーコードが表示される件について]
      if( m_mode == NetLib::Pgl::E_PURPOSE_RATING_BATTLE || 
          m_mode == NetLib::Pgl::E_PURPOSE_INTERNET_CUP )
      {//インターネット大会 or なかま大会　or レーティングバトル
        if( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetIsCreateGameSyncSaveID() == false )
        {//ゲームシンクID未作成だったら未サインイン扱い
          if( m_pGameSyncConfirmPlayStatusResponseListener )
          {
            m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusSuccess();
          }

          return gflnet2::ServerClient::REQUEST_STATE_FINISH;
        }
      }

      m_pPglConnector = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Pgl::PGLConnector();

      if( m_pPglConnector->Initialize( m_pWork->GetDeviceHeap() , nexUniqueId, authToken, static_cast<nn::cfg::CTR::CfgRegionCode>( pMystatus->GetRegionCode() ) ) )
      {
        m_pPglConnector->SetPGLConnectorListener( this );
        bool retSync = m_pPglConnector->GetPlayStatus( static_cast<NetLib::Pgl::PGL_PURPOSE_TYPE>( m_mode ) );
        if( retSync )
        {
          SetSequenceNo( SEQ_WAIT );
        }
        else
        {//エラー
          GFL_PRINT("GameSGameSyncConfirmPlayStatusRequestSequenceyncProc : GetPlayStatus Failed -----------------------------\n");
          GFL_ASSERT( 0 );//@fix
          if( m_pGameSyncConfirmPlayStatusResponseListener )
          {
            m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( 0 );
          }
          Cancel();
        }
      }
      else
      {
        GFL_ASSERT( 0 );//@fix
        if( m_pGameSyncConfirmPlayStatusResponseListener )
        {
          m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( 0 );
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

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GameSyncConfirmPlayStatusRequestSequence::UpdateCancel()
{
  if( m_pPglConnector )
  {
    if( m_pPglConnector->IsConnecting() ){
      m_pPglConnector->CancelConnecting(); //キャンセル命令を発行
      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;//終了待ち
    }

    m_pPglConnector->RemovePGLConnectorListener();
    m_pPglConnector->Finalize();
    GFL_SAFE_DELETE( m_pPglConnector );
  }


  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//==========================================================================
/**
 *					PGLサーバー通信リスナー
 */
//==========================================================================
//------------------------------------------------------------------
/**
 * @brief プレイステータス確認通信の完了イベントを検知
 *
 * @param  isSignIn  連携サイトへの登録有無（true:登録済み）
 */
//------------------------------------------------------------------
void GameSyncConfirmPlayStatusRequestSequence::OnPGLGetPlayStatusSuccess( bool isSignIn )
{
  GFL_PRINT("OnPGLGetPlayStatusSuccess %d\n", isSignIn );

  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->SetSignedPGL( isSignIn );
  if( isSignIn )
  {
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->SetIsNoRegistPGL( false );
  }

  if( m_pGameSyncConfirmPlayStatusResponseListener )
  {
    m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusSuccess();
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
void GameSyncConfirmPlayStatusRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, httpStatus );
  
  if( m_isSkipError )
  {
    if( m_pGameSyncConfirmPlayStatusResponseListener )
    {
      m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( httpStatus , NetLib::Pgl::E_RET_COMMON_OK );
    }
  }
  else
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultPGL5xx( httpStatus );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    if( m_pGameSyncConfirmPlayStatusResponseListener )
    {
      m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( httpStatus );
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
void GameSyncConfirmPlayStatusRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, resultCode );

  if( m_isSkipError )
  {
    if( m_pGameSyncConfirmPlayStatusResponseListener )
    {
      m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( resultCode , resultCode );
    }
  }
  else
  {
    NetLib::Error::NijiNetworkError error;
    error.SetResultPGL( resultCode );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    if( m_pGameSyncConfirmPlayStatusResponseListener )
    {
      m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( resultCode );
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
void GameSyncConfirmPlayStatusRequestSequence::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  GFL_PRINT("OnPGLConnectError %d \n", errorCode );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  if( m_pGameSyncConfirmPlayStatusResponseListener )
  {
    m_pGameSyncConfirmPlayStatusResponseListener->OnConfirmPlayStatusError( errorCode );
  }

  Cancel();
}



GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)
#endif