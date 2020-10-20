#if !defined(BATTLESPOTFRAME_INTERNETMATCHENTRY_H_INCLUDED)
#define BATTLESPOTFRAME_INTERNETMATCHENTRY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_InternetMatchEntry.h
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

#include <gflnet2/include/boss/gflnet2_BossEventListener.h>

#include  "ExtSavedata/include/RegulationExtSaveData.h"

#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexCommunityListener.h>

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
class Frame_InternetMatchEntry
  : public    NetAppLib::System::NetAppFrameBase
  , public NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener
  , public NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener
#if defined(GF_PLATFORM_CTR)
  , public NetLib::Pgl::PGLConnectorListener
  , public gflnet2::boss::BossEventListener
  , public gflnet2::nex::NexCommunityListener
#endif
  , protected LowerView_Selection::IEventHandler
  , protected LowerView_Card::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_InternetMatchEntry );
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
    REGSEQ_INIT = 0,
    REGSEQ_WAIT,
    REGSEQ_LEAVE_COMMUNITY,
    REGSEQ_LEAVE_COMMUNITY_WAIT,
    REGSEQ_JOIN_COMMUNITY,
    REGSEQ_JOIN_COMMUNITY_WAIT,
    REGSEQ_ERROR,
    REGSEQ_OK
  };

  enum
  {
    AT_INDEX_communityID = 0,
    REGULATION_NUM = 1,
    DUMMY_WAIT = 15,
    ALEADY_JOINE_ERROR = 60338,
  };

public:
  Frame_InternetMatchEntry(BattleSpotWork* pWork);
  virtual ~Frame_InternetMatchEntry();

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

  void  ExitSelf(const FrameResult frameResult = RESULT_InternetMatchEntry_Back);

  void SetViewModeForWork( void );

  void UpdatePGLCheck( void );
  bool UpdateRegulation( void );
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



    // BossEventListener
  virtual void OnBossNewArrivalData( const u32 serialId , const bool isFinalData );
  virtual void OnBossNewArrivalDataEmpty( void );
  virtual bool OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody );
  virtual void OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum );
  virtual void OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result );
  virtual void OnBossError( const nn::Result& result );
  virtual void OnBossTaskError( const char* pTaskName , const u32 errorCode );

  //コミュニティリスナ
  //------------------------------------------------------------------
  /**
  * @brief コミュニティの作成完了通知
  *
  * @param[in] gatheringId 作成されたセッションのギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnCommunityCreateSuccess( const nn::nex::GatheringID& gatheringId ){ GFL_PRINT("ProcUpdateRatingbattle::OnCommunityCreateSuccess\n"); };

  //------------------------------------------------------------------
  /**
  * @brief コミュニティの検索完了通知
  *
  * @param[in] communityList 検索されたコミュニティリスト
  */
  //------------------------------------------------------------------
  virtual void OnCommunitySearchSuccess( nn::nex::qList<nn::nex::Community>* communityList );

  //------------------------------------------------------------------
  /**
  * @brief コミュニティへの参加完了通知
  *
  * @param[in] gatheringId 参加したギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnCommunityJoinSuccess( const nn::nex::GatheringID& gatheringId );

  //------------------------------------------------------------------
  /**
  * @brief コミュニティの退出完了通知
  *
  * @param[in] gatheringId 退出したギャザリングID
  */
  //------------------------------------------------------------------
  virtual void OnCommunityLeaveSuccess( const nn::nex::GatheringID& gatheringId );

  //------------------------------------------------------------------
  /**
  * @brief コミュニティAPIのエラー終了通知
  *
  * @param[in] result    エラー理由
  * @param[in] errorCode エラーアプレットに渡すエラーコード
  */
  //------------------------------------------------------------------
  virtual void OnNexCommunityError( const nn::nex::qResult& result, u32 errorCode );

  //------------------------------------------------------------------
  /**
  * @brief 自分が参加しているコミュニティの検索結果をチェック
  *
  * @param[in] communityList 検索されたコミュニティリスト
  */
  //------------------------------------------------------------------
  void CheckMyCommunityList( nn::nex::qList<nn::nex::Community>* communityList );


  //------------------------------------------------------------------
  /**
  * @brief 参加可能な公式コミュニティの検索結果をチェック
  *
  * @param[in] communityList 検索されたコミュニティリスト
  */
  //------------------------------------------------------------------
  void CheckOfficialCommunityList( nn::nex::qList<nn::nex::Community>* communityList );









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

  u32 m_regSeq;
  u32 m_SerialID;
  u32 m_communityID;

#if defined(GF_PLATFORM_CTR)
  gflnet2::nex::NexMatchMakingClient* m_pMatchMakeClient;//コミュニティ参加用
#endif
  u32 m_wifiMatchUpID;
  bool m_firstPglCheckDone;

  void* m_pEncodeBuffer;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_INTERNETMATCHENTRY_H_INCLUDED)  */
