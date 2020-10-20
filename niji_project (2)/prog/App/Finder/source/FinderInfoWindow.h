#ifndef NIJI_PROJECT_FIELD_FINDER_INFO_WINDOW_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_INFO_WINDOW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    FinderInfoWindow.h
 * @date    2015/10/23 15:01:12
 * @author  fukushima_yuya
 * @brief   インフォメーションウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)
class DrawerBase;
GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowSheetType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)

//------------------------------------------------------------------------------
// @brief   インフォメーションウィンドウクラス
//------------------------------------------------------------------------------
class FinderInfoWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderInfoWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderInfoWindow( DrawerBase* base, app::util::AppRenderingManager* pAppRenderingManager, app::tool::InfoWindowSheetTypeListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~FinderInfoWindow();

public:
  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //------------------------------------------------------------------
  bool IsReady( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   終了シーケンス
   * @return  "true" == 終了
   * @return  "false" == 終了処理中
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   * @param   version   ヴァージョン
   */
  //------------------------------------------------------------------
  void Open( u8 version );

private:
  enum {
    STR_TITLE,
    STR_DETAIL,
    STR_NUM,
  };

private:
  DrawerBase*                               m_pBase;

  app::tool::InfoWindowSheetType*           m_pInfoWindow;

  app::tool::InfoWindowSheetTypeListener*   m_pInfoWindowListener;

  gfl2::str::StrBuf*                        m_pStrBuf[STR_NUM];

  u8    m_EndSeq;

  gfl2::math::Vector3     m_InfoBtnPos;
};

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_FIELD_FINDER_INFO_WINDOW_H_INCLUDED