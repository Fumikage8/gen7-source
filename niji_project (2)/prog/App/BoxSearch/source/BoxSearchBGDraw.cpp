//======================================================================
/**
 * @file    BoxSearchBGDraw.cpp
 * @date    2016/02/01 13:16:00
 * @author  fukushima_yuya
 * @brief   ボックス検索：背景
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchBGDraw.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// arc
#include <niji_conv_header/app/box/res2d/bgrupp.h>  //!< 背景
#include <niji_conv_header/app/box/res2d/bgrupp_pane.h>
#include <niji_conv_header/app/box/res2d/bgrupp_anim_list.h>
#include <niji_conv_header/app/box/res2d/bgrlow.h>  //!< 背景
#include <niji_conv_header/app/box/res2d/bgrlow_pane.h>
#include <niji_conv_header/app/box/res2d/bgrlow_anim_list.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
BoxSearchBGDraw::BoxSearchBGDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap(pHeap)
  , m_pAppRenderingManager(pAppRanderingManager)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchBGDraw::Init( InitParam& param )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  // 2D関連作成
  Initialize2D( param );
  // BGアニメの開始
  StartBGAnim( param.dispMode );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchBGDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_BGRUPP_BOX_BGR_UPP_000_BFLYT,
      LA_BGRUPP_BOX_BGR_UPP_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_BGRLOW_BOX_BGR_LOW_000_BFLYT,
      LA_BGRLOW_BOX_BGR_LOW_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    &setupData[param.dispMode],
    1,
    this->UNUSE_MESSAGE_DATA );

  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   BGアニメの開始
 *
 * @param   dispMode    ディスプレイ
 */
//------------------------------------------------------------------------------
void BoxSearchBGDraw::StartBGAnim( BoxSearchBGDraw::DispMode dispMode )
{
  GFL_ASSERT( dispMode < DISP_MODE_NUM );

  app::util::G2DUtil* g2d = this->GetG2DUtil();
  {
    static const u32 c_BGAnim[] = {
      LA_BGRUPP_BOX_BGR_UPP_000_KEEP,
      LA_BGRLOW_BOX_BGR_LOW_000_BG_KEEP,
    };

    g2d->StartAnime( 0, c_BGAnim[dispMode] );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool BoxSearchBGDraw::End( void )
{
  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void BoxSearchBGDraw::Update( void )
{
  app::ui::UIView::Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void BoxSearchBGDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    u8 order = (gfl2::lyt::ORDER_NORMAL - 1);
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, order );
  }
}

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)
