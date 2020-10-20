//======================================================================
/**
 * @file    FinderStudioCapturePaneList.cpp
 * @date    2016/11/29 19:35:47
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：ペインリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/FinderStudioCapturePaneList.h>

#include <AppLib/include/UI/UIView.h>
#include <Sound/include/Sound.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
PaneList::PaneList( void )
  : m_pPaneListView( NULL )
  , m_bCreate( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   PaneListViewの生成
 */
void PaneList::CreatePaneListView( app::util::Heap* heap, app::tool::PaneListView::Listener* listener, app::ui::UIView* view )
{
  m_pPaneListView = GFL_NEW( heap->GetSystemHeap() ) app::tool::PaneListView( heap, true );
  m_pPaneListView->SetListener( listener );

  // SubViewへ追加
  view->AddSubView( m_pPaneListView );
}

/**
 * @brief   PaneListViewの破棄
 */
void PaneList::DeletePaneListView( void )
{
  if( m_bCreate )
  {
    DeletePaneList();
  }

  if( m_pPaneListView->GetSuperView() )
  {
    m_pPaneListView->RemoveFromSuperView();
  }

  GFL_SAFE_DELETE( m_pPaneListView );
}

/**
 * @brief   PaneListの生成
 */
void PaneList::CreatePaneList( CreateParam& param )
{
  // ペインリストの生成
  m_pPaneListView->CreatePaneList( &param.paneListParam );
  // スクロールバーの生成
  m_pPaneListView->CreateScrollBar( &param.scrollBarParam );

  // タッチエリアの設定
  _SetTouchArea( &param.paneListParam );

  m_bCreate = true;
}

/**
 * @brief   PaneListの破棄
 */
void PaneList::DeletePaneList( void )
{
  // スクロールバーの破棄
  m_pPaneListView->DeleteScrollBar();
  // ペインリストの破棄
  m_pPaneListView->DeletePaneList();

  m_bCreate = false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Config設定
 */
void PaneList::SetDefaultConfig( void )
{
  GetPaneList()->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_KEY, true, false, true, false );
}

/**
 * @brief   デフォルトSE設定
 */
void PaneList::SetDefaultSE( void )
{
  SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );
}

/**
 * @brief   SE設定
 */
void PaneList::SetSE( u32 selectSE, u32 decideSE, u32 cancelSE )
{
  GetPaneList()->SetSE( selectSE, decideSE, cancelSE );
}

/**
 * @brief   タッチエリアの設定
 */
void PaneList::_SetTouchArea( const app::tool::PaneList::SETUP_PARAM* prm )
{
  gfl2::math::VEC3 pos = prm->paneList[0].bound->GetTranslate();
  u32 posMax = prm->listPosMax;
  if( prm->valMax < posMax )
  {
    posMax = prm->valMax;
  }

  f32 lx = 0.0f - ( prm->defSX / 2.0f );
  f32 rx = lx + prm->defSX;
  f32 uy = prm->defPY + pos.y + ( prm->defSY / 2.0f );
  f32 dy = uy - ( prm->defSY * posMax );

  GetPaneList()->MakeTouchArea( lx, rx, uy, dy );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    入力の有効設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインリスト操作を有効
 */
void PaneList::EnablePaneList( void )
{
  m_pPaneListView->SetInputEnabled( true );
}

/**
 * @brief   ペインリスト操作を無効
 */
void PaneList::DisablePaneList( void )
{
  m_pPaneListView->SetInputEnabled( false );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
