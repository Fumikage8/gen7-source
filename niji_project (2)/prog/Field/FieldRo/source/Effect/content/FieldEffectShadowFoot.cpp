//=================================================================================================
/**
*  @file   FieldEffectShadow.cpp
*  @brief  影(足型)の定義
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
#include "GameSys/include/GameManager.h"
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectShadowFoot.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/FieldEnvManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectShadowFoot
 *  @brief  影(足型)
 */

/**
 *  @brief  コンストラクタ
 */
EffectShadowFoot::EffectShadowFoot( void )
: IEffectBase()
, m_pEffectHeap           ( NULL )
, m_pEffectSystem         ( NULL )
, m_bVisible              ( false )
, m_pAllocator            ( NULL )
, m_pModel                ( NULL )
, m_pModelResourceRootNode( NULL )
, m_pModelInstanceNode    ( NULL )
, m_pFieldRootNode        ( NULL )
, m_pRenderingPipeLine    ( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectShadowFoot::~EffectShadowFoot( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectShadowFoot::Initialize( SetupData &setupData )
{
  m_pEffectHeap         = setupData.pEffectHeap;
  m_pEffectSystem       = setupData.pEffectSystem;
  m_pFieldRootNode      = setupData.pRootNode;
  m_pRenderingPipeLine  = setupData.pPipeLine;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );

  // モデルリソース作成
  void *pModelData = binLinkerAccessor.GetData( 0 );
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelData ) );
  m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode );

  // モデルインスタンス作成
  m_pModel = GFL_NEW( setupData.pHeap ) poke_3d::model::BaseModel();
  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode );
  m_pModel->Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );

  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );

  // 常に描画
  SetVisibleShadow(true);
}

/**
 *  @brief  破棄
 */
bool EffectShadowFoot::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode ); 
  GFL_SAFE_DELETE( m_pModel );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pAllocator );

  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap   = NULL;
  }
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectShadowFoot::Delete( void )
{
  SetVisibleShadow( false );
}

/**
 *  @brief  影切り替え
 */
void EffectShadowFoot::SetVisibleShadow( bool flag )
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
 *  @brief  更新処理
 */
void EffectShadowFoot::updateNormal( void )
{
  m_pModel->SetScale       ( gfl2::math::Vector3(1.0f,1.0f,1.0f) );
  m_pModel->SetPosition    ( gfl2::math::Vector3(0.0f,0.0f,0.0f) );
  m_pModel->SetRotationQuat( gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f) );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
