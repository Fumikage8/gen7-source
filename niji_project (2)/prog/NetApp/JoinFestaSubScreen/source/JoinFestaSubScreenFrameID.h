// ============================================================================
/*
 * @file JoinFestaSubScreenFrameID.h
 * @brief ジョインフェスタ下画面のフレームIDを定義します。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_FRAMEID_H_INCLUDE )
#define JOINFESTASUBSCREEN_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)


enum JoinFestaSubScreenFrameID
{
  JOINFESTASUBSCREEN_FRAME_ID_MAIN,       //! メインフレーム
  JOINFESTASUBSCREEN_FRAME_ID_ASK_END,    //! 終了問いフレーム
  JOINFESTASUBSCREEN_FRAME_ID_BATTLE,     //! 何の対戦をするか選択するフレーム
  JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE,//! 対戦のルールを選択するフレーム
  JOINFESTASUBSCREEN_FRAME_ID_TRADE,      //! 何の交換をするか選択するフレーム
  JOINFESTASUBSCREEN_FRAME_ID_INVITATION, //! お誘い通知フレーム
  JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION, //! アトラクション中フレーム
  JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION_RETIRE, //! アトラクションリタイアフレーム
  JOINFESTASUBSCREEN_FRAME_ID_ALERT,      //! アラート表示フレーム
  JOINFESTASUBSCREEN_FRAME_ID_TUTORIAL,   //! チュートリアル中フレーム
  JOINFESTASUBSCREEN_FRAME_ID_BG_ONLY     //! BG表示のみフレーム
};

GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_FRAMEID_H_INCLUDE
