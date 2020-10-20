#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_UI_UTILITY_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_UI_UTILITY_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestUIUtility.h
 * @date    2016/10/27 18:38:15
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Layout/include/gfl2_LytSys.h>
#include <macro/include/gfl2_Macros.h>

//----------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------
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
// ---[ end ]---


//----------------------------------------------------------------------
// @brief   ファインダースタジオデコテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class UIUtility
{
public:
  UIUtility( app::ui::UIView* view, u32 lytID = 0 );
  ~UIUtility( void ) { ; }

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
  void SetVisiblePane( gfl2::lyt::LytPane* pPane, bool visible );
  bool GetVisiblePane( gfl2::lyt::LytPane* pPane ) const;

  void             SetPanePos( gfl2::lyt::LytPane* pPane, gfl2::math::VEC3 pos );
  gfl2::math::VEC3 GetPanePos( gfl2::lyt::LytPane* pPane ) const;

public:
  //-------------------------------------------------------
  // @brief   取得関数群
  //-------------------------------------------------------
  app::ui::UIView*          GetView( void ) const;
  app::util::G2DUtil*       GetG2D( void ) const;
  app::tool::ButtonManager* GetButtonManager( void ) const;
  gfl2::lyt::LytWk*         GetLytWk( void ) const;
  gfl2::lyt::LytRes*        GetLytRes( void ) const;
  gfl2::lyt::LytMultiResID  GetMultiResID( void ) const;
  u32                       GetLytID( void ) const;

public:
  //-------------------------------------------------------
  // @brief   ペイン生成関数群
  //-------------------------------------------------------
  static gfl2::lyt::LytPicture* CreatePicturePane( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytPicture* pane );

private:
  app::ui::UIView*          m_pUIView;
  gfl2::lyt::LytWk*         m_pLytWk;
  gfl2::lyt::LytMultiResID  m_ResID;
  u32                       m_LayoutID;
  bool                      m_bInputBtnAnim;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_UI_UTILITY_H_INCLUDED__

#endif // #if PM_DEBUG
