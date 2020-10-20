/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectDummyPolygon.cpp
 *  @brief  フィールドエフェクト：キャプチャー用ダミーポリゴン
 *  @author saita_kazuki
 *  @date   2015.09.11
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
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectDummyPolygon.h"
// sound
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectDummyPolygon::EffectDummyPolygon( void )
  :IEffectBase()
  ,m_Model()
  ,m_pAllocator( NULL )
  ,m_pModelResourceRootNode( NULL )
  ,m_pModelInstanceNode( NULL )
  ,m_pFieldRootNode( NULL )
  ,m_pRenderingPipeLine( NULL )
  ,m_pCameraManager( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectDummyPolygon::~EffectDummyPolygon( void )
{
}

/**
 *  @brief  初期化
 */
void EffectDummyPolygon::Initialize( SetupData &setupData )
{
  setup( setupData );
}

/**
 *  @brief  破棄
 */
bool EffectDummyPolygon::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_Model.Destroy();

  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );

  GFL_SAFE_DELETE( m_pAllocator );
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectDummyPolygon::Delete( void )
{
  m_Model.SetVisible( false );
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  初期化
 */
void EffectDummyPolygon::setup( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pCameraManager = setupData.pCameraManager;
  m_pRenderingPipeLine = setupData.pPipeLine;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );

  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData( 0 );
  createModel( pModelData );
  m_Model.Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  m_Model.SetPosition( setupData.vPos);

  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  更新
 */
void EffectDummyPolygon::updateNormal( void )
{
}

/**
 *  @brief  モデルの作成
 */
void EffectDummyPolygon::createModel( void *pModelBinary )
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelBinary ) );
  m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode );

  // モデルインスタンス作成
  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode );
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
