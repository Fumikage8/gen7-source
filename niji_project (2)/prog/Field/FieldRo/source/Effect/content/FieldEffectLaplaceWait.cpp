//=================================================================================================
/**
*  @file   FieldEffectLaplaceWait.cpp
*  @brief  ラプラスの待機エフェクト
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================

// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <math/include/gfl2_Float.h>
#include <system/include/Date/gfl2_Date.h>  // 時間を使用するアニメーション制御に使用

// self
#include "Field/FieldRo/include/Effect/content/FieldEffectLaplaceWait.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const u32 DESIGN_ANIMETION_SLOT  = 0;

/**
 *  @class  EffectLaplaceWait
 *  @brief  ラプラスの高速移動エフェクト
 */
/**
 *  @brief  コンストラクタ
 */
EffectLaplaceWait::EffectLaplaceWait( void )
: IEffectBase()
, m_bVisible( false )
{
}

/**
 *  @brief  デストラクタ
 */
EffectLaplaceWait::~EffectLaplaceWait( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectLaplaceWait::Initialize( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;

  {
    Field::MoveModel::FieldMoveModel *pMoveModel = setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>();
    m_pParent = pMoveModel->GetCharaDrawInstance();
  }

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
  createMotion( setupData.pResource );
  m_pModel->ChangeAnimationByResourceNode( m_pMotionResource_flow , DESIGN_ANIMETION_SLOT  );
  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
}

/**
 *  @brief  破棄
 */
bool EffectLaplaceWait::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  GFL_SAFE_DELETE( m_pModel );
  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResource_flow );
  GFL_SAFE_DELETE( m_pMotionResource_flow );
  GFL_SAFE_DELETE( m_pAllocator );

  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectLaplaceWait::Delete( void )
{
  SetVisible( false );
}

/**
 *  @brief  ラッシュ切り替え
 */
void EffectLaplaceWait::SetVisible( bool flag )
{
  if( m_bVisible == flag )
  {
    return;
  }

  m_bVisible = flag;
  if( m_bVisible )
  {
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
  }
  else
  {
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  モデルの生成
 */
void EffectLaplaceWait::createModel( void *pModelBinary )
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
 *  @brief  モーションの生成
 */
void EffectLaplaceWait::createMotion( void *pResource )
{
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( pResource );

  {
    void *pMotionBinary_flow = binLinkerAccessor.GetData( 1 );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( static_cast<c8*>( pMotionBinary_flow ));
    m_pMotionResource_flow =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
    m_pFieldRootNode->AddResourceChild( m_pMotionResource_flow );
  }
}

/**
 *  @brief  更新処理
 */
void EffectLaplaceWait::updateNormal( void )
{
  // 座標更新
  updatePosition();
  m_pModel->UpdateAnimation();
}

/**
 *  @brief  座標更新
 */
void EffectLaplaceWait::updatePosition( void )
{
  gfl2::math::Vector3 vPos = m_pParent->GetPosition();
  m_pModel->SetPosition( vPos );
  gfl2::math::Quaternion quat = m_pParent->GetRotationQuat();
  m_pModel->SetRotationQuat( quat );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
