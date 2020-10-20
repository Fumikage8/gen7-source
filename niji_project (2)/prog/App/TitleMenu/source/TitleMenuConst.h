#ifndef NIJI_PROJECT_APP_TITLE_MENU_CONST_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_CONST_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    TitleMenuConst.h
 * @date    2015/07/21 19:49:35
 * @author  fukushima_yuya
 * @brief   タイトルメニュー
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
GFL_NAMESPACE_BEGIN(TitleMenu)
class TitleMenuDrawListener;
class DrawerBase;
GFL_NAMESPACE_END(TitleMenu)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuCursor;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(GameSys)
class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)
class FadeManager;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(gfl2)

class WordSetLoader;


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TitleMenu)

//! ARCID
enum {
  ARC_TITLE_MENU,
  ARC_FIELDWINDOW,
  ARC_MAX,
};

//! @brief    画面モード
enum DispMode
{
  DISP_MODE_TITLE_MENU,     //!< タイトルメニュー
  DISP_MODE_INIT_SAVEDATA,  //!< セーブ削除
  DISP_MODE_NUM,
};

//! @brief    YESNOの結果
enum
{
  YESNO_RES_YES,
  YESNO_RES_NO,
  YESNO_RES_NUM,
  YESNO_RES_NONE,
};

//! @brief    ボタンID
enum BtnID
{
  BTN_ID_CONTINUE,    //!< つづきから
  BTN_ID_PRESENT,     //!< ふしぎなおくりもの
  BTN_ID_LIVE_CUP,    //!< ライブ大会
  BTN_ID_NUM,
};

GFL_NAMESPACE_END(TitleMenu)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_TITLE_MENU_CONST_H_INCLUDED