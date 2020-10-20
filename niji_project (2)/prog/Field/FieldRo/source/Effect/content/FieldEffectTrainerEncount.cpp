//======================================================================
/**
 * @file FieldEffectTrainerEncount.cpp
 * @date 2015/09/11 15:28:02
 * @author miyachi_soichi
 * @brief エフェクト：トレーナーエンカウント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerEncount.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// layout
#include "Field/FieldRo/include/FieldLayoutPriority.h"
#include "Sound/include/Sound.h"

#include "niji_conv_header/field/effect/trainer_encount_pane.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x6000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x6000;

/**
 *  @brief  コンストラクタ
 */
EffectTrainerEncount::EffectTrainerEncount( void )
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
EffectTrainerEncount::~EffectTrainerEncount( void )
{
}

/**
 *  @brief  初期化
 */
void EffectTrainerEncount::Initialize( SetupData &setupData )
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
        0,
        2,
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
  m_pLayout->SetVisible( false );
}

/**
 *  @brief  破棄
 */
bool EffectTrainerEncount::Terminate( void )
{
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectTrainerEncount::updateNormal( void )
{
}

/**
 *  @brief  終了しているか.
 */
bool EffectTrainerEncount::IsAnimationLastFrame( void )
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
void EffectTrainerEncount::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  アニメーション開始
 */
void EffectTrainerEncount::StartAnimation( gfl2::math::VEC2 pos, f32 fRotY, u32 se_id )
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil )
  {
    return;
  }
  m_pLayout->SetVisible( true );

  // ペインの操作
  {
    u32 nPaneIdx = PANENAME_TRAINEREYE_INT_UPP_000_PANE_EF_ROOT;
    gfl2::lyt::LytWk *pWork = pUtil->GetLayoutWork( 0 );
    gfl2::lyt::LytPane *pPane = pWork->GetPane( nPaneIdx );
    nw::math::VEC3 rot = pPane->GetRotate();
    rot.z = fRotY;
    pPane->SetRotate( rot );
    pPane->SetTranslate(pos);
  }

  // アニメーション設定
  pUtil->StartAnime( 0, m_nAnimeID );
  // Se
  if( IsPlaySe() && se_id != SMID_NULL )
  {
    Sound::PlaySE( se_id );
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
