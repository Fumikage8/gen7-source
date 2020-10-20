/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectExclamation2.cpp
 *  @brief  フィールドエフェクト：エクスクラメーション２
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */
// gfl2
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>
// sound
#include "Sound/include/Sound.h"
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation2.h"
#include "niji_conv_header/field/effect/exclamation_icon2.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectExclamation2::EffectExclamation2( void )
  :EffectBallonIcon()
  ,m_pMotionResourceRootNode_Open( NULL )
  ,m_pMotionResourceRootNode_Close( NULL )
  ,m_StepFrame( 1.0f )
{
  m_nSeID = SEQ_SE_FLD_277;
  m_eState = STATE_NONE;
  m_eMode = MODE_NORMAL;
  m_bWait = true;
}

/**
 *  @brief  デストラクタ
 */
EffectExclamation2::~EffectExclamation2( void )
{
}

/**
 *  @brief  初期化
 */
void EffectExclamation2::Initialize( EffectBallonIcon::SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pCameraManager = setupData.pCameraManager;
  m_pRenderingPipeLine = setupData.pPipeLine;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData( GetModelDataID() );
  createModel( pModelData );
  m_Model.Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( GetMotionDataID() );
  void *pOpen = binLinkerAccessor.GetData( BL_IDX_EXCLAMATION_ICON2_MAP_OPEN_GFBMOT );
  void *pClose = binLinkerAccessor.GetData( BL_IDX_EXCLAMATION_ICON2_MAP_CLOSE_GFBMOT );
  createMotion( pMotionData );
  createMotion2( pOpen, pClose );
  // ジョイント
  jointParent( setupData.pParentObject );
  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
  // Se
  if( IsPlaySe() )
  {
    Sound::PlaySE( m_nSeID );
  }
}

/**
 *  @brief  破棄
 */
bool EffectExclamation2::Terminate( void )
{
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode_Open );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode_Open );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode_Close );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode_Close );

  return EffectBallonIcon::Terminate();
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectExclamation2::Delete( void )
{
  EffectBallonIcon::Delete();
}

/**
*  @brief  Openリクエスト
*
*  @param  anime アニメーション再生を行うならtrue
*/
void EffectExclamation2::RequestOpenForKeepMode( b32 anime )
{
  if( m_eMode != MODE_KEEP )
  {
    return;
  }

  m_eState = STATE_OPEN;
  m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode_Open );
  m_Model.SetAnimationStepFrame( m_StepFrame );

  if( anime )
  {
    m_Model.SetAnimationFrame( 0.0f );
  }
  else
  {
    m_Model.SetAnimationFrame( m_Model.GetAnimationEndFrame() );
  }
}

/**
*  @brief  Closeリクエスト
*
*  @param  anime アニメーション再生を行うならtrue
*/
void EffectExclamation2::RequestCloseForKeepMode( b32 anime )
{
  if( m_eMode != MODE_KEEP )
  {
    return;
  }

  m_eState = STATE_END;
  m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode_Close );
  m_Model.SetAnimationStepFrame( m_StepFrame );

  if( anime )
  {
    m_Model.SetAnimationFrame( 0.0f );
  }
  else
  {
    m_Model.SetAnimationFrame( m_Model.GetAnimationEndFrame() );
  }
}

// リソースID取得
u32 EffectExclamation2::GetModelDataID( void )
{
  return BL_IDX_EXCLAMATION_ICON2_MAP_CTR_GFBMDLP;
}
u32 EffectExclamation2::GetMotionDataID( void )
{
  return BL_IDX_EXCLAMATION_ICON2_MAP_GFBMOT;
}

// 再生速度変更
void EffectExclamation2::SetStepFrame( f32 stepFrame )
{
  m_StepFrame = stepFrame;
  m_Model.SetAnimationStepFrame( stepFrame );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新
 */
void EffectExclamation2::updateNormal( void )
{
  switch( m_eState )
  {
  case STATE_NONE:
    if( m_eMode == MODE_NORMAL )
    {
      m_eState = STATE_END;
      m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode );
      m_Model.SetAnimationStepFrame( m_StepFrame );
    }
    else
    {
      m_eState = STATE_OPEN;
      m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode_Open );
      m_Model.SetAnimationStepFrame( m_StepFrame );
    }
    break;
  case STATE_OPEN:
    if( !m_bWait )
    {
      m_eState = STATE_END;
      m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode_Close );
      m_Model.SetAnimationStepFrame( m_StepFrame );
    }
    break;
  }

  EffectBallonIcon::updateNormal();
}

/**
 *  @brief  モーション作成
 */
void EffectExclamation2::createMotion2( void *pOpen, void *pClose )
{
  // Open
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( static_cast<c8*>( pOpen ));
    m_pMotionResourceRootNode_Open =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
    m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode_Open );
  }
  // Close
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( static_cast<c8*>( pClose ));
    m_pMotionResourceRootNode_Close =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
    m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode_Close );
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
