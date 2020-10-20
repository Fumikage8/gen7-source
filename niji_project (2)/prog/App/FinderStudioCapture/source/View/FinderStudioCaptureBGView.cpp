//======================================================================
/**
 * @file    FinderStudioCaptureBGView.cpp
 * @date    2016/11/24 14:58:15
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：背景View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureBGView.h"

#include <AppLib/include/Util/app_util_2d.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>


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
BGView::BGView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : Static::ViewBase( heap, renderMan )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化：公開
 */
void BGView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // アニメの初期化
  InitAnim();
}

/**
 * @brief   2Dの初期化：非公開
 */
void BGView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  GFL_UNUSED( msgData );
  GFL_UNUSED( wordSet );

  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, this->UNUSE_MESSAGE_DATA );

  // 表示
  this->_Show();
}

/**
 * @brief   レイアウトアニメの初期設定：非公開
 */
void BGView::InitAnim( void )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();

  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00_TAG_00 );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_00_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_01_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_02_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_03_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_04_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_05_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_06_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_07_STUDIO_BG_00_STAR );
  g2d->StartAnime( 0, LA_FINDERSTUDIOCAPTURE_STUDIO_BG_LOW_00__STAR_08_STUDIO_BG_00_STAR );
}

/**
 * @brief   破棄：公開
 */
bool BGView::Terminate( void )
{
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
