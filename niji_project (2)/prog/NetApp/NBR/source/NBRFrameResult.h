// ============================================================================
/*
 * @file NBRFrameResult.h
 * @brief 通信対戦受付アプリのリザルトコードを定義します。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_FRAMERESULT_H_INCLUDE )
#define NBR_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)

//=================================================================================
GFL_NAMESPACE_BEGIN(RuleSelectorPriorityResult)
// NBR_FRAME_ID_RULE_SELECTOR_PRIORITY 
// ルール選択する人の優先順位決め
enum
{
  TO_RULE_SELECTOR_DECIDE,  //! ルール選択する人決定へ
  APP_END,                  //! アプリ終了  
};
GFL_NAMESPACE_END(RuleSelectorPriorityResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(RuleSelectorDecideResult)
// NBR_FRAME_ID_RULE_SELECTOR_DECIDE
// ルール選択する人を決定
enum
{
  TO_RULE_SELECT_APP_CALL,  //! ルール選択アプリ呼び出し
  TO_RULE_SELECT_WAIT,      //! 
  APP_END,                  //! アプリ終了  
};
GFL_NAMESPACE_END(RuleSelectorDecideResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(RuleSelectAppCallResult)
// NBR_FRAME_ID_RULE_SELECT_APP_CALL
// ルール選択アプリ呼び出し
enum
{
  RULE_SELECT_APP_CALL,     //! ルール選択アプリ呼び出し
};
GFL_NAMESPACE_END(RuleSelectAppCallResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(RuleSelectWaitResult)
// NBR_FRAME_ID_RULE_SELECT_WAIT
// ルール選択待ち
enum
{
  TO_RULE_SELECTOR_DECIDE,    //! ルール選択者決めまで戻る
  TO_TEAM_SELECT,             //! バトルチーム選択へ
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(RuleSelectWaitResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(RuleSelectAppEndResult)
// NBR_FRAME_ID_RULE_SELECT_APP_END
// ルール選択アプリ戻り
enum
{
  TO_RULE_SELECT_APP_CALL,    //! ルール選択アプリ呼び出しへ戻る
  TO_TEAM_SELECT,             //! バトルチーム選択へ
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(RuleSelectAppEndResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(RuleSelectAppEndCancelResult)
// NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL
// ルール選択アプリ戻り
enum
{
  TO_RULE_SELECTOR_DECIDE,    //! ルール選択者決めまで戻る
  TO_RULE_SELECT_APP_CALL,    //! ルール選択アプリ呼び出しへ戻る
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(RuleSelectAppEndCancelResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(TeamSelectResult)
// NBR_FRAME_ID_TEAM_SELECT
// チーム選択
enum
{
  TO_RULE_SELECTOR_DECIDE,    //! ルール選択する人を決定まで戻る
  TEAM_SELECT_APP_CALL,       //! チーム選択アプリ呼び出し
  TO_TEAM_SELECT_WAIT,        //! チーム選択待ちへ
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(TeamSelectResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(TeamSelectAppEndResult)
// NBR_FRAME_ID_TEAM_SELECT_APP_END
// チーム選択アプリ終了
enum
{
  TO_TEAM_SELECT_WAIT,        ///
  TO_RULE_SELECTOR_DECIDE,    //! ルール選択する人を決定まで戻る
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(TeamSelectAppEndResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(TeamSelectWaitResult)
// NBR_FRAME_ID_TEAM_SELECT_WAIT
// チーム選択待ち
enum
{
  TO_POKEPARTY_CHECK,         //! ポケパーティ検査へ
  TO_RULE_SELECTOR_DECIDE,    //! ルール選択する人を決定まで戻る
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(TeamSelectWaitResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(PokePartyCheckResult)
// NBR_FRAME_ID_POKEPARTY_CHECK
// ポケパーティ検査
enum
{
  TO_BATTLE_START,            //! バトルへ
  TO_MULTI_PLAYER_SELECT,     //! マルチチーム振り分け
  TO_MULTI_PARENT_WAIT,       //! マルチ親待ち
  TO_TEAM_SELECT,             //! チーム選択まで戻る
  TO_TEAM_SELECT_WAIT,        //! チーム選択待ちまで戻る
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(PokePartyCheckResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(MultiPlayerSelectResult)
// NBR_FRAME_ID_MULTI_PLAYER_SELECT
// マルチチーム振り分け
enum
{
  TO_BATTLE_START,            //! バトルへ
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(MultiPlayerSelectResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(MultiParentWaitResult)
// NBR_FRAME_ID_MULTI_PARENT_WAIT
// マルチ親待ち
enum
{
  TO_BATTLE_START,            //! バトルへ
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(MultiParentWaitResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(BattleStartResult)
// NBR_FRAME_ID_BATTLE_START
// バトルへ
enum
{
  TIMING_FINISH,              //! 同期完了
};
GFL_NAMESPACE_END(BattleStartResult)

//=================================================================================
GFL_NAMESPACE_BEGIN(ContinueResult)
// NBR_FRAME_ID_CONTINUE
// 続けて遊ぶ
enum
{
  TO_RULE_SELECTOR_DECIDE,    //! ルール選択する人を決定まで戻る
  TO_TEAM_SELECT,             //! バトルチーム選択へ
  APP_END,                    //! アプリ終了  
};
GFL_NAMESPACE_END(ContinueResult)

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_FRAMERESULT_H_INCLUDE
