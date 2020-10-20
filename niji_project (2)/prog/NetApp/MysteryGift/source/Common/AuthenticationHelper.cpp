//==============================================================================
/**
 * @file   AuthenticationHelper.cpp
 * @date   2016/04/12 Tue. 20:46:56
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../GiftResource.h"
#include  "../ApplicationSystem/GiftWork.h"
#include  "./AuthenticationHelper.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include <Savedata/include/GameSyncSave.h>
#include <Savedata/include/MyStatus.h>

namespace
{
  enum  Phase
  {
    PHASE_None,

    PHASE_Initialize_Processing,
    PHASE_Initialize_Done,

    PHASE_InternetConnect_Processing,
    PHASE_InternetConnect_Done,

    PHASE_FriendServerConnect_Processing,
    PHASE_FriendServerConnect_Done,

    PHASE_PGLSerialAuth_Processing,
    PHASE_PGLSerialAuth_Done,

    PHASE_CommitSave_Ready,
    PHASE_CommitSave_Processing,
    PHASE_CommitSave_Done,

    PHASE_Finalize_Processing,
    PHASE_Finalize_Done = PHASE_None
  };


  enum  CommitSavePhase
  {
    COMMIT_SAVE_PHASE_Initial,


    /*  1st  */
    COMMIT_SAVE_PHASE_FirstSaveStart,
    COMMIT_SAVE_PHASE_FirstSave,
    COMMIT_SAVE_PHASE_FirstSaveDone,

    /*  pgl  */
    COMMIT_SAVE_PHASE_PGLAuthStart,
    COMMIT_SAVE_PHASE_PGLAuth,
    COMMIT_SAVE_PHASE_PGLAuthDone,

    /*  last  */
    COMMIT_SAVE_PHASE_LastSaveStart,
    COMMIT_SAVE_PHASE_LastSave,
    COMMIT_SAVE_PHASE_LastSaveDone,


    COMMIT_SAVE_PHASE_
  };

}






namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  AuthenticationHelper::Initialize(void)
{
#if defined( GF_PLATFORM_CTR )
  m_pWifiConnectRunner = NULL;
  m_pPglConnector = NULL;
  std::memset( m_serialBuf , 0 , sizeof(m_serialBuf) );

  m_isWifiConnected   = false;
  m_isAuthEnd         = false;
  m_isPGLConnectError = false;
  m_serialStatus      = NetLib::Pgl::E_RET_SERIAL_NG;
  
  m_CommitSaveInfo.Clear();
#endif // defined( GF_PLATFORM_CTR )

  m_datano = 0;
}


//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
AuthenticationHelper::AuthenticationHelper(GiftWork* pWork)
  : m_pWork(pWork)
  , m_Phase(PHASE_None)
  , m_ServerDate()
{
  Initialize();
}


//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
AuthenticationHelper::~AuthenticationHelper()
{
  GFL_ASSERT(m_Phase == PHASE_Finalize_Done);
#if defined( GF_PLATFORM_CTR )
  if( m_pWifiConnectRunner )
  {
    m_pWifiConnectRunner->Finalize();
  }
  GFL_SAFE_DELETE( m_pWifiConnectRunner );
  if( m_pPglConnector )
  {
    m_pPglConnector->RemovePGLConnectorListener();
    m_pPglConnector->Finalize();
  }
  GFL_SAFE_DELETE( m_pPglConnector );
#endif // defined( GF_PLATFORM_CTR )
}


//------------------------------------------------------------------
/**
  *  @brief    初期化
  */
//------------------------------------------------------------------
void  AuthenticationHelper::BeginInitialize(void)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);
  GFL_ASSERT(m_Phase == PHASE_None);

  Initialize();

#if defined( GF_PLATFORM_CTR )
  NetLib::NijiNetworkSystem::InitializeWifiNetwork();
#endif // defined( GF_PLATFORM_CTR )


  m_Phase = PHASE_Initialize_Processing;
}


//------------------------------------------------------------------
/**
  *  @brief    初期化監視
  */
//------------------------------------------------------------------
bool  AuthenticationHelper::ObserveInitialize(void)
{
  GFL_ASSERT((m_Phase == PHASE_Initialize_Processing) || (m_Phase == PHASE_Initialize_Done));


  {
    m_Phase = PHASE_Initialize_Done;
  }

  return (m_Phase == PHASE_Initialize_Done);
}


