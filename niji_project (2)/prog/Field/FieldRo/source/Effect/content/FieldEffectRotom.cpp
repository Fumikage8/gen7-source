//======================================================================
/**
 * @file FieldEffectRotom.cpp
 * @date 2015/11/09 12:59:31
 * @author saita_kazuki
 * @brief ロトム図鑑説明1枚絵表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Effect/content/FieldEffectRotom.h"

// field
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/FieldLayoutPriority.h"

// gfl2
#include <Layout/include/gfl2_LytSys.h>

// conv_header
#include <niji_conv_header/field/effect/rotom.h>
#include <niji_conv_header/field/effect/rotom_anim_list.h>
#include <niji_conv_header/field/effect/rotom_pane.h>

// arc
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x700;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x4800;

/**
 *  @brief  コンストラクタ
 */
EffectRotom::EffectRotom( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectRotom::~EffectRotom( void )
{
}

/**
 *  @brief  初期化
 */
void EffectRotom::Initialize( SetupData &setupData )
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
        0,
        app::util::G2DUtil::ATTACH_ROOT
      },
    };

    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        LYTRES_ROTOM_TRP_RTM_UPP_000_BFLYT,
        LA_ROTOM_TRP_RTM_UPP_000___NUM,
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

  this->Start();
}

/**
 *  @brief  破棄
 */
bool EffectRotom::Terminate( void )
{
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectRotom::updateNormal( void )
{
}

/**
 *  @brief  終了しているか.
 */
bool EffectRotom::IsAnimationLastFrame( void )
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil )
  {
    return true;
  }

  return pUtil->IsAnimeEnd( 0, LA_ROTOM_TRP_RTM_UPP_000_ROTOM);
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectRotom::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  開始
 */
void EffectRotom::Start()
{
  if( this->IsAnimationLastFrame() == false)
  {
    return;
  }

  m_pLayout->SetVisible( true );
  m_pLayout->StartAnimation( LA_ROTOM_TRP_RTM_UPP_000_ROTOM, false, true);
  m_pLayout->SetAnimationFrame( LA_ROTOM_TRP_RTM_UPP_000_ROTOM, 0.0f);
}

/**
 *  @brief  終了
 */
void EffectRotom::End( void )
{
  m_pLayout->SetVisible( false );
}

/**
 *  @brief  アニメーションを次に進める
 */
void EffectRotom::NextAnimation()
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( pUtil == NULL )
  {
    return;
  }

  f32 frame = pUtil->GetAnimeFrame( 0, LA_ROTOM_TRP_RTM_UPP_000_ROTOM);
  if( frame >= pUtil->GetAnimeMaxFrame( 0, LA_ROTOM_TRP_RTM_UPP_000_ROTOM))
  {
    return;
  }

  static const f32 ANIMATION_FRAME_ADD = 1.0f;
  f32 next = frame + ANIMATION_FRAME_ADD; 
  m_pLayout->SetAnimationFrame( LA_ROTOM_TRP_RTM_UPP_000_ROTOM, next);
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
