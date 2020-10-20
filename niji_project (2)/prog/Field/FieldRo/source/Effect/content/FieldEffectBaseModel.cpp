//======================================================================
/**
 * @file FieldEffectBaseModel.cpp
 * @date 2016/03/02 13:08:27
 * @author miyachi_soichi
 * @brief エフェクト：モデル単体呼出
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
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectBaseModel.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

// スクリーン対応用
static const f32 SCREEN_BILLBOARD_LENGTH = -34.0f;

/**
 *  @brief  コンストラクタ
 */
EffectBaseModel::EffectBaseModel( void )
: IEffectBase()
, m_pModel( NULL )
, m_pAllocator( NULL )
, m_pModelResourceRootNode( NULL )
, m_pModelInstanceNode( NULL )
, m_pFieldRootNode( NULL )
, m_pRenderingPipeLine( NULL )
, m_pCameraManager( NULL )
, m_vPosition()
, m_bSuicide( false )
, m_bBillboard( false )
, m_bScreen( false )
, m_bLoop( false )
{
}

/**
 *  @brief  デストラクタ
 */
EffectBaseModel::~EffectBaseModel( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectBaseModel::Initialize( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;
  m_pCameraManager = setupData.pCameraManager;
  m_vPosition = setupData.position;
  m_vScale = setupData.scale;
  m_bSuicide = setupData.bSuicide;
  m_bBillboard = setupData.bBillboard;
  m_bScreen = setupData.bScreen;
  m_bLoop = setupData.bLoop;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  m_pModel = GFL_NEW( setupData.pHeap ) poke_3d::model::BaseModel();
  void *pModelData = binLinkerAccessor.GetData( setupData.nModelID );
  createModel( pModelData );
  m_pModel->Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( setupData.nMotionID );
  createMotion( pMotionData );
  m_pModel->ChangeAnimationByResourceNode( m_pMotionResourceRootNode );

  if( !m_bScreen )
  {
    // 通常シーンへの描画
    m_pModel->SetPosition( m_vPosition );
    m_pModel->SetScale( m_vScale );
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
  }
  else
  {
    // スクリーンへの描画
    m_vPosition.z = SCREEN_BILLBOARD_LENGTH;
    m_pModel->SetPosition( m_vPosition );
    m_pModel->SetScale( m_vScale );
    m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
    m_pRenderingPipeLine->AddWeatherBoardRenderingTarget( m_pModelInstanceNode );
  }
}

/**
 *  @brief  破棄
 */
bool EffectBaseModel::Terminate( void )
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
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectBaseModel::Delete( void )
{
  m_pModel->SetVisible( false );
  if( !m_bScreen )
  {
    // 通常シーンへの描画
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
  }
  else
  {
    // スクリーンへの描画
    m_pRenderingPipeLine->RemoveWeatherBoardRenderingTarget( m_pModelInstanceNode );
  }
}

/**
 *  @brief  終了しているか.
 */
bool EffectBaseModel::IsAnimationLastFrame( void )
{
  if( !m_pModel ){ return true; }
  if( m_bLoop ){ return false; }

  return GFL_BOOL_CAST(m_pModel->IsAnimationLastFrame());
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectBaseModel::updateNormal( void )
{
  if( m_bSuicide )
  {
    if( IsAnimationLastFrame() )
    {
      // 再生終了したので自殺する
      RequestDelete();
    }
  }
  if( m_pModel )
  {
    m_pModel->UpdateAnimation();
  }
  if( m_bBillboard )
  {
    // Viewがほしい
    if( m_pCameraManager )
    {
      gfl2::math::Matrix34 mtx_view;
      m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
      m_pModelInstanceNode->SetBillboardRotation( mtx_view, gfl2::renderingengine::BillboardType::ScreenXYZ );
    }
  }
}

/**
 *  @brief  モデルの作成
 */
void EffectBaseModel::createModel( void *pModelBinary )
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
void EffectBaseModel::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
