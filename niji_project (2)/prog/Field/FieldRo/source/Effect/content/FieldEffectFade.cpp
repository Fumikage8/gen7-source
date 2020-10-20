//======================================================================
/**
 * @file FieldEffectFade.cpp
 * @date 2015/08/26 18:14:46
 * @author miyachi_soichi
 * @brief エフェクトフェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectFade.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// layout
#include "Field/FieldRo/include/FieldLayoutPriority.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x6000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x6000;

/**
 *  @brief  コンストラクタ
 */
EffectFade::EffectFade( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
, m_nState( STATE_NONE )
{
}

/**
 *  @brief  デストラクタ
 */
EffectFade::~EffectFade( void )
{
}

/**
 *  @brief  初期化
 */
void EffectFade::Initialize( SetupData &setupData )
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
  m_pLayout->SetPriority( Field::FIELD_LAYOUT_PRIORITY_FADE );
  m_pLayout->SetVisible( false );
}

/**
 *  @brief  破棄
 */
bool EffectFade::Terminate( void )
{
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectFade::updateNormal( void )
{
  if( IsAnimationLastFrame() ){ return; }

  ++ m_nLifeNow;
  if( m_nLifeNow < m_nLifeMax )
  {
    m_ColorNow += m_ColorAdd;
  }
  else
  {
    // 終了
    m_nLifeNow = 0;
    m_nLifeMax = 0;
    m_ColorNow.Set( m_ColorEnd );
//    if( m_nState == STATE_IN )  ///< OUT を待てないので修正. 宮地さん了承 Modify. N.Takeda 2015/09/01
    m_nState = STATE_NONE;
  }

  SetColor();
}

/**
 *  @brief  終了しているか.
 */
bool EffectFade::IsAnimationLastFrame( void )
{
  return m_nState == STATE_NONE;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectFade::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  フェード
 */
void EffectFade::RequestOut( gfl2::math::Vector4 *start, gfl2::math::Vector4 *end, s32 life )
{
  m_nState = STATE_OUT;
  m_nLifeNow = 0;
  m_nLifeMax = life;
  m_ColorNow.Set( *start );
  m_ColorEnd.Set( *end );
  m_ColorAdd.SetX( ( end->x - start->x ) / static_cast<f32>( life ) );
  m_ColorAdd.SetY( ( end->y - start->y ) / static_cast<f32>( life ) );
  m_ColorAdd.SetZ( ( end->z - start->z ) / static_cast<f32>( life ) );
  m_ColorAdd.SetW( ( end->w - start->w ) / static_cast<f32>( life ) );

  SetColor();
  m_pLayout->SetVisible( true );
}
void EffectFade::RequestIn( s32 life )
{
  m_nState = STATE_IN;
  m_nLifeNow = 0;
  m_nLifeMax = life;
  m_ColorEnd.Set( m_ColorNow );
  m_ColorEnd.SetW( 0.0f );
  m_ColorAdd.SetX( 0.0f );
  m_ColorAdd.SetY( 0.0f );
  m_ColorAdd.SetZ( 0.0f );
  m_ColorAdd.SetW( ( 0.0f - m_ColorNow.w ) / static_cast<f32>( life ) );

  SetColor();
  m_pLayout->SetVisible( true );
}
void EffectFade::SetColor( void )
{
  m_pLayout->SetColor( 0, m_ColorNow );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
