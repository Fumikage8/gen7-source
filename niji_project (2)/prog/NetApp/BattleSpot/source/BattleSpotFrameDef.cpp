//==============================================================================
/**
 * @file   BattleSpotFrameDef.cpp
 * @date   2015/10/02 Fri. 12:26:42
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./System/BattleSpotWork.h"
#include  "./BattleSpotFrameDef.h"


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(BattleSpotWork* pWork, const NetAppLib::System::FrameResult frameResult)
{
  switch(frameResult)
  {
    /*  welcome  */
    case  RESULT_Welcome_Back:            return  FRAME_INVALID;
    case  RESULT_Welcome_FreeBattle:      return  FRAME_FreeBattleEntry;
    case  RESULT_Welcome_RatingBattle:    return  FRAME_RatingBattleEntry;
    case  RESULT_Welcome_InternetMatch:   return  FRAME_InternetMatchEntry;
    case  RESULT_Welcome_FriendMatch:     return  FRAME_FriendMatchEntry;

    /*  freeBattle  */
    /*  entry  */
    case  RESULT_FreeBattleEntry_Back:    return  FRAME_Welcome;
    case  RESULT_FreeBattleEntry_Next:    return  FRAME_INVALID;

    /*  ratingBattle  */
    /*  entry  */
    case  RESULT_RatingBattleEntry_Back:  return  FRAME_Welcome;
    case  RESULT_RatingBattleEntry_Next:    return  FRAME_RatingAfterRuleSelect;
    //case  RESULT_RatingBattleEntry_Next:    return  FRAME_INVALID;

    /*  internetMatch  */
    /*  entry  */
    case  RESULT_InternetMatchEntry_Back: return  FRAME_Welcome;
    case  RESULT_InternetMatchEntry_TeamSelect: return  FRAME_INVALID;
    case  RESULT_InternetMatchEntry_Next: return  FRAME_InternetBattleMatching;

    case  RESULT_InternetBattleMatching_Next: return FRAME_UpdateDisconnectCount;
    case  RESULT_InternetBattleMatching_Timeout: return FRAME_Welcome;
    case  RESULT_InternetBattleMatching_InvalidPid: return FRAME_Welcome;
    case  RESULT_InternetBattleMatching_CupIsOver: return FRAME_InternetMatchEntry;//リタイアに流す為にこっち
    case  RESULT_InternetBattleMatching_Disconnect: return FRAME_InternetAfterBattle;

    case  RESULT_InternetAfterBattle_Next: return FRAME_InternetBattleMatching;
    case  RESULT_InternetAfterBattle_Exit: return FRAME_Welcome;
    case  RESULE_InternetAfterBattle_BattleVideo: return FRAME_UpdateDisconnectCount;

    /*  friendMatch  */
    /*  entry  */
    case  RESULT_FriendMatchEntry_Back:   return  FRAME_Welcome;
    case  RESULT_FriendMatchEntry_QR:     return  FRAME_INVALID;
    case  RESULT_FriendMatchEntry_TeamSelect: return FRAME_INVALID;
    case  RESULT_FriendMatchEntry_Next: return FRAME_FriendBattleMatching;

    case  RESULT_FriendBattleMatching_Next: return FRAME_UpdateDisconnectCount;
    case  RESULT_FriendBattleMatching_Timeout: return FRAME_Welcome;
    case  RESULT_FriendBattleMatching_InvalidPid: return FRAME_Welcome;
    case  RESULT_FriendBattleMatching_CupIsOver: return FRAME_FriendMatchEntry;//開催終了に流す為にこっち
    case  RESULT_FriendBattleMatching_Disconnect: return FRAME_FriendAfterBattle;

    case  RESULT_FriendAfterBattle_Next: return FRAME_FriendBattleMatching;
    case  RESULT_FriendAfterBattle_Exit: return FRAME_Welcome;
    case  RESULE_FriendAfterBattle_BattleVideo: return FRAME_UpdateDisconnectCount;


    //FRAME_FreeBattleMatching,
    case  RESULT_FreeBattleMatching_Next: return  FRAME_UpdateDisconnectCount;
    case  RESULT_FreeBattleMatching_Timeout: return FRAME_Welcome;
    case  RESULT_FreeBattleMatching_Disconnect: return FRAME_FreeAfterBattle;

    //FRAME_FreeAfterBattle,
    case  RESULT_FreeAfterBattle_Next:    return  FRAME_FreeBattleMatching;
    case  RESULT_FreeAfterBattle_Change:    return  FRAME_INVALID;
    case  RESULT_FreeAfterBattle_Exit:    return  FRAME_Welcome;
    case  RESULE_FreeAfterBattle_BattleVideo: return FRAME_UpdateDisconnectCount;


    //FRAME_RatingBattleMatching,
    case  RESULT_RatingBattleMatching_Next: return FRAME_UpdateDisconnectCount;
    case  RESULT_RatingBattleMatching_Timeout: return FRAME_Welcome;
    case  RESULT_RatingBattleMatching_Disconnect: return FRAME_RatingAfterBattle;

    //FRAME_RatingAfterBattle,
    case  RESULT_RatingAfterBattle_Next:  return  FRAME_RatingBattleMatching;
    case  RESULT_RatingAfterBattle_Change:  return  FRAME_INVALID;
    case  RESULT_RatingAfterBattle_Exit:  return  FRAME_Welcome;
    case  RESULE_RatingAfterBattle_BattleVideo: return FRAME_UpdateDisconnectCount;

    //RatingAfterRuleSelect
    case  RESULT_RatingAfterRuleSelect_Next:  return FRAME_INVALID;
    case  RESULT_RatingAfterRuleSelect_Back:  return FRAME_Welcome;

    //UpdateDisconnectCount
    case  RESULT_UpdateDisconnectCount_Next:  return FRAME_INVALID;

  }
  return  FRAME_INVALID; 
}


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */




