#ifndef NIJI_PROJECT_APP_TITLE_MENU_YES_NO_WINDOW_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_YES_NO_WINDOW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    TitleMenuYesNoWindow.h
 * @date    2015/09/07 18:13:04
 * @author  fukushima_yuya
 * @brief   タイトルメニューYesNoウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuWindow;
class MenuWindowListener;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

//------------------------------------------------------------------------------
// @brief   YesNoウィンドウ
//------------------------------------------------------------------------------
class YesNoWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( YesNoWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  YesNoWindow( DrawerBase* pBase, MenuCursor* pCursor, app::util::AppRenderingManager* pRenderingManager, app::tool::MenuWindowListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~YesNoWindow( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( app::tool::MenuWindowListener* pListener );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か
   * @return  "true" == 完了
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄完了か
   * @return  "true" == 完了
   */
  //------------------------------------------------------------------
  bool IsDelete( void );

  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  void OpenWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void CloseWindow( void );

private:
  DrawerBase*   m_pBase;

  MenuCursor*   m_pCursor;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  app::tool::MenuWindow*    m_pMenuWindow;

  gfl2::lyt::LytTextBox*    m_pMessage;

  u8  m_Seq;

  bool    m_isOpenWindow;
};

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_TITLE_MENU_YES_NO_WINDOW_H_INCLUDED