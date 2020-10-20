//======================================================================
/**
* @file FieldEffectTrainerFocus.cpp
* @date 2015/09/02 12:12:48
* @author miyachi_soichi
* @brief 地名表示
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerFocus.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// arc
#include <niji_conv_header/field/effect/trainer_focus.h>
#include <niji_conv_header/field/effect/trainer_focus_anim_list.h>
#include <niji_conv_header/field/effect/trainer_focus_pane.h>
#include <arc_index/message.gaix>

#include <Layout/include/gfl2_LytSys.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x3000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x6000;

/**
*  @brief  コンストラクタ
*/
EffectTrainerFocus::EffectTrainerFocus( void )
  : IEffectBase()
  , m_appUtilHeap()
  , m_pLayout( NULL )
  , m_pWindowManager( NULL )
{
}

/**
*  @brief  デストラクタ
*/
EffectTrainerFocus::~EffectTrainerFocus( void )
{
}

/**
*  @brief  初期化
*/
void EffectTrainerFocus::Initialize( SetupData &setupData )
{
  m_pWindowManager = setupData.pWinMgr;
  // ローカルヒープ作成
  m_appUtilHeap.LocalHeapCreate( setupData.pHeap, setupData.pHeap, EFFECT_LAYOUT_HEAP_SIZE, EFFECT_LAYOUT_DEVHEAP_SIZE );
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
          LYTRES_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_BFLYT,
          LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000___NUM,
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

  m_eState = STATE_NONE;
}

/**
*  @brief  破棄
*/
bool EffectTrainerFocus::Terminate( void )
{
  Delete(); // 大野さん依頼：UIView切り離し呼出
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
*  @brief  更新
*/
void EffectTrainerFocus::updateNormal( void )
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil ){ return; }

  switch( m_eState )
  {
  case STATE_IN:
    {
      break;
    }
  case STATE_OUT:
    {
      // 画面外に消えたらOFF
      if( pUtil->IsAnimeEnd( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00 ) )
      {
        m_pLayout->SetVisible( false );
        m_eState = STATE_NONE;
      }

      break;
    }
  }
}

/**
*  @brief  終了しているか.
*/
bool EffectTrainerFocus::IsAnimationLastFrame( void )
{
  return true;
}

/**
*  @brief  各エフェクトで必要な破棄作業
*/
void EffectTrainerFocus::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
*  @brief  開始
*/
void EffectTrainerFocus::StartIn( u32 frame )
{
  if( m_eState != STATE_IN )
  {
    app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
    if( !pUtil ){ return; }

    if( pUtil->IsAnimeEnd( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00 ) )
    {
      pUtil->StartAnime( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00, true, false );
    }
    else
    {
      // 現在のフレームから再生(逆再生中)
      pUtil->StartAnimeFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00, pUtil->GetAnimeFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00 ), true, false );
    }

    f32 animeSize = pUtil->GetAnimeMaxFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00 ) - pUtil->GetAnimeFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00 );
    pUtil->SetAnimeSpeed( 0, animeSize / frame );
    m_pLayout->SetVisible( true );
    m_eState = STATE_IN;
  }
}

void EffectTrainerFocus::StartOut( u32 frame )
{
  if( m_eState == STATE_IN )
  {
    app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
    if( !pUtil ){ return; }

    // 現在のフレームから逆再生
    pUtil->StartAnimeFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00, pUtil->GetAnimeFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00 ) );
    pUtil->SetAnimeSpeed( 0, pUtil->GetAnimeFrame( 0, LA_TRAINER_FOCUS_TRAINERFOCUS_INT_UPP_000_TRAINERFOCUS_00  ) / frame * -1.0f );
    m_pLayout->SetVisible( true );
    m_eState = STATE_OUT;
  }
}
 


GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)