//======================================================================
/**
 * @file FieldEffectRain.cpp
 * @date 2015/09/03 11:22:53
 * @author saito_nozomu
 * @brief 雨
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectRain.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

//conv_header
#include "niji_conv_header/field/effect/rain.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const float RAIN_BILLBOARD_LENGTH = 34;
static const float THUNDER_BILLBOARD_LENGTH = 32;

/**
 *  @class  EffectRain
 *  @brief  雨
 */
/**
 *  @brief  コンストラクタ
 */
EffectRain::EffectRain( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
{
  m_pEffectHandle = NULL;
  m_pResource  = NULL;
  m_pPipeLine = NULL;
  m_pAllocator  = NULL;
  for( u32 i=0; i<BILLBOARD_TYPE_NUM; ++i )
  {
    m_pModelResourceRootNode[i] = NULL;
    m_pModelInstanceNode[i] = NULL;
    m_pMotionResourceRootNode[i] = NULL;
  }
  m_pFieldRootNode = NULL;
  m_ResID = EFFECT_RESOURCE_SKY_RAIN;
  m_EmitterLength = 0;
}

/**
 *  @brief  デストラクタ
 */
EffectRain::~EffectRain( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectRain::Initialize( SetupData &setupData )
{
  
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;

  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

  // アロケータ作成
  m_pAllocator = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::util::GLHeapAllocator( m_pEffectHeap->GetGFHeapBase() );
  
  m_pFieldRootNode = setupData.pRootNode;
  m_pResource = setupData.pResource;
  m_pPipeLine = setupData.pPipeLine;

  for( u32 i=0; i<BILLBOARD_TYPE_NUM; ++i )
  {
    m_pModelResourceRootNode[i] = NULL;
    m_pModelInstanceNode[i] = NULL;
    m_pMotionResourceRootNode[i] = NULL;
  }
  m_ResID = EFFECT_RESOURCE_SKY_RAIN;
  m_EmitterLength = 0;
}

/**
 *  @brief  破棄
 */
bool EffectRain::Terminate( void )
{
  // 再生停止
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }

  for (int i=0; i<BILLBOARD_TYPE_NUM;i++)
  {
    if( m_Model[i].IsCreated() )
    {
      m_pPipeLine->RemoveWeatherBoardRenderingTarget( m_Model[i].GetModelInstanceNode() );
      m_Model[i].Destroy();
    }

    if(m_pModelResourceRootNode[i]){
      m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode[i] );
    }
    GFL_SAFE_DELETE( m_pModelResourceRootNode[i] );

    if(m_pMotionResourceRootNode[i]){
      m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode[i] );
    }
    GFL_SAFE_DELETE( m_pMotionResourceRootNode[i] );
  }
  

  GFL_SAFE_DELETE( m_pAllocator );
  GFL_SAFE_DELETE( m_pEffectHandle );
  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectRain::Delete( void )
{
  if(m_ResID == EFFECT_RESOURCE_SKY_RAIN)
  {
    // レンダリング対象からはずす
    m_pPipeLine->RemoveWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_RAIN].GetModelInstanceNode());
    m_pPipeLine->RemoveWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_THUNDER].GetModelInstanceNode());
    m_pPipeLine->RemoveWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_MIST].GetModelInstanceNode());
  }
}

/**
 *  @brief 雨
 *  @param emitter_pos エミッター位置
 *  @param pos 座標
 *  @param dir 方向
 *  @param rot　回転
 *  @param emitter_length　距離
 */
void EffectRain::CreateRain( const gfl2::math::Vector3 &emitter_pos, const gfl2::math::Vector3 &pos, float emitter_length )
{
  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, emitter_pos, 0, EFFECT_RESOURCE_SKY_RAIN );
  
  m_ResID = EFFECT_RESOURCE_SKY_RAIN;
  m_EmitterLength = emitter_length;

  CreateBillboard();

}
/**
 *  @brief 雨地面跳ね返り
 *  @param 座標
 */
void EffectRain::CreateSprash( const gfl2::math::Vector3 &pos )
{
  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_GROUND_RAIN );

  m_ResID = EFFECT_RESOURCE_GROUND_RAIN;
}

/**
 *  @brief 雷モデルを取得
 *  @return 雷モデルへのポインタ
 */
