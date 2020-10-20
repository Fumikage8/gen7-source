//======================================================================
/**
 * @file    FinderStudioViewUtil.cpp
 * @date    2016/11/18 11:04:03
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewユーティリティ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>

#include <AppLib/include/Ui/UIView.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ViewUtil::ViewUtil( app::ui::UIView* view, u32 lytID )
  : m_pUIView( view )
  , m_pLytWk( GetG2D()->GetLayoutWork( lytID ) )
  , m_ResID( GetG2D()->GetLayoutResourceID( 0 ) )
  , m_LayoutID( lytID )
  , m_bInputBtnAnim( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    UIView設定関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアニメ中にキー入力を有効にするかの現在のフラグをセット
 */
void ViewUtil::SetPrevInputExclusiveKeyByBtnAnim( void )
{
  m_bInputBtnAnim = m_pUIView->GetInputExclusiveKeyByButtonAnimation();
}

/**
 * @brief   ボタンアニメ中にキー入力を有効にするかのフラグを設定
 *
 * @param   flag    "true"なら有効、"false"なら無効
 */
void ViewUtil::SetInputExclusiveKeyByBtnAnim( bool flag )
{
  m_pUIView->SetInputExclusiveKeyByButtonAnimation( flag );
}

/**
 * @brief   ボタンアニメ中にキー入力を有効にするかのフラグを元の状態に戻す
 */
void ViewUtil::ResetInputExclusiveKeyByBtnAnim( void )
{
  m_pUIView->SetInputExclusiveKeyByButtonAnimation( m_bInputBtnAnim );
}

/**
 * @brief   ボタンアニメ中にキー入力を有効にするかの元の状態のフラグを取得
 */
bool ViewUtil::GetInputExclusiveKeyByBtnAnim( void ) const
{
  return m_bInputBtnAnim;
}

/**
 * @brief   モーダルウィンドウの設定
 */
void ViewUtil::SetModal( bool modal )
{
  m_pUIView->SetModal( modal );
}

/**
 * @brief   モーダルウィンドウ状態のフラグを取得
 *
 * @retval  "true"ならモーダルウィンドウ
 * @retval  "false"ならそれ以外
 */
bool ViewUtil::GetModal( void )
{
  return m_pUIView->IsModal();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペイン取得関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインの取得
 */
gfl2::lyt::LytPane* ViewUtil::GetPane( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetPane( index );
}

/**
 * @brief   テキストボックスペインの取得
 */
gfl2::lyt::LytTextBox* ViewUtil::GetTextBox( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetTextBoxPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetTextBoxPane( index );
}

/**
 * @brief   境界ペインの取得
 */
gfl2::lyt::LytBounding* ViewUtil::GetBounding( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetBoundingPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetBoundingPane( index );
}

/**
 * @brief   ピクチャペインの取得
 */
gfl2::lyt::LytPicture* ViewUtil::GetPicture( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetPicturePane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetPicturePane( index );
}

/**
 * @brief   部品ペインの取得
 */
gfl2::lyt::LytParts* ViewUtil::GetParts( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetPartsPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetPartsPane( index );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペイン操作関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインの表示切替
 */
void ViewUtil::SetVisiblePane( gfl2::lyt::LytPane* pane, bool visible )
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2D()->SetPaneVisible( pane, visible );
}

/**
 * @brief   ペインの表示フラグの取得
 */
bool ViewUtil::GetVisiblePane( gfl2::lyt::LytPane* pane ) const
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  return GetG2D()->GetPaneVisible( pane );
}

/**
 * @brief   ペインの位置を設定
 */
void ViewUtil::SetPanePos( gfl2::lyt::LytPane* pane, gfl2::math::VEC3 pos )
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  pane->SetTranslate( pos );
}

/**
 * @brief   ペインの位置を取得
 */
gfl2::math::VEC3 ViewUtil::GetPanePos( gfl2::lyt::LytPane* pane ) const
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return gfl2::math::VEC3();
  }

  return pane->GetTranslate();
}

/**
 * @brief   ペインのサイズを設定
 */
void ViewUtil::SetPaneSize( gfl2::lyt::LytPane* pane, nw::lyt::Size size )
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  pane->SetSize( size );
}

/**
 * @brief   ペインのサイズを取得
 */
nw::lyt::Size ViewUtil::GetPaneSize( gfl2::lyt::LytPane* pane )
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return nw::lyt::Size( 0, 0 );
  }

  return pane->GetSize();
}


/**
 * @brief   ペインの回転値を設定
 */
void ViewUtil::SetPaneRotate( gfl2::lyt::LytPane* pane, gfl2::math::VEC3 rotate )
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  pane->SetRotate( rotate );
}

/**
 * @brief   ペインの回転値を取得
 */
