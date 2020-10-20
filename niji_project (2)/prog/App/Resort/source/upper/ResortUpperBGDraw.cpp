//======================================================================
/**
 * @file    ResortUpperBGDraw.cpp
 * @date    2015/09/08 18:39:49
 * @author  fukushima_yuya
 * @brief   ポケリゾート：BG画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Resort/source/upper/ResortUpperBGDraw.h>

#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
UpperBGDraw::UpperBGDraw( CreateParam& param )
  : UpperBaseDraw( param )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperBGDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_BGR_UPP_000_BFLYT,
      LA_RESORT_RESORT_BGR_UPP_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    this->UNUSE_MESSAGE_DATA );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期アニメ
 */
//------------------------------------------------------------------------------
void UpperBGDraw::InitializeAnim( void )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();
  {
    g2d->StartAnime( 0, LA_RESORT_RESORT_BGR_UPP_000_BG_KEEP );
  }
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
