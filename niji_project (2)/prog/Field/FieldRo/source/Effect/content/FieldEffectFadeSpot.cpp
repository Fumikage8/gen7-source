//======================================================================
/**
 * @file FieldEffectFadeSpot.cpp
 * @date 2015/10/15 12:46:46
 * @author miyachi_soichi
 * @brief エフェクト：スポットフェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectFadeSpot.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// layout
#include "Field/FieldRo/include/FieldLayoutPriority.h"

#include "niji_conv_header/field/effect/fade_spot.h"
#include "niji_conv_header/field/effect/fade_spot_anim_list.h"
#include "niji_conv_header/field/effect/fade_spot_pane.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x6000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x9000;

/**
 *  @brief  コンストラクタ
 */
EffectFadeSpot::EffectFadeSpot( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
, m_nAnimeID( 0 )
{
}

/**
 *  @brief  デストラクタ
 */
EffectFadeSpot::~EffectFadeSpot( void )
{
}

/**
 *  @brief  初期化
 */
void EffectFadeSpot::Initialize( SetupData &setupData )
{
  m_pWindowManager = setupData.pWinMgr;
  // ローカルヒープ作成
  m_appUtilHeap.LocalHeapCreate( setupData.pHeap, setupData.pHeap, EFFECT_LAYOUT_HEAP_SIZE, EFFECT_LAYOUT_DEVHEAP_SIZE );
  // レイアウト生成
  app::util::AppRenderingManager *pRender = m_pWindowManager->GetRenderingManager();
  m_pLayout = GFL_NEW( setupData.pHeap ) Field::Effect::EffectLayout( &m_appUtilHeap, pRender );
  // レイアウト初期化
  {
    app::util::G2DUtil::LytResData resTbl[] = 
    {
      { setupData.pResource, 2, app::util::G2DUtil::ATTACH_ROOT },
    };
    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        LYTRES_FADE_SPOT_FADE_SPOT_BG_UPPER_BFLYT,
        LA_FADE_SPOT_FADE_SPOT_BG_UPPER___NUM,
        setupData.pResource,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };
    Field::Effect::EffectLayout::SetupParam param;
    param.layoutNum = 1;
    param.lytResTbl = resTbl;
    param.resTblNum = GFL_NELEMS( resTbl );
    param.lytDataTbl = dataTbl;
    param.dataTblNum = GFL_NELEMS( dataTbl );
    m_pLayout->Initialize( param );
  }
  // 登録
  m_pWindowManager->GetUIView()->AddSubView( m_pLayout );
  m_pLayout->SetPriority( Field::FIELD_LAYOUT_PRIORITY_NORMAL );
  m_pLayout->SetVisible( true );
}

/**
 *  @brief  破棄
 */
bool EffectFadeSpot::Terminate( void )
{
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectFadeSpot::updateNormal( void )
{
}

/**
 *  @brief  終了しているか.
 */
bool EffectFadeSpot::IsAnimationLastFrame( void )
{
  bool bResult = true;

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil ){ return bResult; }

  bResult = pUtil->IsAnimeEnd( 0, m_nAnimeID );

  return bResult;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectFadeSpot::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  アニメーション開始
 */
void EffectFadeSpot::StartAnimation( gfl2::math::VEC2 pos, f32 sync, bool bOut )
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil )
  {
    return;
  }

  const f32 MAX_FRAME = 20.0f;
  f32 rate = MAX_FRAME / sync;

  // ペインの操作
  if( bOut )
  {
    m_nAnimeID = LA_FADE_SPOT_FADE_SPOT_BG_UPPER_WCS_OUT;
    pUtil->SetPaneVisible( 0, PANENAME_FADE_SPOT_BG_UPPER_PANE_WCS_IN, false );
    pUtil->SetPaneVisible( 0, PANENAME_FADE_SPOT_BG_UPPER_PANE_WCS_OUT, true );
    gfl2::lyt::LytWk *pWork = pUtil->GetLayoutWork( 0 );
    gfl2::lyt::LytPane *pPane = pWork->GetPane( PANENAME_FADE_SPOT_BG_UPPER_PANE_WCS_OUT );
    pPane->SetTranslate(pos);
    // アニメーション設定
    pUtil->StartAnime( 0, m_nAnimeID );
    pWork->SetAutoAnimeSpeed( rate );
  }
  else
  {
    m_nAnimeID = LA_FADE_SPOT_FADE_SPOT_BG_UPPER_WCS_IN;
    pUtil->SetPaneVisible( 0, PANENAME_FADE_SPOT_BG_UPPER_PANE_WCS_OUT, false );
    pUtil->SetPaneVisible( 0, PANENAME_FADE_SPOT_BG_UPPER_PANE_WCS_IN, true );
    gfl2::lyt::LytWk *pWork = pUtil->GetLayoutWork( 0 );
    gfl2::lyt::LytPane *pPane = pWork->GetPane( PANENAME_FADE_SPOT_BG_UPPER_PANE_WCS_IN );
    pPane->SetTranslate(pos);
    // アニメーション設定
    pUtil->StartAnimeFrame( 0, m_nAnimeID, 1.0f );
    pWork->SetAutoAnimeSpeed( rate );
  }

}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
