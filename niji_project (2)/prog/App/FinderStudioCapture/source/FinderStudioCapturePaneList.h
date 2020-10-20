#if !defined __FINDER_STUDIO_CAPTURE_PANE_LIST_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_PANE_LIST_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCapturePaneList.h
 * @date    2016/11/29 19:27:57
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：ペインリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/Tool/app_tool_PaneListView.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewUtil;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペインリスト
///
//////////////////////////////////////////////////////////////////////////
class PaneList
{
  GFL_FORBID_COPY_AND_ASSIGN( PaneList ); //コピーコンストラクタ＋代入禁止

public:
  /// 生成パラメータ
  struct CreateParam
  {
    app::tool::PaneList::SETUP_PARAM  paneListParam;
    app::util::ScrollBar::SETUP_PARAM scrollBarParam;
  };
public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  PaneList( void );
  virtual ~PaneList( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   生成・破棄
  //-------------------------------------------------------
  void CreatePaneListView( app::util::Heap* heap, app::tool::PaneListView::Listener* listener, app::ui::UIView* view );
  void DeletePaneListView( void );

  void CreatePaneList( CreateParam& param );
  void DeletePaneList( void );

  //-------------------------------------------------------
  // @brief   設定
  //-------------------------------------------------------
  void SetDefaultConfig( void );
  void SetDefaultSE( void );
  void SetSE( u32 selectSE, u32 decideSE, u32 cancelSE );

  //-------------------------------------------------------
  // @brief   入力の有効設定
  //-------------------------------------------------------
  void EnablePaneList( void );
  void DisablePaneList( void );

  //-------------------------------------------------------
  // @brief   取得
  //-------------------------------------------------------
  app::tool::PaneList* GetPaneList( void ) const { return m_pPaneListView->GetPaneList(); }

private:
  //-------------------------------------------------------
  // @brief   設定：非公開
  //-------------------------------------------------------
  void _SetTouchArea( const app::tool::PaneList::SETUP_PARAM* prm );

private:
  app::tool::PaneListView*    m_pPaneListView;

  bool  m_bCreate;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_PANE_LIST_H_INCLUDED__
