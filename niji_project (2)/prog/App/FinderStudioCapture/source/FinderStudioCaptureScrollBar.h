#if !defined( __FINDER_STUDIO_CAPTURE_SCROLL_BAR_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_SCROLL_BAR_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureScrollBar.h
 * @date    2017/02/14 12:20:39
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：専用スクロールバー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Layout/include/gfl2_LytSys.h>
#include <math/include/gfl2_Math.h>
#include <ui/include/gfl2_UI.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    専用スクロールバー
///
//////////////////////////////////////////////////////////////////////////
class SlideScrollBar
{
  GFL_FORBID_COPY_AND_ASSIGN( SlideScrollBar );

public:
  /// スクロール方向
  enum ScrollDirection
  {
    SCROLL_DIR_VERTICAL,    //!< 垂直
    SCROLL_DIR_HORIZONTAL,  //!< 水平
    SCROLL_DIR_NUM,
  };

  /// タブの状態
  enum TabState
  {
    TAB_STATE_NONE,
    TAB_STATE_DRAG,
    TAB_STATE_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  SlideScrollBar( gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytPane * parts, gfl2::lyt::LytPane * tab, gfl2::lyt::LytPane * min, gfl2::lyt::LytPane * max, ScrollDirection dir );
  virtual ~SlideScrollBar( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   更新
  //-------------------------------------------------------
  bool Update( f32 value );
  bool Update( gfl2::ui::TouchPanel* tp, bool isTouch );

  //-------------------------------------------------------
  // @brief   表示
  //-------------------------------------------------------
  void SetVisible( bool visible );

  //-------------------------------------------------------
  // @brief   取得関数
  //-------------------------------------------------------
  f32 GetValue( void );

private:
  //-------------------------------------------------------
  // @brief   非公開関数群
  //-------------------------------------------------------
  void _SetTabPanePos( gfl2::math::VEC3 pos );
  void _SetOutOfPos( gfl2::math::VEC3* pos );

  f32 _GetValuePos( f32 value );
  gfl2::math::VEC3 _GetNextPos( f32 valuePos );

  gfl2::math::VEC3 _GetTouchPos( gfl2::ui::TouchPanel* tp );
  gfl2::math::VEC3 _GetLayoutPos( gfl2::ui::TouchPanel* tp );

  bool _IsTouchTriggerTab( gfl2::ui::TouchPanel* tp );
  bool _IsTouchHoldTab( gfl2::ui::TouchPanel* tp );
  bool _IsTouchPane( gfl2::ui::TouchPanel* tp, gfl2::lyt::LytPane* pane );

private:
  gfl2::lyt::LytWk*     m_pLytwk;

  gfl2::lyt::LytPane*   m_pBasePane;
  gfl2::lyt::LytPane*   m_pTabPane;
  gfl2::lyt::LytPane*   m_pMinPane;
  gfl2::lyt::LytPane*   m_pMaxPane;

  gfl2::math::VEC3      m_MinPos;
  gfl2::math::VEC3      m_MaxPos;

  ScrollDirection       m_ScrollDir;

  f32   m_ScrollRange;

  TabState  m_TabState;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SCROLL_BAR_H_INCLUDED__
