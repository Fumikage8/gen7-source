//======================================================================
/**
 * @file FieldEffectSnow.cpp
 * @date 2015/09/03 11:22:53
 * @author saito_nozomu
 * @brief 雪
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
#include "Field/FieldRo/include/Effect/content/FieldEffectSnow.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

//conv_header
#include "niji_conv_header/field/effect/snow.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const float SNOW_BILLBOARD_LENGTH = 34;

/**
 *  @class  EffectSnow
 *  @brief  雪
 */
/**
 *  @brief  コンストラクタ
 */
EffectSnow::EffectSnow( void )
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
  m_ResID = EFFECT_RESOURCE_SKY_SNOW;
  m_EmitterLength = 0;
}

/**
 *  @brief  デストラクタ
 */
EffectSnow::~EffectSnow( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectSnow::Initialize( SetupData &setupData )
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
  m_ResID = EFFECT_RESOURCE_SKY_SNOW;
  m_EmitterLength = 0;
}

/**
 *  @brief  破棄
 */
bool EffectSnow::Terminate( void )
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
void EffectSnow::Delete( void )
{
  if(m_ResID == EFFECT_RESOURCE_SKY_SNOWSTORM)
  {
    // レンダリング対象からはずす
    m_pPipeLine->RemoveWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_SNOWSTORM].GetModelInstanceNode());
  }
}

/**
 *  @brief エミッターセットの削除
 */
void EffectSnow::KillEmitter(void)
{
  gfl2::Effect::EmitterSet* set = static_cast<gfl2::Effect::EmitterSet*>(m_pEffectHandle->GetEmitterSet());
  if( set )
  {
    m_pEffectSystem->KillEmitterSet(set);
  }
}

/**
 *  @brief エミッターセットの生成
 *  @param id エフェクトＩＤ
 *  @param pos 座標
 */
void EffectSnow::CreateEmitter(Effect::EffectResourceID id, const gfl2::math::Vector3 &pos)
{
  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, id );
  m_ResID = id;
}

/**
 *  @brief エフェクト　空用　作成
 *  @aram id エフェクトＩＤ
 *  @param pos 座標
 *  @param dir 方向
 *  @param rot　回転
 *  @param emitter_length　距離
 */
void EffectSnow::CreateSkyEffect( Effect::EffectResourceID id, const gfl2::math::Vector3 &pos, float emitter_length )
{
  CreateEmitter(id, pos);
  m_EmitterLength = emitter_length;
  CreateBillboard();
}

/**
 *  @brief ダイヤモンドダスト地面跳ね返り
 *  @param 座標
 */
void EffectSnow::CreateGroundDiamondDust( const gfl2::math::Vector3 &pos )
{
  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_GROUND_DIAMONDDUST );

  m_ResID = EFFECT_RESOURCE_GROUND_DIAMONDDUST;
}

/**
 *  @brief 雪モデルを取得
 *  @return 雪モデルへのポインタ
 */
poke_3d::model::BaseModel* EffectSnow::GetSnowModel(void)
{
  return &(m_Model[BILLBOARD_TYPE_SNOWSTORM]);
}
  
/**
 *  @brief ビルボードの作成
 */
void EffectSnow::CreateBillboard(void)
{
  {
    gfl2::math::Vector3 snow_pos(0,0,-SNOW_BILLBOARD_LENGTH);
    int index = BL_IDX_SNOW_EF_WEATHER_SNOW_BILL_CTR_GFBMDLP;
    int anm_index = BL_IDX_SNOW_EF_WEATHER_SNOW_BILL_GFBMOT;
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;
    binLinkerAccessor.Initialize( m_pResource );
    // モデルを作る
    void *pModelData = binLinkerAccessor.GetData( index  );
    createModel( BILLBOARD_TYPE_SNOWSTORM, pModelData );
    m_Model[BILLBOARD_TYPE_SNOWSTORM].Create( m_pAllocator, m_pEffectHeap->GetGFHeapBase(), m_pModelInstanceNode[BILLBOARD_TYPE_SNOWSTORM] );
    // レンダリング対象に追加
    m_pPipeLine->AddWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_SNOWSTORM].GetModelInstanceNode());
    // モーションを作る
    void *pMotionData = binLinkerAccessor.GetData( anm_index );
    createMotion( BILLBOARD_TYPE_SNOWSTORM, pMotionData );
    m_Model[BILLBOARD_TYPE_SNOWSTORM].ChangeAnimationByResourceNode( m_pMotionResourceRootNode[BILLBOARD_TYPE_SNOWSTORM] );
    m_Model[BILLBOARD_TYPE_SNOWSTORM].SetAnimationStepFrame(1.0f);
    m_Model[BILLBOARD_TYPE_SNOWSTORM].SetAnimationLoop(true);
    // 描画登録
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode[BILLBOARD_TYPE_SNOWSTORM] );
    // 座標を指定
    m_Model[BILLBOARD_TYPE_SNOWSTORM].SetPosition( snow_pos );
    m_Model[BILLBOARD_TYPE_SNOWSTORM].SetVisible(false);
  }

}

/**
 *  @brief  座標更新
 */
void EffectSnow::UpdatePosition( const gfl2::math::Vector3 &pos, const gfl2::math::Vector3 &dir)
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
bool EffectSnow::IsDead(void)
{
  // 再生終了しているなら削除
  if( !m_pEffectHandle->GetEmitterSet() ){ return true; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {  
    return true;
  }
  return false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectSnow::updateNormal( void )
{
  if (m_ResID == EFFECT_RESOURCE_SKY_SNOWSTORM)
  {
    m_Model[BILLBOARD_TYPE_SNOWSTORM].UpdateAnimation();
  }
}

/**
 *  @brief  モデルの作成
 */
void EffectSnow::createModel( int index, void *pModelBinary )
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
void EffectSnow::createMotion( int index, void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode[index] =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode[index] );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
