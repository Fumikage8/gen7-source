//======================================================================
/**
 * @file FieldEffectFootRideMurando.cpp
 * @date 2015/11/13 14:50:16
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideMurando.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectFootRideMurando::EffectFootRideMurando( void )
  :IEffectBase()
  ,m_pModel( NULL )
  ,m_pAllocator( NULL )
  ,m_pModelResourceRootNode( NULL )
  ,m_pModelInstanceNode( NULL )
  ,m_pFieldRootNode( NULL )
  ,m_pRenderingPipeLine( NULL )
  ,m_SRT()
{
}

/**
 *  @brief  デストラクタ
 */
EffectFootRideMurando::~EffectFootRideMurando( void )
{
}

/**
 *  @brief  初期化
 */
void EffectFootRideMurando::Initialize( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  m_pModel = GFL_NEW( setupData.pHeap ) poke_3d::model::BaseModel();
  void *pModelData = binLinkerAccessor.GetData( 0 );
  createModel( pModelData );
  m_pModel->Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( 1 );
  createMotion( pMotionData );
  m_pModel->ChangeAnimationByResourceNode( m_pMotionResourceRootNode );

  Field::IField3DObjectNode *pParent = setupData.pParentObject;
  if( pParent->SafeCast<Field::MoveModel::FieldMoveModel>() != NULL )
  {
    Field::MoveModel::FieldMoveModel *pMoveModel = pParent->SafeCast<Field::MoveModel::FieldMoveModel>();
    poke_3d::model::BaseModel *pBaseModel = pMoveModel->GetCharaDrawInstance();
    if( pBaseModel )
    {
      m_SRT = gfl2::math::SRT(
        gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ),
        pBaseModel->GetRotationQuat(),
        pBaseModel->GetPosition()
        );
    }
    else
    {
      m_SRT.scale = gfl2::math::Vector3::GetZero();
    }
  }
  else if( pParent->SafeCast<Field::StaticModel::StaticModel>() != NULL )
  {
    Field::StaticModel::StaticModel *pStaticModel = pParent->SafeCast<Field::StaticModel::StaticModel>();
    u32 modelType = pStaticModel->GetNowModelType();
    poke_3d::model::BaseModel *pBaseModel = pStaticModel->GetBaseModel( modelType );
    if( pBaseModel )
    {
      m_SRT = gfl2::math::SRT(
        gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ),
        pBaseModel->GetRotationQuat(),
        pBaseModel->GetPosition()
        );
    }
    else
    {
      m_SRT.scale = gfl2::math::Vector3::GetZero();
    }
  }

  m_pModelInstanceNode->SetLocalSRT( m_SRT );
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  破棄
 */
bool EffectFootRideMurando::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_pModel->Destroy();

  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pModel );

  GFL_SAFE_DELETE( m_pAllocator );
  return true;
}

/**
 *  @brief  更新
 */
void EffectFootRideMurando::updateNormal( void )
{
  if( m_pModel->IsAnimationLastFrame() )
  {
    RequestDelete();
  }
  else
  {
    m_pModel->UpdateAnimation();
  }
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectFootRideMurando::Delete( void )
{
  m_pModel->SetVisible( false );
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  モデルの作成
 */
void EffectFootRideMurando::createModel( void *pModelBinary )
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelBinary ) );
  m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode );

  // モデルインスタンス作成
  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode );
}

/**
 *  @brief  モーションの作成
 */
void EffectFootRideMurando::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}

/**
 *  @brief  オフセットを設定
 */
void EffectFootRideMurando::SetOffset( b32 isReverse , gfl2::math::Vector3 offset )
{
  gfl2::math::Quaternion qua(0.0f,0.0f,0.0f,1.0f);
  if( isReverse ){ qua.RadianYXZToQuaternion( 0.0f,0.0f,PI ); }

  m_SRT.rotation *= qua;
  m_SRT.translate += offset;
  m_pModelInstanceNode->SetLocalSRT( m_SRT );
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
