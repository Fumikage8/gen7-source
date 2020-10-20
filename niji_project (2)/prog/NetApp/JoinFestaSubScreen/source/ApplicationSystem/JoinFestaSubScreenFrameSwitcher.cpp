// ============================================================================
/*
 * @file JoinFestaSubScreenFrameSwitcher.cpp
 * @brief ジョインフェスタ下画面のフレームを切り替えるクラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenFrameSwitcher.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

NetAppLib::System::FrameID JoinFestaSubScreenFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  // 各フレーム共通の戻り値チェック
  if( frameResult == COMMON_RESULT_INVITATION )
  {
    // 戻り先は飛ばし元フレーム
    m_invitationReturnFrame = executeFrameID;

    // お誘いフレームに飛ばす
    return JOINFESTASUBSCREEN_FRAME_ID_INVITATION;
  }


  // 各フレーム専用の戻り値チェック
  switch( executeFrameID ){
  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_MAIN:       //! メインフレーム
    {
      switch( frameResult ){
      case MAIN_RESULT_BATTLE:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_BATTLE;
        break;
      case MAIN_RESULT_TRADE:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_TRADE;
        break;
      case MAIN_RESULT_ASK_END:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_ASK_END;
        break;
      case MAIN_RESULT_ALERT:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_ALERT;
        break;
      }
    }
    break;

  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_ASK_END:    //! 終了問いフレーム
    {
      switch( frameResult ){
      case AKS_END_RESULT_NO:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_MAIN;
        break;
      }
    }
    break;


  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_BATTLE:     //! 何の対戦をするか選択するフレーム
    {
      switch( frameResult ){
      case BATTLE_RESULT_NBR:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE;
        break;
      case BATTLE_RESULT_RETURN:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_MAIN;
        break;
      }
    }
    break;

  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE://! 対戦のルールを選択するフレーム
    {
      switch( frameResult ){
      case BATTLE_RULE_RESULT_BOSYUU:
        //nextFrameID = 
        break;
      case BATTLE_RULE_RESULT_RETURN:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_BATTLE;
        break;
      case BATTLE_RULE_RESULT_ALERT:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_ALERT;
        break;
      }
    }
    break;

  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_TRADE:      //! 何の交換をするか選択するフレーム
    {
      switch( frameResult ){
      case TRADE_RESULT_ALERT:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_ALERT;
        break;
      case TRADE_RESULT_RETURN:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_MAIN;
        break;
      }
    }
    break;

  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_INVITATION: //! お誘い通知フレーム
    {
      switch( frameResult ){
      case INVITATION_RESULT_RETURN:
        nextFrameID = m_invitationReturnFrame;
        break;
      case INVITATION_RESULT_ALERT:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_ALERT;
        break;
      }
    }
    break;
  
  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION: //! アトラクション中フレーム
    {
      if( frameResult == ATTRACTION_RETIRE )
      {
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION_RETIRE;
      }
    }
    break;
  
  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION_RETIRE: //! アトラクションリタイアフレーム
    // 次のフレームなし
    nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
    break;

  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_ALERT: //! アトラクションリタイアフレーム
    {
      switch( frameResult )
      {
        case ALEART_RESULT_RETURN_TO_MAIN:
          nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_MAIN;
        break;

        case ALEART_RESULT_RETURN_TO_BATTLE_RULE:
          nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE;
        break;

        case ALEART_RESULT_RETURN_TO_TRADE:
          nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_TRADE;
        break;
        
        case ALEART_RESULT_RETURN_TO_INVITATION:
          nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_INVITATION;
        break;
      }
    }
    break;

  // =================================================================================
  case JOINFESTASUBSCREEN_FRAME_ID_TUTORIAL:    //! チュートリアル中フレーム
    {
      switch( frameResult ){
      case TUTORIAL_RESULT_TO_MAIN:
        nextFrameID = JOINFESTASUBSCREEN_FRAME_ID_MAIN;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