poke_3d::model::BaseModel* EffectRain::GetThunderModel(void)
{
  GFL_ASSERT(m_ResID == EFFECT_RESOURCE_SKY_RAIN);
  return &(m_Model[BILLBOARD_TYPE_THUNDER]);
}

/**
 *  @brief 雨モデルを取得
 *  @return 雨モデルへのポインタ
 */
poke_3d::model::BaseModel* EffectRain::GetRainModel(void)
{
  GFL_ASSERT(m_ResID == EFFECT_RESOURCE_SKY_RAIN);
  return &(m_Model[BILLBOARD_TYPE_RAIN]);
}

/**
 *  @brief 霧モデルを取得
 *  @return 霧モデルへのポインタ
 */
poke_3d::model::BaseModel* EffectRain::GetMistModel(void)
{
  GFL_ASSERT(m_ResID == EFFECT_RESOURCE_SKY_RAIN);
  return &(m_Model[BILLBOARD_TYPE_MIST]);
}
  
/**
 *  @brief ビルボードの作成
 */
void EffectRain::CreateBillboard( void )
{
  {
    gfl2::math::Vector3 rain_pos(0,0,-RAIN_BILLBOARD_LENGTH);
    int index = BL_IDX_RAIN_EF_WEATHER_RAIN_BILL_CTR_GFBMDLP;
    int anm_index = BL_IDX_RAIN_EF_WEATHER_RAIN_BILL_GFBMOT;
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;
    binLinkerAccessor.Initialize( m_pResource );
    // モデルを作る
    void *pModelData = binLinkerAccessor.GetData( index  );
    createModel( BILLBOARD_TYPE_RAIN, pModelData );
    m_Model[BILLBOARD_TYPE_RAIN].Create( m_pAllocator, m_pEffectHeap->GetGFHeapBase(), m_pModelInstanceNode[BILLBOARD_TYPE_RAIN] );
    // レンダリング対象に追加
    m_pPipeLine->AddWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_RAIN].GetModelInstanceNode());
    // モーションを作る
    void *pMotionData = binLinkerAccessor.GetData( anm_index );
    createMotion( BILLBOARD_TYPE_RAIN, pMotionData );
    m_Model[BILLBOARD_TYPE_RAIN].ChangeAnimationByResourceNode( m_pMotionResourceRootNode[BILLBOARD_TYPE_RAIN] );
    m_Model[BILLBOARD_TYPE_RAIN].SetAnimationStepFrame(1.0f);
    m_Model[BILLBOARD_TYPE_RAIN].SetAnimationLoop(true);
    // 描画登録
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode[BILLBOARD_TYPE_RAIN] );
    // 座標を指定
    m_Model[BILLBOARD_TYPE_RAIN].SetPosition( rain_pos );
    m_Model[BILLBOARD_TYPE_RAIN].SetVisible(false);
  }

  {
    gfl2::math::Vector3 thunder_pos(0,0,-THUNDER_BILLBOARD_LENGTH);
    int index = BL_IDX_RAIN_EF_WEATHER_THUNDER_BILL_CTR_GFBMDLP;
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;
    binLinkerAccessor.Initialize( m_pResource );
    // モデルを作る
    void *pModelData = binLinkerAccessor.GetData( index  );
    createModel( BILLBOARD_TYPE_THUNDER, pModelData );
    m_Model[BILLBOARD_TYPE_THUNDER].Create( m_pAllocator, m_pEffectHeap->GetGFHeapBase(), m_pModelInstanceNode[BILLBOARD_TYPE_THUNDER] );
    // レンダリング対象に追加
    m_pPipeLine->AddWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_THUNDER].GetModelInstanceNode());
    // 描画登録
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode[BILLBOARD_TYPE_THUNDER] );
    // 座標を指定
    m_Model[BILLBOARD_TYPE_THUNDER].SetPosition( thunder_pos );
    m_Model[BILLBOARD_TYPE_THUNDER].SetVisible(false);
    gfl2::math::Vector4 col = m_Model[BILLBOARD_TYPE_THUNDER].GetMaterialColorConstant(0,0);
    col.w = 0.1;
    m_Model[BILLBOARD_TYPE_THUNDER].SetMaterialColorConstant(0,0,col);
  }

  {
    gfl2::math::Vector3 mist_pos(0,0,-RAIN_BILLBOARD_LENGTH);
    int index = BL_IDX_RAIN_EF_WEATHER_RAIN_MIST_BILL_CTR_GFBMDLP;
    int anm_index = BL_IDX_RAIN_EF_WEATHER_RAIN_MIST_BILL_GFBMOT;
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;
    binLinkerAccessor.Initialize( m_pResource );
    // モデルを作る
    void *pModelData = binLinkerAccessor.GetData( index  );
    createModel( BILLBOARD_TYPE_MIST, pModelData );
    m_Model[BILLBOARD_TYPE_MIST].Create( m_pAllocator, m_pEffectHeap->GetGFHeapBase(), m_pModelInstanceNode[BILLBOARD_TYPE_MIST] );
    // レンダリング対象に追加
    m_pPipeLine->AddWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_MIST].GetModelInstanceNode());
    // モーションを作る
    void *pMotionData = binLinkerAccessor.GetData( anm_index );
    createMotion( BILLBOARD_TYPE_MIST, pMotionData );
    m_Model[BILLBOARD_TYPE_MIST].ChangeAnimationByResourceNode( m_pMotionResourceRootNode[BILLBOARD_TYPE_MIST] );
    m_Model[BILLBOARD_TYPE_MIST].SetAnimationStepFrame(1.0f);
    m_Model[BILLBOARD_TYPE_MIST].SetAnimationLoop(true);
    // 描画登録
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode[BILLBOARD_TYPE_MIST] );
    // 座標を指定
    m_Model[BILLBOARD_TYPE_MIST].SetPosition( mist_pos );
    m_Model[BILLBOARD_TYPE_MIST].SetVisible(false);
  }
}
#if 0
/**
 *  @brief  座標セット
 */
