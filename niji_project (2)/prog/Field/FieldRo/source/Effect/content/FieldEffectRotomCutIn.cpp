//======================================================================
/**
 * @file FieldEffectRotomCutIn.cpp
 * @date 2017/02/07 21:27:39
 * @author kawazoe_shinichi
 * @brief ロトムカットインエフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Effect/content/FieldEffectRotomCutIn.h"

// field
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/FieldLayoutPriority.h"

// gfl2
#include <Layout/include/gfl2_LytSys.h>

// conv_header layout
#include <niji_conv_header/field/effect/rotom_cut_in.h>
#include <niji_conv_header/field/effect/rotom_cut_in_anim_list.h>
#include <niji_conv_header/field/effect/rotom_cut_in_pane.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x3000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x6000;

/**
 *  @brief  コンストラクタ
 */
EffectRotomCutIn::EffectRotomCutIn(void)
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectRotomCutIn::~EffectRotomCutIn(void)
{
}

/**
 *  @brief  初期化
 */
void EffectRotomCutIn::Initialize(SetupData &setupData)
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
      {
        setupData.pResource,
        2,
        app::util::G2DUtil::ATTACH_ROOT
      },
    };

    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        LYTRES_ROTOM_CUT_IN_ROTOM_CUTIN_BFLYT,
        LA_ROTOM_CUT_IN_ROTOM_CUTIN___NUM,
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
  m_pLayout->SetVisible( false );
  m_pLayout->SetPriority( Field::FIELD_LAYOUT_PRIORITY_3D );
}

/**
 *  @brief  破棄
 */
bool EffectRotomCutIn::Terminate(void)
{
  // 一応呼ぶ
  this->End();

  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectRotomCutIn::updateNormal(void)
{
}

/**
 *  @brief  終了しているか.
 */
bool EffectRotomCutIn::IsAnimationLastFrame(void)
{
  if (this->IsLayoutInitialized() == false)
  {
    return true;
  }

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if (pUtil && pUtil->IsAnimeEnd(0, LA_ROTOM_CUT_IN_ROTOM_CUTIN_CUTIN))
  {
    return true;
  }
  return false;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectRotomCutIn::Delete(void)
{
  this->End();

  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  開始
 */
void EffectRotomCutIn::Start(void)
{
  if( this->IsLayoutInitialized() == false)
  {
    GFL_ASSERT_MSG( 0, "Layout is not initialize\n");
    return;
  }

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();

  m_pLayout->SetVisible( true );
  m_pLayout->StartAnimation(LA_ROTOM_CUT_IN_ROTOM_CUTIN_CUTIN, true, true);
  pUtil->SetAnimeSpeed( 0, 1.0f);

}

/**
 *  @brief  終了
 */
void EffectRotomCutIn::End(void)
{
  if( this->IsLayoutInitialized() == false )
  {
    return;
  }

  m_pLayout->SetVisible( false );

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( pUtil)
  {
    pUtil->StopAnime(0, LA_ROTOM_CUT_IN_ROTOM_CUTIN_CUTIN);
  }
}



/**
 *  @brief  レイアウト初期化が完了しているか
 */
bool EffectRotomCutIn::IsLayoutInitialized()
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( pUtil )
  {
    return true;
  }
  return false;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