//------------------------------------------------------------------
/**
  *  @brief    internet接続～ゲームサーバ接続
  */
//------------------------------------------------------------------
void  AuthenticationHelper::BeginInternetConnect(void)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);
  GFL_ASSERT(m_Phase == PHASE_Initialize_Done);


#if defined( GF_PLATFORM_CTR )
  GFL_ASSERT(!m_pWifiConnectRunner);

  Savedata::MyStatus*                           pMyStatus         = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE  mode              = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT;
  const bool                                    bFirstConnection  = ( pMyStatus->IsValidNexUniqueID() ) ? false : true;

  if( bFirstConnection )
  {
    mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT_AND_CREATE_NEXID;
  }

  m_pWifiConnectRunner = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Wifi::WifiConnectRunner( mode , true );
#endif // defined( GF_PLATFORM_CTR )


  m_Phase = PHASE_InternetConnect_Processing;
}


//------------------------------------------------------------------
/**
  *  @brief    internet接続～ゲームサーバ接続
  */
//------------------------------------------------------------------
AuthenticationHelper::InternetConnectResult   AuthenticationHelper::ObserveInternetConnect(void)
{
  GFL_ASSERT((m_Phase == PHASE_InternetConnect_Processing) || (m_Phase == PHASE_InternetConnect_Done));

  InternetConnectResult result  = INTERNET_CONNECT_RESULT_Processing;

#if defined( GF_PLATFORM_CTR )

  const NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = m_pWifiConnectRunner->Update();

/*
  typedef enum
  {
    E_RET_CONTINUE,                 //!< 実行中
    E_RET_SUCCESS,                  //!< 正常終了
    E_RET_FAILED,                   //!< エラー終了
    E_RET_FAILED_PATCH,

  }E_RET_CODE;
*/

  switch(ret)
  {
    case  NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE:    //!< 実行中
      /*  nop  */
      return INTERNET_CONNECT_RESULT_Processing;


    case  NetLib::Wifi::WifiConnectRunner::E_RET_SUCCESS:     //!< 正常終了
    {
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      bool bFirstConnection = ( pMyStatus->IsValidNexUniqueID() ) ? false : true;

      if( bFirstConnection )
      {
        pMyStatus->SetNexUniqueID( m_pWifiConnectRunner->GetNexUniqueId() );
      }

      m_isWifiConnected = true;


      /*  ServerDate更新  */
      UpdateServerDate();

      result  = INTERNET_CONNECT_RESULT_Success;
      m_Phase = PHASE_InternetConnect_Done;
    }
      break;

    case  NetLib::Wifi::WifiConnectRunner::E_RET_FAILED:      //!< エラー終了
      /*  アプレット呼び出しはWifiConnectRunner内部で行われている  */

      m_isWifiConnected = m_pWifiConnectRunner->IsNeedDisconnectProcess();

      result = INTERNET_CONNECT_RESULT_Error_Network;
      break;
    

    case  NetLib::Wifi::WifiConnectRunner::E_RET_FAILED_PATCH:
      m_isWifiConnected = m_pWifiConnectRunner->IsNeedDisconnectProcess();

      result = INTERNET_CONNECT_RESULT_Error_RequirePatch;
      break;


    default:
      GFL_ASSERT(false);
      result = INTERNET_CONNECT_RESULT_Error_Network;
      break;
  }


  m_pWifiConnectRunner->Finalize();
  GFL_SAFE_DELETE(m_pWifiConnectRunner);

#endif // defined( GF_PLATFORM_CTR )
  return result;
}



//------------------------------------------------------------------
/**
  *  @brief    friendServer接続
  */
//------------------------------------------------------------------
void  AuthenticationHelper::BeginFirendServerConnect(void)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);
  GFL_ASSERT(m_Phase == PHASE_InternetConnect_Done);


  m_Phase = PHASE_FriendServerConnect_Processing;
}


//------------------------------------------------------------------
/**
  *  @brief    friendServer接続
  */
//------------------------------------------------------------------
AuthenticationHelper::FriendServerConnectResult AuthenticationHelper::ObserveFirendServerConnect(void)
{
  GFL_ASSERT((m_Phase == PHASE_FriendServerConnect_Processing) || (m_Phase == PHASE_FriendServerConnect_Done));

  FriendServerConnectResult result  = FRIENDSERVER_CONNECT_RESULT_Processing;


  {
    result  = FRIENDSERVER_CONNECT_RESULT_Success;
    m_Phase = PHASE_FriendServerConnect_Done;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    PGLサーバシリアル認証
  */
//------------------------------------------------------------------
void  AuthenticationHelper::BeginPGLSerialAuth(const wchar_t* pSerialCode)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);


#if defined( GF_PLATFORM_CTR )
  GFL_ASSERT(!m_pPglConnector);
  GFL_ASSERT(m_Phase == PHASE_FriendServerConnect_Done);

  /*  clear  */
  m_isAuthEnd         = false;
  m_isPGLConnectError = false;


  ::std::memset( m_serialBuf , 0 , sizeof(m_serialBuf) );
  char* pTmp = (char*)m_serialBuf;
  for( u32 index = 0 ; index < SERIAL_SIZE - 1 ; ++index )
  {
    wchar_t wc = pSerialCode[index];
    //GFL_PRINT("wc == %d\n",wc);
    if( wc >= L'０' && wc <= L'９' )
    {
      wc -= L'０';
      *pTmp = wc + '0';
    }
    else if( wc >= L'Ａ' && wc <= L'Ｚ' )
    {
      wc -= L'Ａ';
      *pTmp = wc + 'A';
    }
    else
    {
      *pTmp = wc;
    }
    pTmp++;
  }

  GFL_PRINT("AuthenticationHelper::BeginPGLSerialAuth : [%s]\n",m_serialBuf );

  const char*           authToken   = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->GetAuthenticationToken();
  Savedata::MyStatus*   pMystatus   = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
  u64                   nexUniqueId = pMystatus->GetNexUniqueID();

  m_pPglConnector = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Pgl::PGLConnector();

  if( m_pPglConnector->Initialize( m_pWork->GetDeviceHeap() , nexUniqueId, authToken, static_cast<nn::cfg::CTR::CfgRegionCode>( pMystatus->GetRegionCode() ) ) )
  {
    m_pPglConnector->SetPGLConnectorListener( this );
    //bool retSync = m_pPglConnector->GetPlayStatus( static_cast<NetLib::Pgl::PGL_PURPOSE_TYPE>( 0 ) );
    bool retSync = m_pPglConnector->AuthSerialCode( pMystatus->GetSimpleAddressId() , m_serialBuf );
    if( retSync ==false )
    {//エラー
      GFL_ASSERT( 0 );//@fix
    }
  }
  else
  {
    GFL_ASSERT( 0 );//@fix
  }

#endif


  m_Phase = PHASE_PGLSerialAuth_Processing;
}


//------------------------------------------------------------------
/**
  *  @brief    PGLサーバシリアル認証
  */
