//======================================================================
/**
 * @file FieldEffectTrialStart.cpp
 * @date 2015/10/14 14:40:02
 * @author miyachi_soichi
 * @brief エフェクト：試練開始
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectTrialStart.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// layout
#include "Field/FieldRo/include/FieldLayoutPriority.h"
// arc
#include <niji_conv_header/field/effect/trial_start.h>
#include <niji_conv_header/field/effect/trial_start_anim_list.h>
#include <niji_conv_header/field/effect/trial_start_pane.h>
// sound
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x6000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x8000;

/**
 *  @brief  コンストラクタ
 */
EffectTrialStart::EffectTrialStart( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectTrialStart::~EffectTrialStart( void )
{
}

/**
 *  @brief  初期化
 */
void EffectTrialStart::Initialize( SetupData &setupData )
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
        LYTRES_TRIAL_START_SHIREN_START_UP_000_BFLYT,
        LA_TRIAL_START_SHIREN_START_UP_000___NUM,
        setupData.pResource,
        false,
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
  // 開始
  m_nAnimeID = LA_TRIAL_START_SHIREN_START_UP_000_KEEPANIM;
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  pUtil->StartAnime( 0, m_nAnimeID );
  // サウンド
  if( IsPlaySe() )
  {
    Sound::PlaySE( SEQ_SE_FLD_SHIRENKAISHI_OBI );
    Sound::PlaySE( SEQ_SE_FLD_SHIRENKAISHI_FLASH );
    Sound::PlaySE( SEQ_SE_FLD_SHIRENKAISHI_DISP );
    Sound::PlaySE( SEQ_SE_FLD_SHIRENKAISHI_OUT );
  }
}

/**
 *  @brief  破棄
 */
bool EffectTrialStart::Terminate( void )
{
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectTrialStart::updateNormal( void )
{
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectTrialStart::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  終了しているか.
 */
bool EffectTrialStart::IsAnimationLastFrame( void )
{
  bool bResult = true;

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil ){ return bResult; }

  bResult = pUtil->IsAnimeEnd( 0, m_nAnimeID );

  return bResult;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
