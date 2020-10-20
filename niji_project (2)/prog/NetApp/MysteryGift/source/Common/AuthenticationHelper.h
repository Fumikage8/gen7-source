#if !defined(AUTHENTICATIONHELPER_H_INCLUDED)
#define AUTHENTICATIONHELPER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   AuthenticationHelper.h
 * @date   2016/04/12 Tue. 20:46:53
 * @author muto_yu
 * @brief  BOSS/PGLその他サーバ認証関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>

#include <system/include/Date/gfl2_Date.h>

#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include <NetStatic/NetLib/include/PGL/PGLConnector.h>
#include <NetStatic/NetLib/include/PGL/PGLConnectorListener.h>

namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  サーバ認証ヘルパ
  *  @date   2016/04/12 Tue. 20:49:01
  */
//==============================================================================
class AuthenticationHelper
#if defined( GF_PLATFORM_CTR )
 : public NetLib::Pgl::PGLConnectorListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN(AuthenticationHelper);

  enum
  {
    SERIAL_SIZE = 17,
  };

public:
  AuthenticationHelper(GiftWork* pWork);
  virtual ~AuthenticationHelper();

  /*
    initialize
  */
  void  BeginInitialize(void);
  bool  ObserveInitialize(void);



  /*
    internet接続～ゲームサーバ接続
  */
  enum  InternetConnectResult
  {
    INTERNET_CONNECT_RESULT_Processing,           /**<  処理中  */
    INTERNET_CONNECT_RESULT_Success,              /**<  成功    */

    INTERNET_CONNECT_RESULT_Error_Network,        /**<  エラー：net接続       */
    INTERNET_CONNECT_RESULT_Error_RequirePatch,   /**<  エラー：パッチが必要  */

    INTERNET_CONNECT_RESULT_
  };
  void                    BeginInternetConnect(void);
  InternetConnectResult   ObserveInternetConnect(void);


  /*
    friendServer接続
  */
  enum  FriendServerConnectResult
  {
    FRIENDSERVER_CONNECT_RESULT_Processing,             /**<  処理中  */
    FRIENDSERVER_CONNECT_RESULT_Success,                /**<  成功    */

    FRIENDSERVER_CONNECT_RESULT_Error_Network,          /**<  エラー：net接続  */
    
    FRIENDSERVER_CONNECT_RESULT_
  };
  void                      BeginFirendServerConnect(void);
  FriendServerConnectResult ObserveFirendServerConnect(void);


  /*
    PGLサーバシリアル認証
  */
  enum  PGLSerialAuthResult
  {
    PGL_SERIAL_AUTH_RESULT_Processing,
    PGL_SERIAL_AUTH_RESULT_Success,

    PGL_SERIAL_AUTH_RESULT_Error_InvalidCode,           /**<  エラー：認証に失敗                         */

    PGL_SERIAL_AUTH_RESULT_Error_MismatchROM,
    PGL_SERIAL_AUTH_RESULT_Error_MismatchRegion,
    PGL_SERIAL_AUTH_RESULT_Error_AlreadyDownloaded,
    PGL_SERIAL_AUTH_RESULT_Error_CampaignDownloaded,
    PGL_SERIAL_AUTH_RESULT_Error_LockedOther,
    PGL_SERIAL_AUTH_RESULT_Error_CampaignNotStarted,
    PGL_SERIAL_AUTH_RESULT_Error_CampaignEnd,

    PGL_SERIAL_AUTH_RESULT_Error_ServerError,           /**<  セッションエラー：アプレットにより通知する。ゲームコード側では通知せずエラーフローへ  */

    PGL_SERIAL_AUTH_RESULT_
  };
  void                  BeginPGLSerialAuth(const wchar_t* pSerialCode);
  PGLSerialAuthResult   ObservePGLSerialAuth(void);


  /*
    CommitSave
  */
  enum  CommitSaveResult
  {
    COMMIT_SAVE_RESULT_Processing,
    COMMIT_SAVE_RESULT_Success,

    /*  error  */
    COMMIT_SAVE_RESULT_Error_InvalidCode,               /**<  エラー：認証に失敗 */

    COMMIT_SAVE_RESULT_Error_MismatchROM,
    COMMIT_SAVE_RESULT_Error_MismatchRegion,
    COMMIT_SAVE_RESULT_Error_AlreadyDownloaded,
    COMMIT_SAVE_RESULT_Error_CampaignDownloaded,
    COMMIT_SAVE_RESULT_Error_LockedOther,
    COMMIT_SAVE_RESULT_Error_CampaignNotStarted,
    COMMIT_SAVE_RESULT_Error_CampaignEnd,

    COMMIT_SAVE_RESULT_Error_ServerError,               /**<  セッションエラー：アプレットにより通知する。ゲームコード側では通知せずエラーフローへ  */

    COMMIT_SAVE_RESULT_
  };
  void                  ReadyCommitSave(void);    /**<  CommitSaveの開始準備（巻き戻しポイントの確保）  */
  void                  BeginCommitSave(void);
  CommitSaveResult      ObserveCommitSave(void);


  /*
    Fianlize
  */
  void  BeginFinalize(void);
  bool  ObserveFinalize(void);

  u32 GetDataNo(void){ return m_datano; }
  const gfl2::system::Date& GetServerDate(void) const {return m_ServerDate;}

