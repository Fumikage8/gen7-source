//==============================================================================
/**
 * @file   LiveFrameDef.cpp
 * @date   2015/07/09 Thu. 14:51:52
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./LiveFrameDef.h"


namespace NetApp  {
namespace Live  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(const NetAppLib::System::FrameResult frameResult)
{
  switch(frameResult)
  {
    /*  main  */
    case  RESULT_MAIN_Exit:                                   return  FRAME_INVALID;
    case  RESULT_MAIN_Replay:                                 return  FRAME_MatchMenu;
    case  RESULT_MAIN_ReceivePlayerPass:                      return  FRAME_ReceivePlayerPass;
    case  RESULT_MAIN_ReceivePlayerPassQR:                    return  FRAME_INVALID;      /**<  QRReader呼び出し  */
    case  RESULT_MAIN_RegisterTeam:                           return  FRAME_Registration;
    case  RESULT_MAIN_CheckRemainMatch:                       return  FRAME_CheckRemainingMatches;

    /*  receivePlayerPass  */
    case  RESULT_RECEIVE_PLAYER_PASS_RegulationCheckPassed:   return  FRAME_Registration;
    case  RESULT_RECEIVE_PLAYER_PASS_Exit:                    return  FRAME_Main;

    /*  registration  */
    case  RESULT_REGISTRATION_TeamSelect:                     return  FRAME_INVALID;      /**<  teamSelect呼び出し  */
    case  RESULT_REGISTRATION_Registered:                     return  FRAME_CheckRemainingMatches;
    case  RESULT_REGISTRATION_Exit:                           return  FRAME_Main;

    /*  CheckRemainingMatches  */
    case  RESULT_CHECK_REMAINING_MATCHES_Complete:            return  FRAME_Main;
    case  RESULT_CHECK_REMAINING_MATCHES_NotComplete:         return  FRAME_MatchMenu;

    /*  matchMenu  */
    case  RESULT_MATCHMENU_Exit:                              return  FRAME_Main;
    case  RESULT_MATCHMENU_Start:                             return  FRAME_Matching;
    case  RESULT_MATCHMENU_Replay:                            return  FRAME_ReplayMatch;
    case  RESULT_MATCHMENU_UnregisterTeam:                    return  FRAME_Deregistration;

    /*  replay  */
    case  RESULT_REPLAY_Replay:                               return  FRAME_INVALID;      /**<  バトル再生呼び出し  */
    case  RESULT_REPLAY_Exit:                                 return  FRAME_MatchMenu;
    case  RESULT_REPLAY_Exit_Fatal:                           return  FRAME_INVALID;      /**<  内部Fatal           */

    /*  deregistration  */
    case  RESULT_DEREGISTRATION_Exit:                         return  FRAME_MatchMenu;
    case  RESULT_DEREGISTRATION_Unregistered:                 return  FRAME_Main;

    /*  matching  */
    case  RESULT_MATCHING_SendDataToCheatCheckROM:            return  FRAME_MatchMenu;    /**<  不正チェックROMへの送信完了                     */
    case  RESULT_MATCHING_Cancel:                             return  FRAME_MatchMenu;    /**<  データ送信のキャンセル、レギュレーション不一致  */
    case  RESULT_MATCHING_Success:                            return  FRAME_INVALID;      /**<  マッチング成功：バトル呼び出し                  */

    /*  matchResult  */
    case  RESULT_MATCHRESULT_Exit:                            return  FRAME_CheckRemainingMatches;
    case  RESULT_MATCHRESULT_Exit_Fatal:                      return  FRAME_INVALID;      /**<  内部Fatal           */


    default:
      return  FRAME_INVALID;
  }
}



} /*  namespace Live    */
} /*  namespace NetApp  */
