#ifndef NIJI_PROJECT_APP_BOX_SEARCH_CURSOR_H_INCLUDED
#define NIJI_PROJECT_APP_BOX_SEARCH_CURSOR_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file BoxSearchCursor.h
 * @date 2015/07/28 16:49:46
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/app_tool_MenuCursor.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )
class DrawerBase;   //!< 描画共通処理
GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   カーソル
//------------------------------------------------------------------------------
class BoxSearchCursor
{
  GFL_FORBID_COPY_AND_ASSIGN( BoxSearchCursor );

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
   * @param   pBase   描画共通処理
   * @param   pAppRenderingManager    レンダリングマネージャ
   * @param   mode    技覚え  or 技忘れ
   */
  //------------------------------------------------------------------
  BoxSearchCursor( DrawerBase* pBase, app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxSearchCursor( void );

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

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの取得
   * @return  app::tool::MenuCursorクラスポインタ
   */
  //------------------------------------------------------------------
  app::tool::MenuCursor* GetMenuCursor( void );


private:
  DrawerBase*   m_pBase;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  app::tool::MenuCursor*  m_pMenuCursor;

  void*   m_pCursorBuffer;

  u8    m_Seq;

  u8    m_DeleteSeq;
};

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // __HEADER_H_INCLUDED__