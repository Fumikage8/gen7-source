#if !defined(BATTLESPOTFRAMEDEF_H_INCLUDED)
#define BATTLESPOTFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrameDef.h
 * @date   2015/10/02 Fri. 12:11:17
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameID
{
  /*  welcome  */
  FRAME_Welcome    = NetAppLib::System::FRAME_ID_START,

  /*  freeBattle  */
  /*  entry  */
  FRAME_FreeBattleEntry,

  /*  ratingBattle  */
  FRAME_RatingBattleEntry,

  /*  internetMatch  */
  FRAME_InternetMatchEntry,
  FRAME_InternetBattleMatching,
  FRAME_InternetAfterBattle,

  /*  friendMatch  */
  FRAME_FriendMatchEntry,
  FRAME_FriendBattleMatching,
  FRAME_FriendAfterBattle,

  FRAME_FreeBattleMatching,
  FRAME_FreeAfterBattle,
  FRAME_RatingBattleMatching,
  FRAME_RatingAfterBattle,

  FRAME_RatingAfterRuleSelect,

  FRAME_UpdateDisconnectCount,

  /*  debug  */
#if PM_DEBUG
  FRAME_ViewTest,
#endif

  FRAME_INVALID = NetAppLib::System::FRAME_ID_INVALID,
  FRAME_STARTUP = FRAME_Welcome
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  welcome  */
  RESULT_Welcome_Back,
  RESULT_Welcome_FreeBattle,
  RESULT_Welcome_RatingBattle,
  RESULT_Welcome_InternetMatch,
  RESULT_Welcome_FriendMatch,

  /*  freeBattle  */
  /*  entry  */
  RESULT_FreeBattleEntry_Back,
  RESULT_FreeBattleEntry_Next,

  /*  ratingBattle  */
  /*  entry  */
  RESULT_RatingBattleEntry_Back,
  RESULT_RatingBattleEntry_Next,

  /*  internetMatch  */
  /*  entry  */
  RESULT_InternetMatchEntry_Back,
  RESULT_InternetMatchEntry_TeamSelect,
  RESULT_InternetMatchEntry_Next,

  RESULT_InternetBattleMatching_Next,
  RESULT_InternetBattleMatching_Timeout,
  RESULT_InternetBattleMatching_InvalidPid,
  RESULT_InternetBattleMatching_CupIsOver,
  RESULT_InternetBattleMatching_Disconnect,

  RESULT_InternetAfterBattle_Next,
  RESULT_InternetAfterBattle_Exit,
  RESULE_InternetAfterBattle_BattleVideo,

  /*  friendmatch  */
  /*  entry  */
  RESULT_FriendMatchEntry_Back,
  RESULT_FriendMatchEntry_QR,
  RESULT_FriendMatchEntry_TeamSelect,
  RESULT_FriendMatchEntry_Next,

  RESULT_FriendBattleMatching_Next,
  RESULT_FriendBattleMatching_Timeout,
  RESULT_FriendBattleMatching_InvalidPid,
  RESULT_FriendBattleMatching_CupIsOver,
  RESULT_FriendBattleMatching_Disconnect,

  RESULT_FriendAfterBattle_Next,
  RESULT_FriendAfterBattle_Exit,
  RESULE_FriendAfterBattle_BattleVideo,

  //FRAME_FreeBattleMatching,
  RESULT_FreeBattleMatching_Next,
  RESULT_FreeBattleMatching_Timeout,
  RESULT_FreeBattleMatching_Disconnect,

  //FRAME_FreeAfterBattle,
  RESULT_FreeAfterBattle_Next,
  RESULT_FreeAfterBattle_Change,
  RESULT_FreeAfterBattle_Exit,
  RESULE_FreeAfterBattle_BattleVideo,

  //FRAME_RatingBattleMatching,
  RESULT_RatingBattleMatching_Next,
  RESULT_RatingBattleMatching_Timeout,
  RESULT_RatingBattleMatching_Disconnect,

  //FRAME_RatingAfterBattle,
  RESULT_RatingAfterBattle_Next,
  RESULT_RatingAfterBattle_Change,
  RESULT_RatingAfterBattle_Exit,
  RESULE_RatingAfterBattle_BattleVideo,

  //FRAME_RatingAfterRuleSelect
  RESULT_RatingAfterRuleSelect_Next,
  RESULT_RatingAfterRuleSelect_Back,

  //UpdateDisconnectCount
  RESULT_UpdateDisconnectCount_Next,
  /*  debug  */
#if PM_DEBUG
  RESULT_ViewTest_Exit,
#endif

  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(BattleSpotWork* pWork, const NetAppLib::System::FrameResult frameResult);


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAMEDEF_H_INCLUDED)  */