public:
#if defined( GF_PLATFORM_CTR )
  //==========================================================================
  /**
   *					PGLサーバー通信リスナー
   */
  //==========================================================================

  //------------------------------------------------------------------
  /**
  * @brief PGL不思議な贈り物シリアルコード認証通信の完了イベントを検知
  *
  * @param  serialstatus  シリアルコード利用状況ステータス
  * @param  datano        BOSSサーバーに登録されているデータNO
  */
  //------------------------------------------------------------------
  virtual void OnPGLAuthSerialSuccess( NetLib::Pgl::PGL_SERIAL_AUTH_RESULT serialstatus, u32 datano );

  //------------------------------------------------------------------
  /**
  * @brief PGL不思議な贈り物シリアルコード状態更新通信の完了イベントを検知
  *
  * @param  serialstatus  シリアルコード利用状況ステータス
  */
  //------------------------------------------------------------------
  virtual void OnPGLUpdateSerialStatusSuccess( NetLib::Pgl::PGL_SERIAL_AUTH_RESULT serialstatus );

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
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus );

  //------------------------------------------------------------------
  /**
   * @brief PGL通信のレスポンスデータのエラーコードを検知
   *
   * @param  commandType  完了したPGL通信コマンド
   * @param  resultCode   PGLサーバから受信したレスポンスエラーコード
   */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode );

  //------------------------------------------------------------------
  /**
   * @brief PGL通信のエラーイベントを検知
   *
   * @param  commandType  完了したPGL通信コマンド
   * @param  result       エラー内容
   * @param  errorCode    ネットワークエラーコード
   */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode );
#endif // defined( GF_PLATFORM_CTR )
protected:
  void  Initialize(void);
  void  UpdateServerDate(void);

  GiftWork*           m_pWork;
  u32                 m_Phase;
  gfl2::system::Date  m_ServerDate;



#if defined( GF_PLATFORM_CTR )
  NetLib::Wifi::WifiConnectRunner*                m_pWifiConnectRunner;
  NetLib::Pgl::PGLConnector* m_pPglConnector;
  char m_serialBuf[SERIAL_SIZE];

  bool  m_isWifiConnected;      /**<  wifiが接続されている  */
  bool  m_isAuthEnd;
  bool  m_isPGLConnectError;    /**<  PGL接続セッション内でエラーが発生し、アプレットによる報告がされた  */
  NetLib::Pgl::PGL_SERIAL_AUTH_RESULT m_serialStatus;
  
  /*  commitsave  */
  struct
  {
    u32                                   m_Phase;
    Savedata::MysteryGiftSave::CORE_DATA  m_GiftSaveCoreDataClone;
    
    void  Clear(void)
    {
      m_Phase  = 0;
      memset(&m_GiftSaveCoreDataClone, 0, sizeof(m_GiftSaveCoreDataClone));
    }
  }m_CommitSaveInfo;


#endif // defined( GF_PLATFORM_CTR )

  u32 m_datano;

};



} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(AUTHENTICATIONHELPER_H_INCLUDED)  */
