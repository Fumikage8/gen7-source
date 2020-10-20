// ============================================================================
/*
 * @file NBRFrameID.h
 * @brief 通信対戦受付アプリのフレームIDを定義します。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_FRAMEID_H_INCLUDE )
#define NBR_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)


enum NBRFrameID
{
  NBR_FRAME_ID_RULE_SELECTOR_PRIORITY,  //! ルール選択する人の優先順位決め
  NBR_FRAME_ID_RULE_SELECTOR_DECIDE,    //! ルール選択する人を決定
  NBR_FRAME_ID_RULE_SELECT_APP_CALL,    //! ルール選択アプリ呼び出し
  NBR_FRAME_ID_RULE_SELECT_WAIT,        //! ルール選択待ち
  NBR_FRAME_ID_RULE_SELECT_APP_END,     //! ルール選択アプリ戻り
  NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL,//! ルール選択アプリキャンセル戻り
  NBR_FRAME_ID_TEAM_SELECT,             //! チーム選択
  NBR_FRAME_ID_TEAM_SELECT_APP_END,     //! チーム選択アプリ終了
  NBR_FRAME_ID_TEAM_SELECT_WAIT,        //! チーム選択待ち
  NBR_FRAME_ID_POKEPARTY_CHECK,         //! ポケパーティ検査
  NBR_FRAME_ID_MULTI_PLAYER_SELECT,     //! マルチチーム振り分け
  NBR_FRAME_ID_MULTI_PARENT_WAIT,       //! マルチ親待ち
  NBR_FRAME_ID_BATTLE_START,            //! バトルへ
  NBR_FRAME_ID_CONTINUE,                //! 続けて遊ぶ
  NBR_FRAME_ID_APP_END,                 //! アプリ終了
};

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_FRAMEID_H_INCLUDE
