//======================================================================
/**
 * @file    FinderStudioDecorationLowerBGView.cpp
 * @date    2017/02/08 17:28:33
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：背景View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationLowerBGView.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
BGView::BGView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : DecoViewBase( heap, renderMan )
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
      LYTRES_FINDERSTUDIODECO_STUDIO_DECO_LOW_BG_BFLYT,
      LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_BG___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // ViewUtilの生成
  this->CreateViewUtil();
  // Viewの表示
  this->_Show();
}

/**
 * @brief   初期化：アニメ
 */
void BGView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_BG__BG01_BG_LOOP_01 );
}

/**
 * @brief   破棄：公開
 */
bool BGView::Terminate( void )
{
  // ViewUtilの破棄
  this->DeleteViewUtil();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景アニメ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   背景アニメの現在フレームの取得
 *
 * @retval  現在フレーム
 */
f32 BGView::GetBackGroundAnimFrame( void )
{
  return m_pUtil->GetAnimFrame( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_BG__BG01_BG_LOOP_01 );
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
