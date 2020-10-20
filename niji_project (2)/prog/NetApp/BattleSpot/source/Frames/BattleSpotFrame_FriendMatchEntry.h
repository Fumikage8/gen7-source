#if !defined(BATTLESPOTFRAME_FRIENDMATCHENTRY_H_INCLUDED)
#define BATTLESPOTFRAME_FRIENDMATCHENTRY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_FriendMatchEntry.h
 * @date   2015/10/02 Fri. 13:30:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncResponseListener.h"

#include <NetStatic/NetLib/include/PGL/PGLConnector.h>
#include <NetStatic/NetLib/include/PGL/PGLConnectorListener.h>

namespace NetApp    {
namespace BattleSpot {
  class BattleSpotWork;
  class BattleSpotViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief
  */
//==============================================================================
class Frame_FriendMatchEntry
  : public    NetAppLib::System::NetAppFrameBase
  , public NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener
  , public NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener
#if defined(GF_PLATFORM_CTR)
  , public NetLib::Pgl::PGLConnectorListener
#endif
  , protected LowerView_Selection::IEventHandler
  , protected LowerView_Card::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_FriendMatchEntry );
public:

  //PGLシーケンス
  enum
  {
    PGL_INIT = 0,
    PGL_WAIT,
    PGL_GET_WORLD_STATUS,
    PGL_BATTLESTATUS_WAIT,
  };

  enum
  {
    DUMMY_WAIT = 30,
  };

public:
  Frame_FriendMatchEntry(BattleSpotWork* pWork);
  virtual ~Frame_FriendMatchEntry();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
//  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */

protected:
//  virtual void  exitFrame(NetAppLib::System::FrameResult result );

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode);

  virtual void  LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode);


  void  ExitSelf(const FrameResult frameResult = RESULT_FriendMatchEntry_Back);

  void SetViewModeForWork( void );

  void UpdatePGLCheck( void );

  bool UpdateWorldBattleStatus( void );

public:

  virtual void OnConfirmPlayStatusSuccess();
  virtual void OnConfirmPlayStatusError( u32 errorCode );

#if defined(GF_PLATFORM_CTR)
  virtual void OnConfirmPlayStatusError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult ){}
#endif // defined(GF_PLATFORM_CTR)

  virtual void OnSaveDataSyncSuccess();
  virtual void OnSaveDataSyncError( u32 errorCode );

#if defined(GF_PLATFORM_CTR)
  virtual void OnSaveDataSyncError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult ){}
#endif // defined(GF_PLATFORM_CTR)

public:

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルエントリー確認通信の完了イベントを検知
  *
  * @param[in]  wifiMatchUpID     大会ID
  * @param[in]  gpfEntryFlg       PGLエントリーワーク（更新通信に必要）
  * @param[in]  wifiMatchupState  大会の進行情報（更新通信に必要）
  * @param[in]  isSignIn          連携サイトへの登録有無（true:登録済み）
  * @param[in]  timeZone          タイムゾーンオフセット
  */
  //------------------------------------------------------------------
  virtual void OnPGLGetWorldBattleStatusSuccess( u32 wifiMatchUpID, u16 gpfEntryFlg, u16 wifiMatchupState, bool isSignIn, s8 timezone );

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルエントリー状態更新通信の完了イベントを検知
  */
  //------------------------------------------------------------------
  virtual void OnPGLUpdateWorldBattleStatusSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief PGLバトルボックス登録通信の完了イベントを検知
  */
  //------------------------------------------------------------------
  virtual void OnPGLUploadBattleBoxSuccess( void );

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
  * @brief PGL通信のエラーイベントを検知
  *
  * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
  *
  * @param[in] commandType 完了したPGL通信コマンド
  * @param[in] httpStatus サーバから返却されたHTTPステータスコード
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus );

  //------------------------------------------------------------------
  /**
  * @brief PGL通信のレスポンスデータのエラーコードを検知
  *
  * @param[in]  commandType  完了したPGL通信コマンド
  * @param[in]  resultCode   PGLサーバから受信したレスポンスエラーコード
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode );

  //------------------------------------------------------------------
  /**
  * @brief PGL通信のエラーイベントを検知
  *
  * @param[in]  commandType  完了したPGL通信コマンド
  * @param[in]  result       エラー内容
  * @param[in]  errorCode    ネットワークエラーコード
  */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode );
#endif
private:

  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
  UpperView_Information*    m_pUpperView;
  LowerView_Selection*      m_pLowerView;

  u32 m_pglState;
  u32 m_wait;
#if defined(GF_PLATFORM_CTR)
  NetLib::Pgl::PGLConnector* m_pglConnector;
#endif
  u32 m_nextWifiMatchupState;
  u32 m_nextGpfEntryFlg;
  u32 m_updateStatusSeq;
  bool m_teamLockRequest;
  u32 m_wifiMatchUpID;

  void* m_pEncodeBuffer;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_FRIENDMATCHENTRY_H_INCLUDED)  */
