#ifndef NIJI_PROJECT_APP_LANG_SELECT_CURSOR_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_CURSOR_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectCursor.h
 * @date 2015/07/24 10:55:27
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/source/LangSelectConst.h"

#include <AppLib/include/Tool/app_tool_MenuCursor.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )
class DrawerBase;   //!< 描画共通処理
GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   カーソル
//------------------------------------------------------------------------------
class LangSelectCursor
{
  GFL_FORBID_COPY_AND_ASSIGN( LangSelectCursor );

public:
  enum Sequence {
    SEQ_LOAD,
    SEQ_MAIN,
    SEQ_DELETE,
    SEQ_END,
    SEQ_NUM
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  LangSelectCursor( DrawerBase* pBase, app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~LangSelectCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   * @return  true == 完了
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了可能か？
   * @return  true == 終了、false == 否
   */
  //------------------------------------------------------------------
  bool IsEnd( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの開始
   */
  //------------------------------------------------------------------
  void StartMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの終了
   */
  //------------------------------------------------------------------
  void EndMenuCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   画面モードのセット
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  void SetDispMode( DispMode dispMode );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の設定
   * @param   index   カーソル位置
   * @param   playSE  SEの再生フラグ
   */
  //------------------------------------------------------------------
  void SetCursorPos( u32 index, bool playSE = true );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の取得
   */
  //------------------------------------------------------------------
  u32 GetCursorPos( void );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力
   */
  //------------------------------------------------------------------
  void InputKey( gfl2::ui::Button* pButton );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   * @param   visible   表示フラグ
   * @param   id        カーソルID(デフォルト = CURSOR_LEFT)
   *
   * @li  id = app::tool::MenuCursor::CURSOR_LEFT  : 左カーソル
   * @li  id = app::tool::MenuCursor::CURSOR_RIGHT : 右カーソル
   * @li  id = app::tool::MenuCursor::CURSOR_ALL   : 全体
   */
  //------------------------------------------------------------------
  void SetVisible( bool visible, u32 id = app::tool::MenuCursor::CURSOR_LEFT );


private:
  DrawerBase*   m_pBase;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  app::tool::MenuCursor*  m_pMenuCursor;

  void*   m_pCursorBuffer;

  u8    m_Seq;
  u8    m_DeleteSeq;
  u32   m_Index;
  u32   m_LangIndex;

  DispMode  m_DispMode;

  gfl2::lyt::LytPane*   m_pCursorPos[BTN_ID_NUM];
};

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_LANG_SELECT_CURSOR_H_INCLUDED