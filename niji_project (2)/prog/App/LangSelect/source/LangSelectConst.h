#ifndef NIJI_PROJECT_APP_LANG_SELECT_CONST_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_CONST_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectConst.h
 * @date 2015/07/23 15:34:38
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_LytSys.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(LangSelect)
class LangSelectDrawListener;
class DrawerBase;
GFL_NAMESPACE_END(LangSelect)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class ButtonManager;
class MenuCursor;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class G2DUtil;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN(GameSys)
class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)
class FadeManager;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_BEGIN( ui )
class TouchPanel;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(LangSelect)

//! @brief    ボタンID
enum BtnID
{
  BTN_ID_LANG_00,
  BTN_ID_LANG_01,
  BTN_ID_LANG_02,
  BTN_ID_LANG_03,
  BTN_ID_LANG_04,
  BTN_ID_LANG_05,
  BTN_ID_LANG_06,
  BTN_ID_LANG_07,
  BTN_ID_LANG_08,
  BTN_ID_CONFIRM_YES,
  BTN_ID_CONFIRM_NO,
  BTN_ID_CONFIRM_JPN_KANA,
  BTN_ID_CONFIRM_JPN_KANJI,
  BTN_ID_CONFIRM_JPN_NO,
  BTN_ID_NUM,
};

//! @brief    画面モード
enum DispMode {
  DISP_MODE_LANG_SELECT,    //!< 言語選択
  DISP_MODE_CONFIRM,        //!< 言語決定確認
  DISP_MODE_CONFIRM_JPN,    //!< 日本語決定確認（かな・漢字・キャンセル選択画面）
  DISP_MODE_SAVE_DATA,      //!< セーブデータフロー

  DISP_MODE_END_SETUP,      //!< 設定終了
  DISP_MODE_TELL,           //!< 博士から通信

  DISP_MODE_NUM,
};


//! 言語数
#define LANG_ID_MAX     (9)

//! 言語データ
struct LangData
{
  u8  lang;
  f32 frame;
};


GFL_NAMESPACE_END(LangSelect)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_LANG_SELECT_CONST_H_INCLUDED