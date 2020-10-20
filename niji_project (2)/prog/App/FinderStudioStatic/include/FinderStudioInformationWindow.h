#if !defined(__FINDER_STUDIO_INFORMATION_WINDOW_H_INCLUDED__)
#define __FINDER_STUDIO_INFORMATION_WINDOW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioInformationWindow.h
 * @date    2017/01/11 14:52:38
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：インフォメーションウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_Str.h>
#include <math/include/gfl2_Math.h>
#include <Layout/include/gfl2_Layout.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class InfoWindowBookType;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォメーションウィンドウ
///
//////////////////////////////////////////////////////////////////////////
class InformationWindow
{
private:
  enum {
    SEQ_INFO_IDLE,
    SEQ_INFO_REMOVE,
    SEQ_INFO_DELETE,
    SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  static void Update( void );
  static void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-------------------------------------------------------
  // @brief   セットアップ
  //-------------------------------------------------------
  static void AddSubView( app::ui::UIView* view ) { _AddSubView( view ); }

  static void SetupInfoWindow( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  static void ResetInfoWindow( void );

  static void SetupContents( u8 pageMax, gfl2::str::StrBuf** strArray );
  static void SetupContents( u8 pageMax, gfl2::str::StrBuf** strArray, const gfl2::lyt::LytResTextureInfo** texArray );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  static void Show( app::tool::InfoWindowBookTypeListener* listener, gfl2::math::VEC3 pos );
  static void Hide( void );

  //-------------------------------------------------------
  // @brief   チェック
  //-------------------------------------------------------
  static bool IsCanOpenWindow( void ) { return ( m_Seq == SEQ_INFO_IDLE ); }   //!< インフォウィンドウを開けるか？

private:
  //-------------------------------------------------------
  // @brief   生成・破棄：非公開
  //-------------------------------------------------------
  static void _CreateInfoWindow( u8 pageMax );
  static bool _DeleteInfoWindow( void );

  //-------------------------------------------------------
  // @brief   SubView：非公開
  //-------------------------------------------------------
  static void _AddSubView( app::ui::UIView* view );
  static void _RemoveSubView( void );

private:
  static app::util::Heap*                 m_pHeap;
  static app::util::AppRenderingManager*  m_pAppRenderingManager;
  static app::tool::InfoWindowBookType*   m_pInfoWindow;

  static u8   m_Seq;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_INFORMATION_WINDOW_H_INCLUDED__