//------------------------------------------------------------------
AuthenticationHelper::PGLSerialAuthResult   AuthenticationHelper::ObservePGLSerialAuth(void)
{
  GFL_ASSERT((m_Phase == PHASE_PGLSerialAuth_Processing) || (m_Phase == PHASE_PGLSerialAuth_Done));

  PGLSerialAuthResult result  = PGL_SERIAL_AUTH_RESULT_Processing;


#if defined( GF_PLATFORM_CTR )
  if( m_isAuthEnd == true )
  {
    if( m_pPglConnector )
    {
      if( m_pPglConnector->IsConnecting() ){
        m_pPglConnector->CancelConnecting(); //キャンセル命令を発行
        return result;//終了待ち
      }

      m_pPglConnector->RemovePGLConnectorListener();
      m_pPglConnector->Finalize();
      GFL_SAFE_DELETE( m_pPglConnector );
    }

    /*
      結果
    */
    if(m_isPGLConnectError)
    {
      /*
         PGL接続セッションにおいてエラーが発生したため、セッションが完遂せず、アプレットによってエラーが報告された
         アプリケーション側では通知せず、失敗フローへ
         ※NetLib::Pgl::PGL_SERIAL_AUTH_RESULT m_serialStatus;  は無効である
      */
      result = PGL_SERIAL_AUTH_RESULT_Error_ServerError;
    }
    else
    {
      /*
         PGL接続セッションは完遂され、レスポンスが返された
         アプリケーション側で通知した上でそれぞれのフローへ
      */
#if 0
  /**
   * @brief PGL不思議な贈り物シリアルコード認証レスポンスステータス
   */
  typedef enum
  {
    E_RET_SERIAL_OK = 0,              //!< シリアルコードの認証成功
    E_RET_SERIAL_NG = 10,             //!< シリアルコードが無効
    E_RET_SERIAL_MISMATCH_ROM,        //!< 該当のキャンペーンで受け取れるROMと不一致
    E_RET_SERIAL_MISMATCH_LANG,       //!< 該当のキャンペーンで受け取れる言語コードと不一致
    E_RET_SERIAL_MISMATCH_AREA,       //!< 該当のキャンペーンで受け取れる国地域と不一致
    E_RET_SERIAL_MISMATCH_REGION,     //!< 該当のキャンペーンで受け取れるリージョンと不一致
    E_RET_SERIAL_ALREADY_DOWNLOADED,  //!< 既にキャンペーンアイテム取得済み
    E_RET_SERIAL_CAMPAIGN_DOWNLOADED, //!< 同一キャンペーンで既にアイテム取得済み
    E_RET_SERIAL_LOCKED_OTHER,        //!< 他ユーザーがロック中（該当のゲームシンク以外のユーザーがロックした場合）
    E_RET_SERIAL_CAMPAIGN_NOT_STARTED,//!< キャンペーン開始前
    E_RET_SERIAL_CAMPAIGN_END,        //!< キャンペーン終了
  }PGL_SERIAL_AUTH_RESULT;
#endif

      /*  ServerDate更新  */
      UpdateServerDate();

      switch( m_serialStatus )
      {
        /*  成功  */
        case NetLib::Pgl::E_RET_SERIAL_OK:
          result  = PGL_SERIAL_AUTH_RESULT_Success;
          m_Phase = PHASE_PGLSerialAuth_Done;
        break;

        /*  以下失敗  */
        case NetLib::Pgl::E_RET_SERIAL_NG:    //!< シリアルコードが無効
          result  = PGL_SERIAL_AUTH_RESULT_Error_InvalidCode;
          m_Phase = PHASE_FriendServerConnect_Done;  /*  E_RET_SERIAL_NGのときはserial再入力に対応するためPhaseを巻き戻す  */
          break;

        case NetLib::Pgl::E_RET_SERIAL_MISMATCH_ROM:          result  = PGL_SERIAL_AUTH_RESULT_Error_MismatchROM;         break;  //!< 該当のキャンペーンで受け取れるROMと不一致
        case NetLib::Pgl::E_RET_SERIAL_MISMATCH_LANG:         result  = PGL_SERIAL_AUTH_RESULT_Error_MismatchRegion;      break;  //!< 該当のキャンペーンで受け取れる言語コードと不一致
        case NetLib::Pgl::E_RET_SERIAL_MISMATCH_AREA:         result  = PGL_SERIAL_AUTH_RESULT_Error_MismatchRegion;      break;  //!< 該当のキャンペーンで受け取れる国地域と不一致
        case NetLib::Pgl::E_RET_SERIAL_MISMATCH_REGION:       result  = PGL_SERIAL_AUTH_RESULT_Error_MismatchRegion;      break;  //!< 該当のキャンペーンで受け取れるリージョンと不一致
        case NetLib::Pgl::E_RET_SERIAL_ALREADY_DOWNLOADED:    result  = PGL_SERIAL_AUTH_RESULT_Error_AlreadyDownloaded;   break;  //!< 既にキャンペーンアイテム取得済み
        case NetLib::Pgl::E_RET_SERIAL_CAMPAIGN_DOWNLOADED:   result  = PGL_SERIAL_AUTH_RESULT_Error_CampaignDownloaded;  break;  //!< 同一キャンペーンで既にアイテム取得済み
        case NetLib::Pgl::E_RET_SERIAL_LOCKED_OTHER:          result  = PGL_SERIAL_AUTH_RESULT_Error_LockedOther;         break;  //!< 他ユーザーがロック中（該当のゲームシンク以外のユーザーがロックした場合）
        case NetLib::Pgl::E_RET_SERIAL_CAMPAIGN_NOT_STARTED:  result  = PGL_SERIAL_AUTH_RESULT_Error_CampaignNotStarted;  break;  //!< キャンペーン開始前
        case NetLib::Pgl::E_RET_SERIAL_CAMPAIGN_END:          result  = PGL_SERIAL_AUTH_RESULT_Error_CampaignEnd;         break;  //!< キャンペーン終了

        default:
          GFL_ASSERT(false);
          result = PGL_SERIAL_AUTH_RESULT_Error_ServerError;
        break;
      }
    }
  }
#endif
  return result;
}



