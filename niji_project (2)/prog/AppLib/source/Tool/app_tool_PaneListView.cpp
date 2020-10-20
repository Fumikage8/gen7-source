//=============================================================================
/**
 * @file    app_tool_PaneListView.cpp
 * @brief   UIVew用ペインリスト
 * @author  Hiroyuki Nakamura
 * @date    2015.03.18
 */
//=============================================================================
#include <AppLib/include/Tool/app_tool_PaneListView.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   is_effect   エフェクトを有効にするか ※デフォルト=無効
 */
//-----------------------------------------------------------------------------
PaneListView::PaneListView( app::util::Heap * heap, bool is_effect )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_pPaneList( NULL )
  , m_pScrollBar( NULL )
  , m_listener( NULL )
  , m_isEffect( is_effect )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
PaneListView::~PaneListView()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.03.18
 */
//-----------------------------------------------------------------------------
void PaneListView::Update(void)
{
  app::ui::UIView::Update();
  UpdatePaneList();
  UpdateScrollBar();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.03.18
 */
//-----------------------------------------------------------------------------
void PaneListView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListener
 * @brief   リスナーを設定
 * @date    2015.04.02
 */
//-----------------------------------------------------------------------------
void PaneListView::SetListener( Listener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsButtonAnimation
 * @brief   ボタンのアニメ状態を取得する
 * @date    2015.03.18
 *
 * @retval  true  = アニメ中
 * @retval  false = アニメしていない
 */
//-----------------------------------------------------------------------------
bool PaneListView::IsButtonAnimation(void) const
{
  if( m_pPaneList == NULL )
  {
    return false;
  }

  if( m_pPaneList->GetUpdateResult() == PaneList::RET_NONE )
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetUpdateResult
 * @brief   ペインリストの更新結果を取得
 * @date    2015.03.18
 *
 * @return  更新結果
 */
//-----------------------------------------------------------------------------
PaneList::Result PaneListView::GetUpdateResult(void) const
{
  if( m_pPaneList == NULL )
  {
    GFL_ASSERT( 0 );
    return PaneList::RET_NONE;
  }
  return m_pPaneList->GetUpdateResult();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneList
 * @brief   ペインリストを生成
 * @date    2015.03.18
 *
 * @param   setup   セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void PaneListView::CreatePaneList( const PaneList::SETUP_PARAM * setup )
{
  if( m_pPaneList != NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_pPaneList = GFL_NEW(m_heap->GetSystemHeap()) app::tool::PaneList( setup, m_isEffect );
  SetInputListener( this );

}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePaneList
 * @brief   ペインリストを削除
 * @date    2015.03.18
 */
//-----------------------------------------------------------------------------
void PaneListView::DeletePaneList(void)
{
  GFL_SAFE_DELETE( m_pPaneList );
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetPaneList
 * @brief   ペインリストを再設定
 * @date    2015.03.18
 *
 * @param   setup   セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void PaneListView::ResetPaneList( const PaneList::SETUP_PARAM * setup )
{
  DeletePaneList();
  CreatePaneList( setup );

  if( setup->valMax <= setup->listPosMax )
  {
    ResetScrollBar( 0 );
  }
  else
  {
    ResetScrollBar( setup->valMax-setup->listPosMax );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePaneList
 * @brief   ペインリストの更新処理
 * @date    2015.03.18
 */
//-----------------------------------------------------------------------------
void PaneListView::UpdatePaneList(void)
{
  if( m_pPaneList == NULL )
  {
    return;
  }
  PaneList::Result result = m_pPaneList->Update();
  if( m_listener != NULL )
  {
    if( result == PaneList::RET_LIST_SELECT )
    {
      m_listener->PaneListItemSelectListener( m_pPaneList->GetItemPos() );
    }
    else if( result == PaneList::RET_LIST_CATCH )
    {
      m_listener->PaneListItemCatchListener( m_pPaneList->GetItemPos() );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateScrollBar
 * @brief   スクロールバーを生成
 * @date    2015.03.18
 *
 * @param   setup   セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void PaneListView::CreateScrollBar( const app::util::ScrollBar::SETUP_PARAM * setup )
{
  if( m_pScrollBar != NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_pScrollBar = GFL_NEW(m_heap->GetSystemHeap()) app::util::ScrollBar( setup );
  m_pScrollBar->SetRequest( app::util::ScrollBar::REQ_BAR_ON );
  ResetScrollBar( setup->max );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteScrollBar
 * @brief   スクロールバーを削除
 * @date    2015.03.18
 */
//-----------------------------------------------------------------------------
void PaneListView::DeleteScrollBar(void)
{
  GFL_SAFE_DELETE( m_pScrollBar );
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetScrollBar
 * @brief   スクロールバーを再設定
 * @date    2015.03.18
 *
 * @param   max   最大スクロール値
 */
//-----------------------------------------------------------------------------
void PaneListView::ResetScrollBar( u32 max )
{
  if( m_pScrollBar != NULL )
  {
    m_pScrollBar->ChangeParam( max );
    if( max == 0 )
    {
      m_pScrollBar->SetDrawEnable( false );
    }
    else
    {
      m_pScrollBar->SetDrawEnable( true );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateScrollBar
 * @brief   スクロールバーの更新処理
 * @date    2015.03.18
 */
//-----------------------------------------------------------------------------
void PaneListView::UpdateScrollBar(void)
{
  if( m_pScrollBar == NULL || m_pPaneList == NULL )
  {
    return;
  }
  if( m_pScrollBar->IsVisible() == false )
  {
    return;
  }

  u32	pos;
  f32	scroll;

  m_pPaneList->GetCursorData( &pos, &scroll );
  m_pScrollBar->Draw( scroll );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPaneList
 * @brief   ペインリスト取得
 * @date    2015.03.27
 *
 * @return  ペインリスト
 */
//-----------------------------------------------------------------------------
app::tool::PaneList * PaneListView::GetPaneList(void)
{
  return m_pPaneList;
}


  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タッチパネルイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
  *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult PaneListView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if(!isTouch){
    m_pPaneList->SetTpReleaseEnable(true);
  }
  m_pPaneList->SetTpEnable(isTouch);
  return ENABLE_ACCESS;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパネルのダブルクリックイベントの検知
 *
  * @param  pTouchPanel タッチパネル
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult PaneListView::OnDoubleClickEvent( gfl2::ui::TouchPanel * pTouchPanel )
{
  return ENABLE_ACCESS;
}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult PaneListView::OnLayoutPaneEvent( const u32 painId )
{
  return ENABLE_ACCESS;
}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペイン長押しイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult PaneListView::OnLayoutPaneLongPressEvent( const u32 painId )
{
  return ENABLE_ACCESS;
}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペイン選択アニメーションの開始を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  */
  //--------------------------------------------------------------------------------------------
void PaneListView::OnLayoutPaneStartingAnimeEvent( const u32 painId )
{
}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  * 
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  * 
  *                 TRIGER判定として使いたい場合は、ここを使わずに
  *                 ボタンマネージャーモードをTRIGGERにして
  *                 OnLayoutPaneEventを使うべき。
  */
  //--------------------------------------------------------------------------------------------
void PaneListView::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
}

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  キー入力の検知
   *         ※タッチパネルの入力が優先される
   *
   * @param  pButton  ボタン ( A,B,X,Y,L,R... )
   * @param  pKey     十字キー
   * @param  pStick   アナログスティック
   *
   * @return 同フレーム内での他入力イベントを許可するならtrue、
   *         他のイベントを排除するならfalseを返却すること。
   */
  //--------------------------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult PaneListView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  m_pPaneList->SetKeyEnable(true);
  m_pPaneList->SetButtonEnable(true);
  return ENABLE_ACCESS;
}



GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
