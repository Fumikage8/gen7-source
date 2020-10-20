// ============================================================================
/*
 * @file NBRFrameSwitcher.cpp
 * @brief 通信対戦受付アプリのフレームを切り替えるクラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/ApplicationSystem/NBRFrameSwitcher.h"
#include "NetApp/NBR/source/NBRFrameID.h"
#include "NetApp/NBR/source/NBRFrameResult.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// 通信バトルイベント
#include "NetStatic/NetEvent/include/NetworkBattleEvent.h" // for PersonalData

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NetAppLib::System::FrameID NBRFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID ){
  // =================================================================================
  case NBR_FRAME_ID_RULE_SELECTOR_PRIORITY:  ///< ルール選択する人の優先順位決め
    {
      switch( frameResult ){
      case RuleSelectorPriorityResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case RuleSelectorPriorityResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_RULE_SELECTOR_DECIDE:    ///< ルール選択する人を決定
    {
      switch( frameResult ){
      case RuleSelectorDecideResult::TO_RULE_SELECT_APP_CALL:
        nextFrameID = NBR_FRAME_ID_RULE_SELECT_APP_CALL;
        break;
      case RuleSelectorDecideResult::TO_RULE_SELECT_WAIT:
        nextFrameID = NBR_FRAME_ID_RULE_SELECT_WAIT;
        break;
      case RuleSelectorDecideResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_RULE_SELECT_APP_CALL:    ///< ルール選択アプリ呼び出し
    {
      switch( frameResult ){
      case RuleSelectAppCallResult::RULE_SELECT_APP_CALL:
        m_pAppParam->returnMode = RET_RULE_SELECT;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_RULE_SELECT_WAIT:        ///< ルール選択待ち
    {
      switch( frameResult ){
      case RuleSelectWaitResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case RuleSelectWaitResult::TO_TEAM_SELECT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT;
        break;
      case RuleSelectWaitResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_RULE_SELECT_APP_END:     ///< ルール選択アプリ戻り
    {
      switch( frameResult ){
      case RuleSelectAppEndResult::TO_RULE_SELECT_APP_CALL:
        nextFrameID = NBR_FRAME_ID_RULE_SELECT_APP_CALL;
        break;
      case RuleSelectAppEndResult::TO_TEAM_SELECT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT;
        break;
      case RuleSelectAppEndResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL://! ルール選択アプリキャンセル戻り
    {
      switch( frameResult ){
      case RuleSelectAppEndCancelResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case RuleSelectAppEndCancelResult::TO_RULE_SELECT_APP_CALL:
        nextFrameID = NBR_FRAME_ID_RULE_SELECT_APP_CALL;
        break;
      case RuleSelectAppEndCancelResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_TEAM_SELECT:             ///< チーム選択
    {
      switch( frameResult ){
      case TeamSelectResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case TeamSelectResult::TEAM_SELECT_APP_CALL:
        m_pAppParam->returnMode = RET_TEAM_SELECT;
        break;
      case TeamSelectResult::TO_TEAM_SELECT_WAIT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT_WAIT;
        break;
      case TeamSelectResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_TEAM_SELECT_APP_END:     ///< チーム選択アプリ終了
    {
      switch( frameResult ){
      case TeamSelectAppEndResult::TO_TEAM_SELECT_WAIT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT_WAIT;
        break;
      case TeamSelectAppEndResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case TeamSelectAppEndResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_TEAM_SELECT_WAIT:        ///< チーム選択待ち
    {
      switch( frameResult ){
      case TeamSelectWaitResult::TO_POKEPARTY_CHECK:
        nextFrameID = NBR_FRAME_ID_POKEPARTY_CHECK;
        break;
      case TeamSelectWaitResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case TeamSelectWaitResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_POKEPARTY_CHECK:         //! ポケパーティ検査
    {
      switch( frameResult ){
      case PokePartyCheckResult::TO_BATTLE_START:
        nextFrameID = NBR_FRAME_ID_BATTLE_START;
        break;
      case PokePartyCheckResult::TO_MULTI_PLAYER_SELECT:
        nextFrameID = NBR_FRAME_ID_MULTI_PLAYER_SELECT;
        break;
      case PokePartyCheckResult::TO_MULTI_PARENT_WAIT:
        nextFrameID = NBR_FRAME_ID_MULTI_PARENT_WAIT;
        break;
      case PokePartyCheckResult::TO_TEAM_SELECT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT;
        break;
      case PokePartyCheckResult::TO_TEAM_SELECT_WAIT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT_WAIT;
        break;
      case PokePartyCheckResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_MULTI_PLAYER_SELECT:     ///< マルチチーム振り分け
    {
      switch( frameResult ){
      case MultiPlayerSelectResult::TO_BATTLE_START:
        nextFrameID = NBR_FRAME_ID_BATTLE_START;
        break;
      case MultiPlayerSelectResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_MULTI_PARENT_WAIT:       ///< マルチ親待ち
    {
      switch( frameResult ){
      case MultiParentWaitResult::TO_BATTLE_START:
        nextFrameID = NBR_FRAME_ID_BATTLE_START;
        break;
      case MultiParentWaitResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_BATTLE_START:            ///< バトルへ
    {
      switch( frameResult ){
      case BattleStartResult::TIMING_FINISH:
        // セットされてるバトルのパラメータをコピー
        {
          // 選択したBGM
          m_pAppParam->bgm          = m_pNBRWork->m_bgm;
          // 戦う位置
          m_pAppParam->standingPos  = m_pNBRWork->GetMinePlace();

          // 個々人のデータ
          for( int btlClientId=0; btlClientId<BTL_CLIENT_NUM; btlClientId++ )
          {
            // ポケパーティ
            pml::PokeParty* party = m_pNBRWork->GetPokePartyFromBtlClientId( static_cast<BTL_CLIENT_ID>(btlClientId) );
            if( party != NULL )
            {
              // 存在する場合のみコピー
              m_pAppParam->pPersonalData[ btlClientId ]->pPokeParty->CopyFrom( *party );
              m_pAppParam->pPersonalData[ btlClientId ]->qrTeam = m_pNBRWork->IsQrTeamFromBtlClientId( static_cast<BTL_CLIENT_ID>(btlClientId) );
            }
            // マイステータス
            Savedata::MyStatus* myStatus = m_pNBRWork->GetMyStatusFromBtlClientId( static_cast<BTL_CLIENT_ID>(btlClientId) );
            if( myStatus != NULL )
            {
              m_pAppParam->pPersonalData[ btlClientId ]->pMyStatus->CopyFrom( *myStatus );
            }
          }

        }
        m_pAppParam->returnMode   = RET_BATTLE;
        break;
      }
    }
    break;

  // =================================================================================
  case NBR_FRAME_ID_CONTINUE:                ///< 続けて遊ぶ
    {
      switch( frameResult ){
      case ContinueResult::TO_RULE_SELECTOR_DECIDE:
        nextFrameID = NBR_FRAME_ID_RULE_SELECTOR_DECIDE;
        break;
      case ContinueResult::TO_TEAM_SELECT:
        nextFrameID = NBR_FRAME_ID_TEAM_SELECT;
        break;
      case ContinueResult::APP_END:
        nextFrameID = NBR_FRAME_ID_APP_END;
        break;
      }
    }
    break;

  case NBR_FRAME_ID_APP_END:
    {
      m_pAppParam->returnMode = RET_END;
      // 次のフレームを無しにしてアプリ終了させる
      nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