//------------------------------------------------------------------
/**
  *  @brief    CommitSaveの開始準備（巻き戻しポイントの確保）
  */
//------------------------------------------------------------------
void  AuthenticationHelper::ReadyCommitSave(void)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);

#if defined( GF_PLATFORM_CTR )
  GFL_ASSERT(!m_pPglConnector);
  GFL_ASSERT((m_Phase == PHASE_PGLSerialAuth_Done) || (m_Phase == PHASE_CommitSave_Ready));

  /*  巻き戻し用Clone  */
  {
    m_CommitSaveInfo.Clear();
    m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_Initial;
    GetMysteryGiftSave()->SetupClone(m_CommitSaveInfo.m_GiftSaveCoreDataClone);                /**<  commitSave巻き戻し用Cloneのセット     */

    m_Phase = PHASE_CommitSave_Ready;
  }
#endif
}



//------------------------------------------------------------------
/**
  *  @brief    CommitSave
  */
//------------------------------------------------------------------
void  AuthenticationHelper::BeginCommitSave(void)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);


#if defined( GF_PLATFORM_CTR )
  GFL_ASSERT(!m_pPglConnector);
  GFL_ASSERT(m_Phase == PHASE_CommitSave_Ready);


  const char*         authToken   = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->GetAuthenticationToken();
  Savedata::MyStatus* pMystatus   = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
  const u64           nexUniqueId = pMystatus->GetNexUniqueID();

  m_pPglConnector = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Pgl::PGLConnector();

  if( m_pPglConnector->Initialize( m_pWork->GetDeviceHeap() , nexUniqueId, authToken, static_cast<nn::cfg::CTR::CfgRegionCode>( pMystatus->GetRegionCode() ) ) )
  {
    m_pPglConnector->SetPGLConnectorListener( this );
  }
  else
  {
    GFL_ASSERT( 0 );//@fix
  }

  m_isAuthEnd = false;

#endif


  m_Phase = PHASE_CommitSave_Processing;
}


//------------------------------------------------------------------
/**
  *  @brief    CommitSave
  */
