/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectLensFlare.cpp
 *  @brief  フィールドエフェクト：レンズフレア
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectLensFlare.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectLensFlare::EffectLensFlare( void )
  :IEffectBase()
  ,m_Model()
  ,m_pAllocator( NULL )
  ,m_pModelResourceRootNode( NULL )
  ,m_pModelInstanceNode( NULL )
  ,m_pFieldRootNode( NULL )
  ,m_pRenderingPipeLine( NULL )
  ,m_nLife(0)
{
}

/**
 *  @brief  デストラクタ
 */
EffectLensFlare::~EffectLensFlare( void )
{
}

/**
 *  @brief  初期化
 */
void EffectLensFlare::Initialize( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData( 0 );
  createModel( pModelData );
  m_Model.Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( 1 );
  createMotion( pMotionData );
  m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode );

  gfl2::math::SRT srt(
    gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ),
    setupData.quaternion,
    setupData.vPos
    );
  m_pModelInstanceNode->SetLocalSRT( srt );
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  破棄
 */
bool EffectLensFlare::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_Model.Destroy();

  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode );

  GFL_SAFE_DELETE( m_pAllocator );
  return true;
}

/**
 *  @brief  更新
 */
void EffectLensFlare::updateNormal( void )
{
  m_Model.UpdateAnimation();
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectLensFlare::Delete( void )
{
  m_Model.SetVisible( false );
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  アニメーション終了検知
 */
bool EffectLensFlare::IsAnimationLastFrame( void )
{
  return GFL_BOOL_CAST(m_Model.IsAnimationLastFrame());
}

/**
 *  @brief  モデルの作成
 */
void EffectLensFlare::createModel( void *pModelBinary )
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
void EffectLensFlare::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
