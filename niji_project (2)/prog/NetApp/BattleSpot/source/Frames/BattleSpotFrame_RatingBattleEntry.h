#if !defined(BATTLESPOTFRAME_RATINGBATTLEENTRY_H_INCLUDED)
#define BATTLESPOTFRAME_RATINGBATTLEENTRY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_RatingBattleEntry.h
 * @date   2015/10/02 Fri. 13:31:47
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
#include  "../Views/BattleSpotViewContainer.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"

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
class Frame_RatingBattleEntry
  : public    NetAppLib::System::NetAppFrameBase
  , public NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener
#if defined( GF_PLATFORM_CTR )
  , public gflnet2::boss::BossEventListener
  , public gflnet2::nex::NexCommunityListener
#endif // defined( GF_PLATFORM_CTR )
  , protected LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_RatingBattleEntry );

  enum
  {
    REGSEQ_INIT = 0,
    REGSEQ_WAIT,
    REGSEQ_FIND_MY_COMMUNITY,
    REGSEQ_FIND_MY_COMMUNITY_WAIT,
    REGSEQ_LEAVE_COMMUNITY,
    REGSEQ_LEAVE_COMMUNITY_WAIT,
    REGSEQ_FIND_OFFICIALCOMMUNITY,
    REGSEQ_FIND_OFFICIALCOMMUNITY_WAIT,
    REGSEQ_JOIN_COMMUNITY,
    REGSEQ_JOIN_COMMUNITY_WAIT,
    REGSEQ_DELETED,
    REGSEQ_UPDATE,
    REGSEQ_COMPLETE,
    REGSEQ_ERROR,
    REGSEQ_SD_FULL,
    REGSEQ_OK
  };

  enum
  {
    REGULATION_SINGLE = 0,
    REGULATION_DOUBLE,
    REGULATION_SPECIAL,
    REGULATION_WCS,
    REGULATION_NUM,
  };

  enum
  {
    AT_INDEX_communityID = 0,
    MAX_MY_COMMUNITY_NUM = 16,
    MAX_COMMUNITY_NUM = 4
  };

public:
  Frame_RatingBattleEntry(BattleSpotWork* pWork);
  virtual ~Frame_RatingBattleEntry();

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

  void  ExitSelf(const FrameResult frameResult = RESULT_RatingBattleEntry_Back);

  bool UpdateRegulation( void );
  void UpdatePGLCheck( void );

#ifdef  GF_PLATFORM_CTR
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

public:

  virtual void OnConfirmPlayStatusSuccess();
  virtual void OnConfirmPlayStatusError( u32 errorCode );

#if defined(GF_PLATFORM_CTR)
  virtual void OnConfirmPlayStatusError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult ){}
#endif // defined(GF_PLATFORM_CTR)

private:

  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
  UpperView_Information*           m_pUpperView;
  LowerView_Selection*      m_pLowerView;



  u32 m_pglState;

  u32 m_regSeq;
  u32 m_SerialID;

  Regulation* m_pRegulation[REGULATION_NUM];

  Regulation* m_pTempRegulation;
  ExtSavedata::RegulationExtSaveData       m_RegulationExtSaveData;

  u32 m_communityIDArray[REGULATION_NUM];
  bool m_isDeleteComplete;
  bool m_isNewExists;

#if defined(GF_PLATFORM_CTR)
  gflnet2::nex::NexMatchMakingClient* m_pMatchMakeClient;//コミュニティ参加用

  u32 m_myCommunityIDArray[MAX_MY_COMMUNITY_NUM];//自分が参加しているコミュニティID
  nn::nex::GatheringID m_joinGatheringID[MAX_MY_COMMUNITY_NUM];//参加すべきコミュニティのギャザリングID
  u32 m_joinCommunityID[MAX_MY_COMMUNITY_NUM];//参加すべきコミュニティのコミュニティID
  nn::nex::GatheringID m_leaveGatheringID[MAX_MY_COMMUNITY_NUM];//脱退すべきコミュニティのギャザリングID
  u32 m_joinCommunityNum;
  u32 m_leaveCommunityNum;
#endif
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_RATINGBATTLEENTRY_H_INCLUDED)  */
