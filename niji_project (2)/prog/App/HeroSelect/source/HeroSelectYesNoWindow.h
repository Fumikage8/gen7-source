#ifndef NIJI_PROJECT_APP_EVENT_YES_NO_WINDOW_H_INCLUDED
#define NIJI_PROJECT_APP_EVENT_YES_NO_WINDOW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectYesNoWindow.h
 * @date 2015/08/27 10:53:19
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )
class DrawerBase;
class MenuCursor;
class HeroSelectCursor;
GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuWindow;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   YesNoウィンドウ
//------------------------------------------------------------------------------
class YesNoWindow
  : public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( YesNoWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  YesNoWindow( DrawerBase* pBase, MenuCursor* pCursor, app::util::AppRenderingManager* pRenderingManager );

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
  void Init( void );

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

  //------------------------------------------------------------------
  /**
   * @brief   結果の取得
   */
  //------------------------------------------------------------------
  u8 GetResult( void ) const;

private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの選択
   * @param   選択された項目ID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( u32 id );

private:
  DrawerBase*                       m_pBase;

  MenuCursor*                       m_pCursor;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  app::tool::MenuWindow*            m_pMenuWindow;

  gfl2::lyt::LytTextBox*            m_pMessage;

  u8      m_Seq;

  u8      m_Result;

  bool    m_isOpenWindow;
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_EVENT_YES_NO_WINDOW_H_INCLUDED