#if !defined(__FINDER_STUDIO_VIEW_UTIL_H_INCLUDED__)
#define __FINDER_STUDIO_VIEW_UTIL_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioViewUtil.h
 * @date    2016/11/18 11:02:21
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewユーティリティ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Layout/include/gfl2_LytSys.h>
#include <macro/include/gfl2_Macros.h>
#include <ui/include/gfl2_UI.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_BEGIN( util )
class G2DUtil;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_BEGIN( tool )
class ButtonManager;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    Viewユーティリティ
///
//////////////////////////////////////////////////////////////////////////
class ViewUtil
{
public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ViewUtil( app::ui::UIView* view, u32 lytID = 0 );
  ~ViewUtil( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   UIView設定関数群
  //-------------------------------------------------------
  void SetPrevInputExclusiveKeyByBtnAnim( void );
  void SetInputExclusiveKeyByBtnAnim( bool flag );
  void ResetInputExclusiveKeyByBtnAnim( void );
  bool GetInputExclusiveKeyByBtnAnim( void ) const;

  void SetModal( bool modal );
  bool GetModal( void );

public:
  //-------------------------------------------------------
  // @brief   ペイン取得関数群
  //-------------------------------------------------------
  gfl2::lyt::LytPane*     GetPane( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );
  gfl2::lyt::LytTextBox*  GetTextBox( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );
  gfl2::lyt::LytBounding* GetBounding( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );
  gfl2::lyt::LytPicture*  GetPicture( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );
  gfl2::lyt::LytParts*    GetParts( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );

public:
  //-------------------------------------------------------
  // @brief   ペイン操作関数群
  //-------------------------------------------------------
  void SetVisiblePane( gfl2::lyt::LytPane* pane, bool visible );
  bool GetVisiblePane( gfl2::lyt::LytPane* pane ) const;

  void             SetPanePos( gfl2::lyt::LytPane* pane, gfl2::math::VEC3 pos );
  gfl2::math::VEC3 GetPanePos( gfl2::lyt::LytPane* pane ) const;

  void          SetPaneSize( gfl2::lyt::LytPane* pane, nw::lyt::Size size );
  nw::lyt::Size GetPaneSize( gfl2::lyt::LytPane* pane );

  void             SetPaneRotate( gfl2::lyt::LytPane* pane, gfl2::math::VEC3 rotate );
  gfl2::math::VEC3 GetPaneRotate( gfl2::lyt::LytPane* pane ) const;

  //-------------------------------------------------------
  // @brief   ペインタッチ判定
  //-------------------------------------------------------
  bool IsTouchPane( gfl2::ui::TouchPanel* tp, gfl2::lyt::LytPane* pane );
  bool IsTouchTriggerPane( gfl2::lyt::LytPane* pane );
  bool IsTouchHoldPane( gfl2::lyt::LytPane* pane );

public:
  //-------------------------------------------------------
  // @brief   ペインアニメ関数群
  //-------------------------------------------------------
  void StartAnim( u32 anmID, bool auto_anim = true, bool auto_unbind = true );
  void StopAnim( u32 anmID, bool clear = false );
  bool IsAnimEnd( u32 anmID );
  void SetAnimFrame( u32 anmID, f32 frame );
  f32  GetAnimFrame( u32 anmID );

public:
  //-------------------------------------------------------
  // @brief   ボタン関数群
  //-------------------------------------------------------
  void LockButton( void );
  void UnlockButton( void );

  bool IsButtonAction( void );

public:
  //-------------------------------------------------------
  // @brief   取得関数群
  //-------------------------------------------------------
  app::ui::UIView*                    GetView( void ) const;
  app::util::G2DUtil*                 GetG2D( void ) const;
  app::tool::ButtonManager*           GetButtonManager( void ) const;
  gfl2::lyt::LytWk*                   GetLytWk( void ) const;
  gfl2::lyt::LytRes*                  GetLytRes( void ) const;
  gfl2::lyt::LytMultiResID            GetMultiResID( void ) const;
  const gfl2::lyt::LytResTextureInfo* GetTextureInfo( u32 dataID );
  u32                                 GetLytID( void ) const;

private:
  app::ui::UIView*          m_pUIView;
  gfl2::lyt::LytWk*         m_pLytWk;
  gfl2::lyt::LytMultiResID  m_ResID;
  u32                       m_LayoutID;
  bool                      m_bInputBtnAnim;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_VIEW_UTIL_H_INCLUDED__