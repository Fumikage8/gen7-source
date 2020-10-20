// ============================================================================
/*
 * @file JoinFestaSubScreenFrameResult.h
 * @brief ジョインフェスタ下画面のリザルトコードを定義します。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_FRAMERESULT_H_INCLUDE )
#define JOINFESTASUBSCREEN_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)

// 各フレームで共通の戻り値
enum CommonResultCode
{
  COMMON_RESULT_INVITATION = 0xff,
};

// メインフレームの戻り値
enum MainResultCode
{
  MAIN_RESULT_BATTLE,
  MAIN_RESULT_TRADE,
  MAIN_RESULT_ASK_END,
  MAIN_RESULT_ALERT,
};

// 終了確認フレームの戻り値
enum AskEndResultCode
{
  AKS_END_RESULT_NO,
};

// バトル選択フレームの戻り値
enum BattleResultCode
{
  BATTLE_RESULT_NBR,
  BATTLE_RESULT_RETURN,
};

// バトルルール選択フレームの戻り値
enum BattleRuleResultCode
{
  BATTLE_RULE_RESULT_BOSYUU,
  BATTLE_RULE_RESULT_RETURN,
  BATTLE_RULE_RESULT_ALERT,
};

// 交換選択フレームの戻り値
enum TradeResultCode
{
  TRADE_RESULT_ALERT,
  TRADE_RESULT_RETURN,
};

// お誘いフレームの戻り値
enum InvitationResultCode
{
  INVITATION_RESULT_RETURN,
  INVITATION_RESULT_ALERT,
};

// アトラクション中フレームの戻り値
enum AttractionResultCode
{
  ATTRACTION_RETIRE,
};

// アラート表示フレームの戻り値
enum AlertResultCode
{
  ALEART_RESULT_RETURN_TO_MAIN,
  ALEART_RESULT_RETURN_TO_BATTLE_RULE,
  ALEART_RESULT_RETURN_TO_TRADE,
  ALEART_RESULT_RETURN_TO_INVITATION,
};

// チュートリアル中フレームの戻り値
enum TutorialResultCode
{
  TUTORIAL_RESULT_TO_MAIN,
};


GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_FRAMERESULT_H_INCLUDE