void EffectRain::SetPosition( gfl2::math::Vector3 pos )
{
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    nw::math::VEC3 nwPos( pos.x, pos.y, pos.z );

    // rotate
    nw::math::VEC3 nwRot  ( 0, 0, 0 );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }

  // 座標を指定
  m_Model[BILLBOARD_TYPE_RAIN].SetPosition( pos );
}
#endif
/**
 *  @brief  座標更新
 */
void EffectRain::UpdatePosition( const gfl2::math::Vector3 &pos, const gfl2::math::Vector3 &dir )
{
  gfl2::math::Vector3 emitter_pos = pos + (dir * m_EmitterLength);
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    nw::math::VEC3 nwPos( emitter_pos.x, emitter_pos.y, emitter_pos.z );

    // rotate
    f32 x, y, z;
    nw::math::VEC3 nwRot  ( 0, 0, 0 );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
  
}

/**
 *  @brief 削除確認
 */
bool EffectRain::IsDead(void)
{
  // 再生終了しているなら削除
  if( !m_pEffectHandle->GetEmitterSet() ){ return true; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {  
    return true;
  }
  return false;
}

void EffectRain::VisibleEmitter(bool visible)
{
  if ( m_pEffectHandle->GetEmitterSet() )
  {
    int num = m_pEffectHandle->GetEmitterSet()->GetNumCreatedEmitter();
    for (int i=0;i<num;i++)
    {
      m_pEffectHandle->GetEmitterSet()->GetEmitterController(i)->SetVisible(visible);
    }
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectRain::updateNormal( void )
{
  if (m_ResID == EFFECT_RESOURCE_SKY_RAIN)
  {
    m_Model[BILLBOARD_TYPE_RAIN].UpdateAnimation();
    m_Model[BILLBOARD_TYPE_MIST].UpdateAnimation();
  }
}
#if 0
/**
 *  @brief  座標更新
 */
void EffectRain::updatePosition( void )
{
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    gfl2::math::Vector3 pos = m_pParent->GetPosition() + m_offsetPosition;
    nw::math::VEC3 nwPos( pos.x, pos.y, pos.z );

    // rotate
    f32 x, y, z;
    gfl2::math::Quaternion qua = m_pParent->GetRotationQuat();
    qua.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot  ( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

/**
 *  @brief  座標オフセットの設定
 */
void EffectRain::setOffsetPosition( gfl2::math::Vector3 offsetPos )
{
  m_offsetPosition = offsetPos;
}
#endif

/**
 *  @brief  モデルの作成
 */
void EffectRain::createModel( int index, void *pModelBinary )
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelBinary ) );
  m_pModelResourceRootNode[index] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode[index] );

  // モデルインスタンス作成
  m_pModelInstanceNode[index] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode[index] );
}

/**
 *  @brief  モーションの作成
 */
void EffectRain::createMotion( int index, void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode[index] =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode[index] );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
