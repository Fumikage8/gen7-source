#ifndef NIJI_PROJECT_FIELD_FINDER_APP_PARAM_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_APP_PARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    FinderAppParam.h
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   外部設定パラメータ
 */
//==============================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//-----------------------------------------------------------------------------
/**
 *             定数宣言
 */
//-----------------------------------------------------------------------------
//! @brief    終了モード
enum EndMode {
  END_MODE_NONE,        //!< 継続中
  END_MODE_CANCEL,      //!< キャンセル
  END_MODE_FIN_SHOOT,   //!< 写真選択へ遷移
  END_MODE_NUM,
};

//! @brief    ピントの種類
enum {
  PINT_TYPE_00,       //!< ピント「近く」
  PINT_TYPE_01,       //!< ピント　 ↑
  PINT_TYPE_02,       //!< ピント　 ｜
  PINT_TYPE_03,       //!< ピント　 ｜
  PINT_TYPE_04,       //!< ピント　 ｜
  PINT_TYPE_05,       //!< ピント　 ｜
  PINT_TYPE_06,       //!< ピント　 ↓
  PINT_TYPE_07,       //!< ピント「遠く」
  PINT_TYPE_NUM,
};

//-----------------------------------------------------------------------------
/**
 *             構造体
 */
//-----------------------------------------------------------------------------
//! @brief  外部設定パラメータ
struct APP_PARAM
{
  // [in]
  f32   zoom_min;         //!< ズーム最小値
  f32   zoom_max;         //!< ズーム最大値
  f32   zoom_val;         //!< 現在のズーム値

  u8    shutter_count;    //!< 残りシャッター回数

  u16   tutorial_flg;     //!< チュートリアル起動フラグ

  u32   photo_score;      //!< 写真のスコア

  bool  fade_skip;        //!< ロトムフェードスキップフラグ
  bool  trial_mode;       //!< 試練モードフラグ

#if PM_DEBUG
  bool  debug_shutter;    //!< シャッターを閉じずに撮影するデバッグ
#endif

  // [in / out]
  u8    pint_type;        //!< ピントの値

  bool  shutter_open;     //!< シャッターを開くフラグ
  bool  gyro_flg;         //!< ジャイロフラグ

  // [out]
  EndMode endMode;        //!< 終了モード
  bool  shutter_close;    //!< シャッターを閉じた事を通知するフラグ
  bool  poke_call;        //!< コールボタンフラグ(true = 押した、false = 押されていない)
  bool  ext_input_disable;//!< 外部入力禁止フラグ(true = 入力禁止、false = 入力有効)

  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  APP_PARAM( void )
    : zoom_min(0.0f)
    , zoom_max(1.0f)
    , zoom_val(0.0f)
    , shutter_count(6)
    , tutorial_flg(0)
    , photo_score(0)
    , fade_skip(false)
    , trial_mode(false)
#if PM_DEBUG
    , debug_shutter( false )
#endif
    , pint_type(0)
    , shutter_open(false)
    , gyro_flg(true)
    , endMode(END_MODE_NONE)
    , shutter_close(false)
    , poke_call(false)
    , ext_input_disable(true)
  {
    ;
  }
};

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

#endif  //#ifndef NIJI_PROJECT_FIELD_FINDER_EXTERNAL_PARAM_H_INCLUDED
