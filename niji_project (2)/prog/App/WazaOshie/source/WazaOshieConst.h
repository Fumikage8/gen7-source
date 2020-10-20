#ifndef NIJI_PROJECT_APP_WAZAOSHIE_CONST_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_CONST_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieConst.h
 * @date 2015/07/03 16:49:35
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/WazaOshie/include/WazaOshieAppParam.h>

// gflib2
#include <heap/include/gfl2_Heap.h>
#include <Layout/include/gfl2_LytSys.h>
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
class ButtonManager;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class G2DUtil;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_BEGIN( ui )
class TouchPanel;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( gfl2 )

class WordSetLoader;


//------------------------------------------------------------------------------
// @brief   定数宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

// @brief   RESID
enum {
  RES_ID_STATUS,
  RES_ID_EXPLAIN,
  RES_ID_NUM,
};

// @brief   ボタンID
enum BtnID
{
  BTN_ID_BACK = 0,
  BTN_ID_WAZA_0,
  BTN_ID_WAZA_1,
  BTN_ID_WAZA_2,
  BTN_ID_WAZA_3,
  BTN_ID_WAZA_4,
  BTN_ID_INFO_0,
  BTN_ID_INFO_1,
  BTN_ID_DSC_DECIDE,
  BTN_ID_DSC_CANCEL,
  BTN_ID_DSC_ARROW_L,
  BTN_ID_DSC_ARROW_R,
  BTN_ID_NUM,

  BTN_ID_SKL_NUM = BTN_ID_INFO_1 + 1,
  BTN_ID_DSC_NUM = BTN_ID_NUM - BTN_ID_SKL_NUM,
};

// @brief   画面ID
enum
{
  DISP_MODE_STATUS,
  DISP_MODE_EXPLAIN,
  DISP_MODE_NUM,
};


GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_CONST_H_INCLUDED