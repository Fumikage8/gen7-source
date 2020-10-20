#ifndef NIJI_PROJECT_FIELD_FINDER_WINDOW_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_WINDOW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    FinderWindow.h
 * @date    2015/12/07 11:59:13
 * @author  fukushima_yuya
 * @brief   ポケファインダーカメラアプリ：YESNOウィンドウ
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
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)
class DrawerBase;
struct APP_PARAM;
GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuWindow;
class MenuWindowListener;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
enum WndKind
{
  WND_KIND_UPLOAD,      //!< アップロード
  WND_KIND_CLOSE,       //!< 閉じる
  WND_KIND_NUM,
};


//------------------------------------------------------------------------------
// @brief   YesNoウィンドウリスナー
//------------------------------------------------------------------------------
class FinerConfirmWindowListenr
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   項目選択時のリスナー
   *
   * @param   kind    ウィンドウの種類
   * @param   id      選択項目ID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( WndKind  kind, u32 id ) { ; }
};

//------------------------------------------------------------------------------
// @brief   YesNoウィンドウ
//------------------------------------------------------------------------------
class FinderConfirmWindow
  : public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderConfirmWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  FinderConfirmWindow(
    DrawerBase* base,
    APP_PARAM*  appParam,
    app::util::AppRenderingManager* renderMan,
    FinerConfirmWindowListenr*  listener );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~FinderConfirmWindow( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  bool InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  void UpdateFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   *
   * @param   kind    ウィンドウの種類
   */
  //------------------------------------------------------------------
  void Open( WndKind kind );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   テキストの初期化
   */
  //------------------------------------------------------------------
  void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウの生成
   */
  //------------------------------------------------------------------
  void CreateMessageWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの生成
   */
  //------------------------------------------------------------------
  void CreateMenuWindow( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウを開く
   */
  //------------------------------------------------------------------
  void OpenMessageWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウを閉じる
   */
  //------------------------------------------------------------------
  void CloseMessageWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを開く
   */
  //------------------------------------------------------------------
  void OpenMenuWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを閉じる
   */
  //------------------------------------------------------------------
  void CloseMenuWindow( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージのセット
   */
  //------------------------------------------------------------------
  void SetMessage( void );
  void _SetMessage( u32 msgID );

private:
  //------------------------------------------------------------------
  /**
   * @brief   親VIEWの入力切り替え
   *
   * @param   enable    入力フラグ(true = 有効、false = 無効)
   */
  //------------------------------------------------------------------
  void SetEnableInput( bool enable );

public:   //!< メニューウィンドウリスナー
  //------------------------------------------------------------------
  /**
   * @brief   項目選択時のリスナー
   *
   * @param   id    メニュー項目ID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( u32 id );

private:
  DrawerBase*                       m_pBase;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  app::tool::MenuWindow*            m_pMenuWindow;

  FinerConfirmWindowListenr*        m_pListener;

  APP_PARAM*                        m_pAppParam;

private:
  u8        m_InitSeq;

  u8        m_EndSeq;

  u8        m_MainSeq;

  WndKind   m_WindowKind;

private:
  gfl2::lyt::LytTextBox*      m_pMsgLine_1;

  gfl2::lyt::LytTextBox*      m_pMsgLine_2;

  gfl2::lyt::LytPane*         m_pMsgCursor;
};

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_FIELD_FINDER_WINDOW_H_INCLUDED