//======================================================================
/**
 * @file FieldEffectPokeShadow.cpp
 * @date 2015/09/09 16:45:14
 * @author miyachi_soichi
 * @brief エフェクト：バトルモデル用影
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
// PokeTool
#include "PokeTool/include/PokeModel.h"
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
// TrialModel
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectPokeShadow.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 * @class MyClass
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
 EffectPokeShadow::EffectPokeShadow()
: IEffectBase()
, m_pModel( NULL )
, m_pAllocator( NULL )
, m_pModelResourceRootNode( NULL )
, m_pModelInstanceNode( NULL )
, m_pFieldRootNode( NULL )
, m_pRenderingPipeLine( NULL )
, m_pParent( NULL )
, m_pJointNode( NULL )
, m_pCollisionScene( NULL )
, m_bRegist( false )
{
}

/**
 *  @brief  デストラクタ
 */
EffectPokeShadow::~EffectPokeShadow( void )
{
}

/**
 *  @brief  初期化
 */
void EffectPokeShadow::Initialize( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;
  m_pParent = setupData.pTrialModel;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData( 0 );
  createModel( pModelData );
  m_pModel = GFL_NEW( setupData.pHeap ) poke_3d::model::BaseModel();
  m_pModel->Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  m_bRegist = true;
  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
  m_pModel->SetVisible( false );
}

/**
 *  @brief  破棄
 */
bool EffectPokeShadow::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_pModel->Destroy();
  GFL_SAFE_DELETE( m_pModel );

  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );

  GFL_SAFE_DELETE( m_pAllocator );
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectPokeShadow::Delete( void )
{
  m_pModel->SetVisible( false );
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  モデルの作成
 */
void EffectPokeShadow::Joint( void )
{
  s32 joint_index = -1;
  poke_3d::model::BaseModel *pBaseModel = m_pParent->GetPokeModel();
  joint_index = m_pParent->GetPokeModel()->GetJointIndex( "Origin" );

  if( joint_index == -1 )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
    u32 jointCount = pNode->GetJointNum();
    for( u32 i = 0; i < jointCount; ++ i )
    {
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode *pJoint = pNode->GetJointInstanceNode( i );

      GFL_PRINT( "joint_name %s\n", pJoint->GetName() );
    }

    GFL_ASSERT_MSG( false, "joint_name Loc_Head not found" );
  }
  else
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
    m_pJointNode = pNode->GetJointInstanceNode( joint_index );
  }
}

/**
 *  @brief  描画設定
 */
void EffectPokeShadow::SetVisible( bool flag )
{
  // 準備ができていない
  if( !m_pModel ){ return; }

  if( flag && !m_bRegist )
  {
    m_bRegist = true;
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
  }
  else if( !flag && m_bRegist )
  {
    m_bRegist = false;
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
  }

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新
 */
void EffectPokeShadow::updateNormal( void )
{
  // 表示状態の更新
  if( m_pParent->IsEnable() && m_pModel )
  {
    if( m_pParent->IsDisp() && !m_pModel->IsVisible() )
    {
      m_pModel->SetVisible( true );
    }
    else if( !m_pParent->IsDisp() && m_pModel->IsVisible() )
    {
      m_pModel->SetVisible( false );
    }
  }

  // 座標更新
  updatePosition();
}

/**
 *  @brief  モデルの作成
 */
void EffectPokeShadow::createModel( void *pModelBinary )
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
 *  @brief  座標更新
 */
void EffectPokeShadow::updatePosition( void )
{
  if( m_pParent && m_pModel && m_pParent->IsEnable() )
  {
    if( m_pJointNode )
    {
      gfl2::math::Vector3 vPos = m_pJointNode->GetWorldMatrix().GetElemPosition();

      if( m_pCollisionScene )
      {
        // 地面の高さを取得
        f32 startOffset = 100.0f;
        f32 endOffset = 10000.0f;
        RaycastCustomCallback::HIT_DATA hitData;
        gfl2::math::Vector3             startVec3(vPos + gfl2::math::Vector3(0.0f,startOffset,0.0f));
        gfl2::math::Vector3             endVec3  (vPos - gfl2::math::Vector3(0.0f,endOffset,0.0f));
        gfl2::math::Vector4              startVec (startVec3.x,startVec3.y,startVec3.z,0.0f);
        gfl2::math::Vector4              endVec   (  endVec3.x,  endVec3.y,  endVec3.z,0.0f);

        // レイが地面に当たったかどうか
        if( m_pCollisionScene->RaycastFromStaticActorsMesh( startVec,endVec,&hitData ) )
        {
          // 地面から少し浮かせる
          vPos.y = hitData.intersection.y + 3.0f;
        }
      }

      m_pModel->SetPosition( vPos );
    }
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
