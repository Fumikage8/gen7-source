#if !defined __FINDER_STUDIO_DECORATION_VIEW_DEFINE_H_INCLUDED__
#define __FINDER_STUDIO_DECORATION_VIEW_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationViewDefine.h
 * @date    2016/11/22 17:16:50
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：View用定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数定義 begin ]---

/// ビューID
enum {
  VIEW_ID_PARENT,
  VIEW_ID_UPP_MAIN,
  VIEW_ID_LOW_MAIN,
  VIEW_ID_LOW_STAMP_LIST,
  VIEW_ID_LOW_FRAME_LIST,
  VIEW_ID_NUM,
};

/// リザルト
enum {
  VIEW_RES_CONTINUE,
  VIEW_RES_NUM,
};

/// メッセージウィンドウID
enum MsgWndID
{
  MSG_WND_ID_CAUTION_PRE,     //!< 注意喚起：アプリ起動時
  MSG_WND_ID_STAMP_OVER,      //!< 配置スタンプ数のオーバー
  MSG_WND_ID_FIN_MENU_SAVE,   //!< 終了メニュー：保存方法選択
  MSG_WND_ID_FIN_MENU_QUIT,   //!< 終了メニュー：終了確認
  MSG_WND_ID_OVERWRITE_CONF,  //!< 上書き保存確認
  MSG_WND_ID_CAUTION,         //!< コピー保存時の注意喚起
  MSG_WND_ID_DELETE_CONF,     //!< 終了確認：データ破棄
  MSG_WND_ID_NUM,

  MSG_WND_ID_NULL,
};

/// 言語ID
enum {
  LANG_IDX_JPN,
  LANG_IDX_USA,
  LANG_IDX_FRA,
  LANG_IDX_ITA,
  LANG_IDX_DEU,
  LANG_IDX_SPA,
  LANG_IDX_KOR,
  LANG_IDX_CHN,
  LANG_IDX_TWN,
  LANG_IDX_NUM,
};


/// ---[ 定数定義 end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_VIEW_DEFINE_H_INCLUDED__
