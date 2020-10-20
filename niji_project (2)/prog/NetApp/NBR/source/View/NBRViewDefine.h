// ============================================================================
/*
 * @file NBRViewDefine.h
 * @brief 通信対戦受付アプリ　ビュー定義系
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBRVIEWDEFINE_H_INCLUDE )
#define NBRVIEWDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>

#include "NetApp/NBR/source/NBRSoundDefine.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/NBR/NBR.h>
#include <niji_conv_header/app/NBR/NBR_anim_list.h>
#include <niji_conv_header/app/NBR/NBR_pane.h>
#include <niji_conv_header/message/msg_net_battle_reception.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

/**
 * @enum UpperViewID
 * 上画面ID
 */
enum UpperViewID {
  UPPER_VIEW_PLAYER_INFO_DEFAULT, //! プレイヤー情報（デフォルト）
  UPPER_VIEW_PLAYER_INFO_SIMPLE,  //! プレイヤー情報（ルール未定）
  UPPER_VIEW_VS,                  //! VS画面
  UPPER_VIEW_REGULATION,          //! レギュレーション詳細
};

/**
 * @enum LowerViewID
 * 下画面ID
 */
enum LowerViewID {
  LOWER_VIEW_RETURN_BUTTON,       //! 戻るボタンのみ
  LOWER_VIEW_2BUTTON,             //! 2ボタン
  LOWER_VIEW_TEAM_SELECT,         //! バトルチーム選択
  LOWER_VIEW_MULTI_SELECT,        //! マルチ・プレイヤー選択
};

/**
 * @enum LayoutWorkID
 * レイアウトID
 */
enum LayoutWorkID {
  LAYOUT_WORK_ID_UPPER,
  LAYOUT_WORK_ID_LOWER,
  LAYOUT_WORK_ID_MAX,
};

/**
 * @enum MessageFinishType
 * ストリームメッセージの終了タイプ
 */
enum MessageFinishType{
  MESSAGE_FINISH_INPUT, //! 入力待ち。▼がある場合これ
  MESSAGE_FINISH_NONE,  //! メッセージ表示し終わったら終わる。▼ない場合これ
};

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRVIEWDEFINE_H_INCLUDE
