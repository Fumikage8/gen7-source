//======================================================================
/**
 * @file FieldEffectSandStorm.cpp
 * @date 2015/12/09 18:07:17
 * @author saito_nozomu
 * @brief 砂嵐
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
#include "Field/FieldRo/include/Effect/content/FieldEffectSandStorm.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

//conv_header
#include "niji_conv_header/field/effect/sandstorm.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const float SANDSTORM_BILLBOARD_LENGTH = 34;

/**
 *  @class  EffectSandStorm
 *  @brief  砂嵐
 */
/**
 *  @brief  コンストラクタ
 */
EffectSandStorm::EffectSandStorm( void )
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
}

/**
 *  @brief  デストラクタ
 */
EffectSandStorm::~EffectSandStorm( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectSandStorm::Initialize( SetupData &setupData )
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
}

/**
 *  @brief  破棄
 */
bool EffectSandStorm::Terminate( void )
{
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
void EffectSandStorm::Delete( void )
{
  // レンダリング対象からはずす
  m_pPipeLine->RemoveWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_SANDSTORM].GetModelInstanceNode());
}

/**
 *  @brief エフェクト　空用　作成
 */
void EffectSandStorm::CreateEffect( void )
{
  CreateBillboard();
}

/**
 *  @brief 雪モデルを取得
 *  @return 雪モデルへのポインタ
 */
poke_3d::model::BaseModel* EffectSandStorm::GetSandStormModel(void)
{
  return &(m_Model[BILLBOARD_TYPE_SANDSTORM]);
}
  
/**
 *  @brief ビルボードの作成
 */
void EffectSandStorm::CreateBillboard( void )
{
  {
    gfl2::math::Vector3 sand_pos(0,0,-SANDSTORM_BILLBOARD_LENGTH);
    int index = BL_IDX_SANDSTORM_EF_WEATHER_SANDSTORM_BILL_CTR_GFBMDLP;
    int anm_index = BL_IDX_SANDSTORM_EF_WEATHER_SANDSTORM_BILL_GFBMOT;
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;
    binLinkerAccessor.Initialize( m_pResource );
    // モデルを作る
    void *pModelData = binLinkerAccessor.GetData( index  );
    createModel( BILLBOARD_TYPE_SANDSTORM, pModelData );
    m_Model[BILLBOARD_TYPE_SANDSTORM].Create( m_pAllocator, m_pEffectHeap->GetGFHeapBase(), m_pModelInstanceNode[BILLBOARD_TYPE_SANDSTORM] );
    // レンダリング対象に追加
    m_pPipeLine->AddWeatherBoardRenderingTarget(m_Model[BILLBOARD_TYPE_SANDSTORM].GetModelInstanceNode());
    // モーションを作る
    void *pMotionData = binLinkerAccessor.GetData( anm_index );
    createMotion( BILLBOARD_TYPE_SANDSTORM, pMotionData );
    m_Model[BILLBOARD_TYPE_SANDSTORM].ChangeAnimationByResourceNode( m_pMotionResourceRootNode[BILLBOARD_TYPE_SANDSTORM] );
    m_Model[BILLBOARD_TYPE_SANDSTORM].SetAnimationStepFrame(1.0f);
    m_Model[BILLBOARD_TYPE_SANDSTORM].SetAnimationLoop(true);
    // 描画登録
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode[BILLBOARD_TYPE_SANDSTORM] );
    // 座標を指定
    m_Model[BILLBOARD_TYPE_SANDSTORM].SetPosition( sand_pos );
    m_Model[BILLBOARD_TYPE_SANDSTORM].SetVisible(false);
  }

}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectSandStorm::updateNormal( void )
{
  m_Model[BILLBOARD_TYPE_SANDSTORM].UpdateAnimation();
}

/**
 *  @brief  モデルの作成
 */
void EffectSandStorm::createModel( int index, void *pModelBinary )
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
void EffectSandStorm::createMotion( int index, void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode[index] =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode[index] );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