gfl2::math::VEC3 ViewUtil::GetPaneRotate( gfl2::lyt::LytPane* pane ) const
{
  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
    return gfl2::math::VEC3();
  }

  return pane->GetRotate();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペインタッチ判定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインのタッチ判定
 *
 * @param   tp      タッチパネル
 * @param   pane    対象ペイン
 *
 * @retval  "true"ならタッチ中
 * @retval  "flase"ならそれ以外
 */
bool ViewUtil::IsTouchPane( gfl2::ui::TouchPanel* tp, gfl2::lyt::LytPane* pane )
{
  return GetG2D()->IsTouchPane( tp, m_LayoutID, pane );
}

/**
 * @brief   ペインのタッチ判定：トリガー
 *
 * @param   pane    対象ペイン
 *
 * @retval  "true"ならタッチ
 * @retval  "flase"ならそれ以外
 */
bool ViewUtil::IsTouchTriggerPane( gfl2::lyt::LytPane* pane )
{
  return GetG2D()->IsPaneTouchTrigger( m_LayoutID, pane );
}

/**
 * @brief   ペインのタッチ判定：ホールド
 *
 * @param   pane    対象ペイン
 *
 * @retval  "true"ならホールド
 * @retval  "flase"ならそれ以外
 */
bool ViewUtil::IsTouchHoldPane( gfl2::lyt::LytPane* pane )
{
  return GetG2D()->IsPaneTouchHold( m_LayoutID, pane );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペインアニメ関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   アニメの再生
 */
void ViewUtil::StartAnim( u32 anmID, bool auto_anim, bool auto_unbind )
{
  GetG2D()->StartAnime( m_LayoutID, anmID, auto_anim, auto_unbind );
}

/**
 * @brief   アニメの停止
 */
void ViewUtil::StopAnim( u32 anmID, bool clear )
{
  GetG2D()->StopAnime( m_LayoutID, anmID, clear );
}

/**
 * @brief   アニメの終了チェック
 *
 * @retval  "true"なら終了
 * @retval  "false"ならそれ以外
 */
bool ViewUtil::IsAnimEnd( u32 anmID )
{
  return GetG2D()->IsAnimeEnd( m_LayoutID, anmID );
}

/**
 * @brief   アニメのフレームを指定
 */
void ViewUtil::SetAnimFrame( u32 anmID, f32 frame )
{
  GetG2D()->SetAnimeFrame( m_LayoutID, anmID, frame );
}

/**
 * @brief   アニメの現在フレームを取得
 */
f32 ViewUtil::GetAnimFrame( u32 anmID )
{
  return GetG2D()->GetAnimeFrame( m_LayoutID, anmID );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンをロック
 */
void ViewUtil::LockButton( void )
{
  GetButtonManager()->LockButtonControl();
}

/**
 * @brief   ボタンをアンロック
 */
void ViewUtil::UnlockButton( void )
{
  GetButtonManager()->UnlockButtonControl();
}


/**
 * @brief   ボタンアクション中か？
 *
 * @retval  "true"ならボタンアクション中
 * @retval  "false"ならそれ以外
 */
bool ViewUtil::IsButtonAction( void )
{
  return GetButtonManager()->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    取得関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Viewクラスの取得
 */
app::ui::UIView* ViewUtil::GetView( void ) const
{
  return m_pUIView;
}

/**
 * @brief   G2DUtilクラスの取得
 */
app::util::G2DUtil* ViewUtil::GetG2D( void ) const
{
  return m_pUIView->GetG2DUtil();
}

/**
 * @brief   ButtonManagerクラスの取得
 */
app::tool::ButtonManager* ViewUtil::GetButtonManager( void ) const
{
  return m_pUIView->GetButtonManager();
}

/**
 * @brief   LytWkクラスの取得
 */
gfl2::lyt::LytWk* ViewUtil::GetLytWk( void ) const
{
  return m_pLytWk;
}

/**
 * @brief   LytResクラスの取得
 */
gfl2::lyt::LytRes* ViewUtil::GetLytRes( void ) const
{
  return m_pUIView->GetLayoutSystem()->GetResource( &m_ResID );
}

/**
 * @brief   LytMultiResIDの取得
 */
gfl2::lyt::LytMultiResID ViewUtil::GetMultiResID( void ) const
{
  return m_ResID;
}

/**
 * @brief   LytResTextureInfoクラスの取得
 *
 * @param   dataID    データID
 */
const gfl2::lyt::LytResTextureInfo* ViewUtil::GetTextureInfo( u32 dataID )
{
  return GetLytRes()->GetTexture( dataID, 0 );
}

/**
 * @brief   レイアウトIDの取得
 */
u32 ViewUtil::GetLytID( void ) const
{
  return m_LayoutID;
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
