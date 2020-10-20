// ============================================================================
/*
 * @file QuickMatchAppParam.h
 * @brief クイックマッチアプリを起動する為のパラメータです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_APPPARAM_H_INCLUDE )
#define QUICKMATCH_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "Battle/Regulation/include/Regulation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)

enum APP_RESULT
{
  APP_RESULT_BATTLE = 0,
  APP_RESULT_TRADE,
  APP_RESULT_EXIT
};



struct APP_PARAM
{
  APP_PARAM()
    : m_isEnd(false)
    , m_result(APP_RESULT_EXIT)
    , m_rule( Regulation::BATTLE_RULE_SINGLE )
    , m_isHost( false )//ルールを選ぶ人
    , m_isBattleTreeMode( false )
    , m_isBattleTreeConnected( false )
  {
  };

  bool m_isEnd;
  APP_RESULT m_result;
  Regulation::BATTLE_RULE m_rule;
  bool m_isHost;
  bool m_isBattleTreeMode;
  bool m_isBattleTreeConnected;
};


GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCH_APPPARAM_H_INCLUDE