//------------------------------------------------------------------
AuthenticationHelper::CommitSaveResult   AuthenticationHelper::ObserveCommitSave(void)
{
  GFL_ASSERT((m_Phase == PHASE_CommitSave_Processing) || (m_Phase == PHASE_CommitSave_Done));


  CommitSaveResult      result          = COMMIT_SAVE_RESULT_Processing;
  GameSys::GameData*    pGameData       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  gfl2::heap::HeapBase* pHeapBase       = m_pWork->GetDeviceHeap();
  gfl2::heap::HeapBase* pHeapBaseLower  = pHeapBase->GetLowerHandle();

#if defined( GF_PLATFORM_CTR )

  GFL_PRINT("%s: %u\n", __FUNCTION__, m_CommitSaveInfo.m_Phase);
  switch(m_CommitSaveInfo.m_Phase)
  {
    /*
      initial
    */
    case  COMMIT_SAVE_PHASE_Initial:
      m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_FirstSaveStart;
      /*  fall through  */


    /*
      1st
    */
    case  COMMIT_SAVE_PHASE_FirstSaveStart:
      pGameData->FirstSaveStart(pHeapBaseLower);
      m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_FirstSave;
      /*  fall through  */

    case  COMMIT_SAVE_PHASE_FirstSave:
    {
      gfl2::fs::FsCtrBackupSystem::Result fsResult;

      if(pGameData->FirstSaveMain(&fsResult))
      {
        if(fsResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK)
        {
          /*  成功  */
          m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_FirstSaveDone;
        }
        else
        {
          GFL_ASSERT(false);
        }
      }
    }
      break;

    case  COMMIT_SAVE_PHASE_FirstSaveDone:
      m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_PGLAuthStart;
      break;

    /*
      pgl
    */
    case  COMMIT_SAVE_PHASE_PGLAuthStart:
    {
      Savedata::MyStatus* pMystatus = pGameData->GetPlayerStatus();
      bool                retSync   = m_pPglConnector->UpdateSerialCode( pMystatus->GetSimpleAddressId() , m_serialBuf );
      if( retSync ==false )
      {//エラー
        GFL_ASSERT( 0 );//@fix
      }
      
      m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_PGLAuth;
    }
      /*  fall through  */

    case  COMMIT_SAVE_PHASE_PGLAuth:
    {
      if( m_isAuthEnd == true )
      {
        if( m_pPglConnector )
        {
          if( m_pPglConnector->IsConnecting() )
          {
            m_pPglConnector->CancelConnecting();  //キャンセル命令を発行
          }
          else
          {
            m_pPglConnector->RemovePGLConnectorListener();
            m_pPglConnector->Finalize();
            GFL_SAFE_DELETE( m_pPglConnector );
            m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_PGLAuthDone;
          }
        }
        else
        {
          GFL_ASSERT(false);
          m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_PGLAuthDone;
        }
      }
    }
      break;
    
    

    case  COMMIT_SAVE_PHASE_PGLAuthDone:
    {
      bool  bOK = false;
      
      
      /*
        結果
      */
      if(m_isPGLConnectError)
      {
        /*
           PGL接続セッションにおいてエラーが発生したため、セッションが完遂せず、アプレットによってエラーが報告された
           アプリケーション側では通知せず、失敗フローへ
           ※NetLib::Pgl::PGL_SERIAL_AUTH_RESULT m_serialStatus;  は無効である
        */
        result = COMMIT_SAVE_RESULT_Error_ServerError;
      }
      else
      {
        /*
           PGL接続セッションは完遂され、レスポンスが返された
           アプリケーション側で通知した上でそれぞれのフローへ
        */
#if 0
    /**
     * @brief PGL不思議な贈り物シリアルコード認証レスポンスステータス
     */
    typedef enum
    {
      E_RET_SERIAL_OK = 0,              //!< シリアルコードの認証成功
      E_RET_SERIAL_NG = 10,             //!< シリアルコードが無効
      E_RET_SERIAL_MISMATCH_ROM,        //!< 該当のキャンペーンで受け取れるROMと不一致
      E_RET_SERIAL_MISMATCH_LANG,       //!< 該当のキャンペーンで受け取れる言語コードと不一致
      E_RET_SERIAL_MISMATCH_AREA,       //!< 該当のキャンペーンで受け取れる国地域と不一致
      E_RET_SERIAL_MISMATCH_REGION,     //!< 該当のキャンペーンで受け取れるリージョンと不一致
      E_RET_SERIAL_ALREADY_DOWNLOADED,  //!< 既にキャンペーンアイテム取得済み
      E_RET_SERIAL_CAMPAIGN_DOWNLOADED, //!< 同一キャンペーンで既にアイテム取得済み
      E_RET_SERIAL_LOCKED_OTHER,        //!< 他ユーザーがロック中（該当のゲームシンク以外のユーザーがロックした場合）
      E_RET_SERIAL_CAMPAIGN_NOT_STARTED,//!< キャンペーン開始前
      E_RET_SERIAL_CAMPAIGN_END,        //!< キャンペーン終了
    }PGL_SERIAL_AUTH_RESULT;
#endif
        switch( m_serialStatus )
        {
          /*  成功  */
          case NetLib::Pgl::E_RET_SERIAL_OK:
            bOK     = true;
            break;

          /*  以下失敗  */
          case NetLib::Pgl::E_RET_SERIAL_NG:                    result  = COMMIT_SAVE_RESULT_Error_InvalidCode;         break;  //!< シリアルコードが無効
          case NetLib::Pgl::E_RET_SERIAL_MISMATCH_ROM:          result  = COMMIT_SAVE_RESULT_Error_MismatchROM;         break;  //!< 該当のキャンペーンで受け取れるROMと不一致
          case NetLib::Pgl::E_RET_SERIAL_MISMATCH_LANG:         result  = COMMIT_SAVE_RESULT_Error_MismatchRegion;      break;  //!< 該当のキャンペーンで受け取れる言語コードと不一致
          case NetLib::Pgl::E_RET_SERIAL_MISMATCH_AREA:         result  = COMMIT_SAVE_RESULT_Error_MismatchRegion;      break;  //!< 該当のキャンペーンで受け取れる国地域と不一致
          case NetLib::Pgl::E_RET_SERIAL_MISMATCH_REGION:       result  = COMMIT_SAVE_RESULT_Error_MismatchRegion;      break;  //!< 該当のキャンペーンで受け取れるリージョンと不一致
          case NetLib::Pgl::E_RET_SERIAL_ALREADY_DOWNLOADED:    result  = COMMIT_SAVE_RESULT_Error_AlreadyDownloaded;   break;  //!< 既にキャンペーンアイテム取得済み
          case NetLib::Pgl::E_RET_SERIAL_CAMPAIGN_DOWNLOADED:   result  = COMMIT_SAVE_RESULT_Error_CampaignDownloaded;  break;  //!< 同一キャンペーンで既にアイテム取得済み
          case NetLib::Pgl::E_RET_SERIAL_LOCKED_OTHER:          result  = COMMIT_SAVE_RESULT_Error_LockedOther;         break;  //!< 他ユーザーがロック中（該当のゲームシンク以外のユーザーがロックした場合）
          case NetLib::Pgl::E_RET_SERIAL_CAMPAIGN_NOT_STARTED:  result  = COMMIT_SAVE_RESULT_Error_CampaignNotStarted;  break;  //!< キャンペーン開始前
          case NetLib::Pgl::E_RET_SERIAL_CAMPAIGN_END:          result  = COMMIT_SAVE_RESULT_Error_CampaignEnd;         break;  //!< キャンペーン終了

          default:
            GFL_ASSERT(false);
            result  = COMMIT_SAVE_RESULT_Error_ServerError;
            break;
        }
      }

      /*  遷移  */
      if(bOK)
      {
        m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_LastSaveStart;
      }
      else
      {
        /*  firstSaveのキャンセルとCloneの巻き戻し  */
        pGameData->FirstSaveCancel();
        GetMysteryGiftSave()->WriteBackClone(&(m_CommitSaveInfo.m_GiftSaveCoreDataClone));   /**<  commitSave巻き戻し用Cloneのセット     */
      }
    }
      break;


    /*
      last
    */
    case  COMMIT_SAVE_PHASE_LastSaveStart:
      pGameData->LastSaveStart(pHeapBaseLower);
      m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_LastSave;
      /*  fall through  */

    case  COMMIT_SAVE_PHASE_LastSave:
    {
      gfl2::fs::FsCtrBackupSystem::Result fsResult;

      if(pGameData->LastSaveMain(&fsResult))
      {
        if(fsResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK)
        {
          /*  成功  */
          m_CommitSaveInfo.m_Phase  = COMMIT_SAVE_PHASE_LastSaveDone;
        }
        else
        {
          /*  失敗  */
          /*  firstSaveのキャンセルとCloneの巻き戻し  */
          pGameData->FirstSaveCancel();
          GetMysteryGiftSave()->WriteBackClone(&(m_CommitSaveInfo.m_GiftSaveCoreDataClone));   /**<  commitSave巻き戻し用Cloneのセット     */
        }
      }
    }
      break;

    case  COMMIT_SAVE_PHASE_LastSaveDone:
      result  = COMMIT_SAVE_RESULT_Success;
      break;

    default:
      GFL_ASSERT(false);
      break;
  }
#endif

  return result;
}




