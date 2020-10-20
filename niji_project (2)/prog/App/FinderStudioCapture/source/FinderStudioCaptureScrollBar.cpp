//======================================================================
/**
 * @file    FinderStudioCaptureScrollBar.cpp
 * @date    2017/02/14 12:45:52
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：ペインリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureScrollBar.h"


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
 SlideScrollBar::SlideScrollBar( gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytPane * parts, gfl2::lyt::LytPane * tab, gfl2::lyt::LytPane * min, gfl2::lyt::LytPane * max, ScrollDirection dir )
  : m_pLytwk( lytwk )
  , m_pBasePane( parts )
  , m_pTabPane( tab )
  , m_pMinPane( min )
  , m_pMaxPane( max )
  , m_ScrollDir( dir )
  , m_TabState( TAB_STATE_NONE )
{
  m_MinPos = m_pMinPane->GetTranslate();
  m_MaxPos = m_pMaxPane->GetTranslate();

  m_ScrollRange = m_MinPos.y - m_MaxPos.y;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：値を設定
 *
 * @param   value   値(0.0f～1.0f)
 */
bool SlideScrollBar::Update( f32 value )
{
  f32 valuePos = _GetValuePos( value );
  gfl2::math::VEC3 nextPos = _GetNextPos( valuePos );

  _SetTabPanePos( nextPos );

  return true;
}

/**
 * @brief   更新：スライド操作
 *
 * @param   tp    タッチパネル
 */
bool SlideScrollBar::Update( gfl2::ui::TouchPanel* tp, bool isTouch )
{
  if( isTouch )
  {
    if( m_TabState == TAB_STATE_NONE )
    {
      if( _IsTouchTriggerTab( tp ) )
      {
        m_TabState = TAB_STATE_DRAG;
      }
    }
    else if( m_TabState == TAB_STATE_DRAG )
    {
      gfl2::math::VEC3 pos = _GetLayoutPos( tp );

      gfl2::math::VEC3 now = m_pTabPane->GetTranslate();

      if( m_ScrollDir == SCROLL_DIR_VERTICAL )
      {
        now.y = pos.y;
        now.y -= ( m_pTabPane->GetSize().height / 2.0f );
      }
      else {
        now.y = -pos.x;
        now.y += ( m_pTabPane->GetSize().height / 2.0f );
      }

      FUKUSHIMA_PRINT( "now y=[%.1f]\n", now.y );

      _SetTabPanePos( now );

      return true;
    }
  }
  else {
    m_TabState = TAB_STATE_NONE;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示切替
 */
void SlideScrollBar::SetVisible( bool visible )
{
  m_pBasePane->SetVisible( visible );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    取得関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   値の取得
 */
f32 SlideScrollBar::GetValue( void )
{
  gfl2::math::VEC3 value = m_MinPos - m_pTabPane->GetTranslate();

  f32 rate = 0.0f;

  rate = value.y / m_ScrollRange;

  return rate;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    非公開関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   タブペインの位置設定
 */
void SlideScrollBar::_SetTabPanePos( gfl2::math::VEC3 pos )
{
  _SetOutOfPos( &pos );

  m_pTabPane->SetTranslate( pos );
}

/**
 * @brief   移動領域外の設定
 */
void SlideScrollBar::_SetOutOfPos( gfl2::math::VEC3* pos )
{
  if( pos->y >= m_MinPos.y )
  {
    pos->y = m_MinPos.y;
  }
  else if( pos->y <= m_MaxPos.y )
  {
    pos->y = m_MaxPos.y;
  }
}


/**
 * @brief   値からタブの移動値を取得
 */
f32 SlideScrollBar::_GetValuePos( f32 value )
{
  return m_ScrollRange * value;
}

/**
 * @brief   移動先の座標値を取得
 */
gfl2::math::VEC3 SlideScrollBar::_GetNextPos( f32 valuePos )
{
  gfl2::math::VEC3 nextPos = m_MinPos;

  nextPos.y = m_MinPos.y - valuePos;

  return nextPos;
}


/**
 * @brief   タッチ座標の取得：スクリーン座標系
 */
gfl2::math::VEC3 SlideScrollBar::_GetTouchPos( gfl2::ui::TouchPanel* tp )
{
  gfl2::math::VEC3 pos( 0, 0, 0 );

  pos.x = static_cast<f32>( tp->GetX() );
  pos.y = static_cast<f32>( tp->GetY() );

  return pos;
}

/**
 * @brief   タッチ座標の取得：レイアウト座標系
 */
gfl2::math::VEC3 SlideScrollBar::_GetLayoutPos( gfl2::ui::TouchPanel* tp )
{
  gfl2::math::VEC3 pos = _GetTouchPos( tp );

  pos.x = pos.x - 160.0f;
  pos.y = 120.0f - pos.y;

  return pos;
}


/**
 * @brief   タブペインのトリガー判定
 */
bool SlideScrollBar::_IsTouchTriggerTab( gfl2::ui::TouchPanel* tp )
{
  if( tp->IsTouchTrigger() )
  {
    if( _IsTouchPane( tp, m_pTabPane ) )
    {
      FUKUSHIMA_PRINT( "Trigger\n" );
      return true;
    }
  }

  return false;
}

/**
 * @brief   タブペインのホールド判定
 */
bool SlideScrollBar::_IsTouchHoldTab( gfl2::ui::TouchPanel* tp )
{
  if( !tp->IsTouchTrigger() && tp->IsTouch() )
  {
    if( _IsTouchPane( tp, m_pTabPane ) )
    {
      FUKUSHIMA_PRINT( "Hold\n" );
      return true;
    }
  }

  return false;
}

/**
 * @brief   ペインのタッチ判定
 */
bool SlideScrollBar::_IsTouchPane( gfl2::ui::TouchPanel* tp, gfl2::lyt::LytPane* pane )
{
  gfl2::math::VEC3 pos = _GetTouchPos( tp );

  return m_pLytwk->GetHitPane( pos.x, pos.y, pane );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
