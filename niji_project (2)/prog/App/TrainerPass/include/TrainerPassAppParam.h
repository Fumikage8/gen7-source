//=============================================================================
/**
 * @file    TrainerPassAppParam.h
 * @brief   トレーナーパス画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================
#if !defined( TRAINER_PASS_APP_PARAM_H_INCLUDED )
#define TRAINER_PASS_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)

//! 呼び出しモード
enum CallMode
{
  CALL_MODE_MENU = 0,   //!< フィールドメニュー
  CALL_MODE_STAMP,      //!< スタンプデモ
};

//! 終了モード
enum EndMode
{
  END_MODE_NORMAL = 0,  //!< 通常終了
  END_MODE_PHOTO,       //!< 証明写真へ
};


//! 外部設定パラメータ
struct APP_PARAM
{
  // [in]
  CallMode call_mode;   //!< 呼び出しモード
  u32 stamp_no;         //!< スタンプ番号（デモモードのみ） 0オリジン

  // [out]
  EndMode end_mode;     //!< 終了モード
};

GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)

#endif  // TRAINER_PASS_APP_PARAM_H_INCLUDED
