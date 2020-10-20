#if !defined(LIVEFRAMEDEF_H_INCLUDED)
#define LIVEFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrameDef.h
 * @date   2015/07/09 Thu. 13:12:30
 * @author muto_yu
 * @brief  ライブ大会Frame関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"

namespace NetApp  {
namespace Live  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    各フレームのID
  *  @note    NetAppLib::System::FrameIDとの互換を取る
  */
//------------------------------------------------------------------
enum  FrameID
{
  FRAME_Main    = NetAppLib::System::FRAME_ID_START,

  FRAME_ReceivePlayerPass,        /**<  選手証の受信              */
  FRAME_Registration,             /**<  チーム登録                */
  FRAME_CheckRemainingMatches,    /**<  残試合数確認              */
  FRAME_MatchMenu,                /**<  大会メニュー              */
  FRAME_ReplayMatch,              /**<  過去の対戦を振り返る      */
  FRAME_Deregistration,           /**<  登録解除                  */
  FRAME_Matching,                 /**<  マッチング（→対戦へ）    */
  FRAME_MatchResult,              /**<  対戦結果（←対戦から）    */

  FRAME_INVALID = NetAppLib::System::FRAME_ID_INVALID,
  FRAME_STARTUP = FRAME_Main
};



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief   各フレームの結果定義
  *  @note    NetAppLib::System::FrameResultとの互換を取る
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  main  */
  RESULT_MAIN_Exit,
  RESULT_MAIN_Replay,
  RESULT_MAIN_ReceivePlayerPass,
  RESULT_MAIN_ReceivePlayerPassQR,
  RESULT_MAIN_RegisterTeam,
  RESULT_MAIN_CheckRemainMatch,

  /*  receive player pass  */
  RESULT_RECEIVE_PLAYER_PASS_RegulationCheckPassed,
  RESULT_RECEIVE_PLAYER_PASS_Exit,

  /*  registration  */
  RESULT_REGISTRATION_TeamSelect,
  RESULT_REGISTRATION_Registered,
  RESULT_REGISTRATION_Exit,

  /*  checkRemainingMatches  */
  RESULT_CHECK_REMAINING_MATCHES_Complete,
  RESULT_CHECK_REMAINING_MATCHES_NotComplete,

  /*  matchMenu  */
  RESULT_MATCHMENU_Exit,
  RESULT_MATCHMENU_Start,
  RESULT_MATCHMENU_Replay,
  RESULT_MATCHMENU_UnregisterTeam,

  /*  replay  */
  RESULT_REPLAY_Replay,
  RESULT_REPLAY_Exit,
  RESULT_REPLAY_Exit_Fatal,

  /*  deregistration  */
  RESULT_DEREGISTRATION_Exit,
  RESULT_DEREGISTRATION_Unregistered,

  /*  matching  */
  RESULT_MATCHING_SendDataToCheatCheckROM,            /**<  不正チェックROMへの送信完了                     */
  RESULT_MATCHING_Cancel,                             /**<  データ送信のキャンセル、レギュレーション不一致  */
  RESULT_MATCHING_Success,                            /**<  マッチング成功                                  */

  /*  matchResult  */
  RESULT_MATCHRESULT_Exit,
  RESULT_MATCHRESULT_Exit_Fatal,

  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(const NetAppLib::System::FrameResult frameResult);



} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEFRAMEDEF_H_INCLUDED)  */
