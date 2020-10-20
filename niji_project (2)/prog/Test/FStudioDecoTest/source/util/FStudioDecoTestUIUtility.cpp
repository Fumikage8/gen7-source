#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestUIUtility.cpp
 * @date    2016/10/27 18:40:01
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestUIUtility.h"

#include <AppLib/include/Ui/UIView.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

//------------------------------------------------------------------------------
// @brief   コンストラクタ
//------------------------------------------------------------------------------
UIUtility::UIUtility( app::ui::UIView* view, u32 lytID )
  : m_pUIView(view)
  , m_pLytWk(GetG2D()->GetLayoutWork(m_LayoutID))
  , m_ResID(GetG2D()->GetLayoutResourceID(0))
  , m_LayoutID(lytID)
  , m_bInputBtnAnim(false)
{
  ;
}



//------------------------------------------------------------------------------
// @brief   UIView設定関数群
//------------------------------------------------------------------------------
void UIUtility::SetPrevInputExclusiveKeyByBtnAnim( void )
{
  m_bInputBtnAnim = m_pUIView->GetInputExclusiveKeyByButtonAnimation();
}

void UIUtility::SetInputExclusiveKeyByBtnAnim( bool flag )
{
  m_pUIView->SetInputExclusiveKeyByButtonAnimation( flag );
}

void UIUtility::ResetInputExclusiveKeyByBtnAnim( void )
{
  m_pUIView->SetInputExclusiveKeyByButtonAnimation( m_bInputBtnAnim );
}

bool UIUtility::GetInputExclusiveKeyByBtnAnim( void ) const
{
  return m_bInputBtnAnim;
}


void UIUtility::SetModal( bool modal )
{
  m_pUIView->SetModal( modal );
}

bool UIUtility::GetModal( void )
{
  return m_pUIView->IsModal();
}



//------------------------------------------------------------------------------
// @brief   ペイン取得関数群
//------------------------------------------------------------------------------
gfl2::lyt::LytPane* UIUtility::GetPane( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetPane( index );
}

gfl2::lyt::LytTextBox* UIUtility::GetTextBox( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetTextBoxPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetTextBoxPane( index );
}

gfl2::lyt::LytBounding* UIUtility::GetBounding( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetBoundingPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetBoundingPane( index );
}

gfl2::lyt::LytPicture* UIUtility::GetPicture( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetPicturePane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetPicturePane( index );
}

gfl2::lyt::LytParts* UIUtility::GetParts( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
{
  if( pParts )
  {
    return m_pLytWk->GetPartsPane( pParts, index, &m_ResID );
  }

  return m_pLytWk->GetPartsPane( index );
}



//------------------------------------------------------------------------------
// @brief   ペイン操作関数群
//------------------------------------------------------------------------------
void UIUtility::SetVisiblePane( gfl2::lyt::LytPane* pPane, bool visible )
{
  if( pPane == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2D()->SetPaneVisible( pPane, visible );
}

bool UIUtility::GetVisiblePane( gfl2::lyt::LytPane* pPane ) const
{
  if( pPane == NULL )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  return GetG2D()->GetPaneVisible( pPane );
}


void UIUtility::SetPanePos( gfl2::lyt::LytPane* pPane, gfl2::math::VEC3 pos )
{
  if( pPane == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  pPane->SetTranslate( pos );
}

gfl2::math::VEC3 UIUtility::GetPanePos( gfl2::lyt::LytPane* pPane ) const
{
  if( pPane == NULL )
  {
    GFL_ASSERT( 0 );
    return gfl2::math::VEC3();
  }

  return pPane->GetTranslate();
}



//------------------------------------------------------------------------------
// @brief   取得関数群
//------------------------------------------------------------------------------
app::ui::UIView* UIUtility::GetView( void ) const
{
  return m_pUIView;
}

app::util::G2DUtil* UIUtility::GetG2D( void ) const
{
  return m_pUIView->GetG2DUtil();
}

app::tool::ButtonManager* UIUtility::GetButtonManager( void ) const
{
  return m_pUIView->GetButtonManager();
}

gfl2::lyt::LytWk* UIUtility::GetLytWk( void ) const
{
  return m_pLytWk;
}

gfl2::lyt::LytRes* UIUtility::GetLytRes( void ) const
{
  return m_pUIView->GetLayoutSystem()->GetResource( &m_ResID );
}

gfl2::lyt::LytMultiResID UIUtility::GetMultiResID( void ) const
{
  return m_ResID;
}

u32 UIUtility::GetLytID( void ) const
{
  return m_LayoutID;
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
