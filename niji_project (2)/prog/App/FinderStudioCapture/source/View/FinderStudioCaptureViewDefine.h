#if !defined __FINDER_STUDIO_CAPTURE_VIEW_DEFINE_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_VIEW_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureViewDefine.h
 * @date    2016/11/18 12:12:30
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：View用定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

// ビューID
enum {
  VIEW_ID_UPPER,
  VIEW_ID_BG,
  VIEW_ID_PREPARE_SELECT,
  VIEW_ID_POKE_LIST,
  VIEW_ID_BG_LIST,
  VIEW_ID_DRESSUP_LIST,
  VIEW_ID_COMPOSITION,
  VIEW_ID_SELECT_BUTTON,
  VIEW_ID_PREPARE_COMMON,
  VIEW_ID_SHOOT,
  VIEW_ID_ZMOTION,
  VIEW_ID_CAMERA,
  VIEW_ID_PHOTO_LIST,
  VIEW_ID_SHOOT_COMMON,
  VIEW_ID_LAYOUT,
  VIEW_ID_NUM,
};

// 準備画面リザルト
enum {
  PREPARE_RES_CONTINUE,       //!< 継続
  PREPARE_RES_DISP_PREV,      //!< 次の画面へ
  PREPARE_RES_DISP_NEXT,      //!< 前の画面へ
  PREPARE_RES_CALL_CONFIRM,   //!< 撮影準備確認画面へ
  PREPARE_RES_CALL_SHOOT,     //!< 撮影画面へ
  PREPARE_RES_CALL_FINISH,    //!< 終了確認へ
  PREPARE_RES_FINISH_YES,     //!< 終了決定
  PREPARE_RES_FINISH_NO,      //!< 終了キャンセル
  PREPARE_RES_INFO_WND,       //!< インフォウィンドウ
  PREPARE_RES_NUM,
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_VIEW_DEFINE_H_INCLUDED__