//------------------------------------------------------------------
/**
  *  @brief   Fianlize
  */
//------------------------------------------------------------------
void  AuthenticationHelper::BeginFinalize(void)
{
  GFL_PRINT("AuthenticationHelper: %s\n", __FUNCTION__);

#if defined( GF_PLATFORM_CTR )

  if(m_isWifiConnected)
  {
    GFL_ASSERT(!m_pWifiConnectRunner);
    m_pWifiConnectRunner = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Wifi::WifiConnectRunner( NetLib::Wifi::WifiConnectRunner::E_MODE_DISCONNECT );
  }

#endif


  m_Phase = PHASE_Finalize_Processing;
}


//------------------------------------------------------------------
/**
  *  @brief   Fianlize
  */
//------------------------------------------------------------------
bool  AuthenticationHelper::ObserveFinalize(void)
{
  GFL_ASSERT((m_Phase == PHASE_Finalize_Processing) || (m_Phase == PHASE_Finalize_Done));


#if defined( GF_PLATFORM_CTR )

  if( m_pPglConnector )
  {
    if( m_pPglConnector->IsConnecting() ){
      m_pPglConnector->CancelConnecting(); //キャンセル命令を発行
      return false;//終了待ち
    }

    m_pPglConnector->RemovePGLConnectorListener();
    m_pPglConnector->Finalize();
    GFL_SAFE_DELETE( m_pPglConnector );
  }

  if( m_pWifiConnectRunner )
  {
    NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = m_pWifiConnectRunner->Update();

    if( ret == NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE )
    {
      return false;
    }

    m_pWifiConnectRunner->Finalize();
    GFL_SAFE_DELETE( m_pWifiConnectRunner );
    m_isWifiConnected = false;
  }

  m_Phase = PHASE_Finalize_Done;
  NetLib::NijiNetworkSystem::TerminateNetwork();

#endif

  return (m_Phase == PHASE_Finalize_Done);
}



#if defined( GF_PLATFORM_CTR )

//------------------------------------------------------------------
/**
* @brief PGL不思議な贈り物シリアルコード認証通信の完了イベントを検知
*
* @param  serialstatus  シリアルコード利用状況ステータス
* @param  datano        BOSSサーバーに登録されているデータNO
*/
//------------------------------------------------------------------
void AuthenticationHelper::OnPGLAuthSerialSuccess( NetLib::Pgl::PGL_SERIAL_AUTH_RESULT serialstatus, u32 datano )
{
  GFL_PRINT("OnPGLAuthSerialSuccess : [%d][%d]\n",serialstatus,datano);
  m_serialStatus = serialstatus;
  m_datano = datano;

  m_isAuthEnd         = true;
  m_isPGLConnectError = false;
}

//------------------------------------------------------------------
/**
* @brief PGL不思議な贈り物シリアルコード状態更新通信の完了イベントを検知
*
* @param  serialstatus  シリアルコード利用状況ステータス
*/
//------------------------------------------------------------------
void AuthenticationHelper::OnPGLUpdateSerialStatusSuccess( NetLib::Pgl::PGL_SERIAL_AUTH_RESULT serialstatus )
{
  GFL_PRINT("OnPGLUpdateSerialStatusSuccess : [%d]\n",serialstatus);
  m_serialStatus = serialstatus;

  m_isAuthEnd         = true;
  m_isPGLConnectError = false;
}


//------------------------------------------------------------------
/**
* @brief PGL通信のエラーイベントを検知
*
* @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
*
* @param commandType 完了したPGL通信コマンド
* @param htttpStatus サーバから返却されたHTTPステータスコード
*/
//------------------------------------------------------------------
void AuthenticationHelper::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, httpStatus );

  NetLib::Error::NijiNetworkError error;
  error.SetResultSerial5xx( httpStatus );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_isAuthEnd         = true;
  m_isPGLConnectError = true;
}

//------------------------------------------------------------------
/**
* @brief PGL通信のレスポンスデータのエラーコードを検知
*
* @param  commandType  完了したPGL通信コマンド
* @param  resultCode   PGLサーバから受信したレスポンスエラーコード
*/
//------------------------------------------------------------------
void AuthenticationHelper::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
{
  GFL_PRINT("OnPGLConnectError %d %d\n", commandType, resultCode );

  NetLib::Error::NijiNetworkError error;
  error.SetResultSerial( resultCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_isAuthEnd         = true;
  m_isPGLConnectError = true;
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
void AuthenticationHelper::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  GFL_PRINT("OnPGLConnectError %d \n", errorCode );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_isAuthEnd         = true;
  m_isPGLConnectError = true;
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  AuthenticationHelper::UpdateServerDate(void)
{
  bool                bGetGameServerTimeResult  = false;
  nn::nex::DateTime   dateTime;

  bGetGameServerTimeResult  = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetNgsFacade()->GetGameServerTime( &dateTime );
  GFL_ASSERT(bGetGameServerTimeResult); /*  接続されていればFalseが返ることはない  */

#if PM_DEBUG
  m_pWork->m_pGiftDebug->OverwriteServerTimeByLocalTimeIfEnable(dateTime);
#endif

  // @fix GFCHECK[2981] Dayの情報を初期化する。
  m_ServerDate.SetDay(1);

  m_ServerDate.SetYear(dateTime.GetYear());
  m_ServerDate.SetMonth(dateTime.GetMonth());
  m_ServerDate.SetDay(dateTime.GetDay());
  m_ServerDate.SetHour(dateTime.GetHour());
  m_ServerDate.SetMinute(dateTime.GetMinute());

#if PM_DEBUG
  m_pWork->m_pGiftDebug->ApplyServerDate(m_ServerDate);
#endif
}

#endif // defined( GF_PLATFORM_CTR )

} /*  namespace Gift    */
} /*  namespace NetApp  */
