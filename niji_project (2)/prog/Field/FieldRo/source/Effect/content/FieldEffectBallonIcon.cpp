/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectBallonIcon.cpp
 *  @brief  フィールドエフェクト：吹き出しアイコン基礎
 *  @author miyachi_soichi
 *  @date   2015.06.09
 */

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

#include "PokeTool/include/PokeModel.h"

#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"
// sound
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectBallonIcon::EffectBallonIcon( void )
  :IEffectBase()
  ,m_Model()
  ,m_pAllocator( NULL )
  ,m_pModelResourceRootNode( NULL )
  ,m_pModelInstanceNode( NULL )
  ,m_pFieldRootNode( NULL )
  ,m_pRenderingPipeLine( NULL )
  ,m_pJointNode( NULL )
  ,m_pCameraManager( NULL )
  ,m_Offset( 0.0f, 0.0f, 0.0f )
{
}

/**
 *  @brief  デストラクタ
 */
EffectBallonIcon::~EffectBallonIcon( void )
{
}

/**
 *  @brief  初期化
 */
void EffectBallonIcon::Initialize( SetupData &setupData )
{
  setupBallon( setupData );
}

/**
 *  @brief  破棄
 */
bool EffectBallonIcon::Terminate( void )
{
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_Model.Destroy();

  if( m_pModelResourceRootNode )
  {
    m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
    GFL_SAFE_DELETE( m_pModelResourceRootNode );
  }
  if( m_pMotionResourceRootNode )
  {
    m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode );
    GFL_SAFE_DELETE( m_pMotionResourceRootNode );
  }

  GFL_SAFE_DELETE( m_pAllocator );
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectBallonIcon::Delete( void )
{
  m_Model.SetVisible( false );
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
}

/**
 *  @brief  アニメーション終了検知
 */
bool EffectBallonIcon::IsAnimationLastFrame( void )
{
  return GFL_BOOL_CAST(m_Model.IsAnimationLastFrame());
}

/**
*  @brief  アニメーション再生速度を設定
*
*  @param  stepFrame 再生速度
*/
void EffectBallonIcon::SetAnimationStepFrame( f32 stepFrame )
{
  m_Model.SetAnimationStepFrame( stepFrame );
}

/**
*  @brief  拡大率を指定
*
*  @param  拡大率
*/
void EffectBallonIcon::SetScale( f32 scale )
{
  m_Model.SetScale( scale, scale, scale );
}

/**
*  @brief  親となるジョイントを設定(頭以外につけたい時に使用)
*
*  @param  pJointNode ジョイント(ポインタを保持する)
*/
void EffectBallonIcon::SetParentJoint( gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode )
{
  m_pJointNode = pJointNode;
  updatePosition();
}

/**
*  @brief  表示オフセットを設定
*
*  @param  rOffset　表示オフセット
*/
void EffectBallonIcon::SetOffest( const gfl2::math::Vector3& rOffset )
{
  m_Offset = rOffset;
}

/**
 *  @brief  初期化
 */
void EffectBallonIcon::setupBallon( SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;
  m_pCameraManager = setupData.pCameraManager;
  m_pRenderingPipeLine = setupData.pPipeLine;
  m_bPosMode = setupData.bPosMode;
  m_vPosition = setupData.vPosition;

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
  createMotion( pMotionData );
  m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode );
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
 *  @brief  更新
 */
void EffectBallonIcon::updateNormal( void )
{
  // 座標更新
  updatePosition();
  m_Model.UpdateAnimation();
}

/**
 *  @brief  モデルの作成
 */
void EffectBallonIcon::createModel( void *pModelBinary )
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
void EffectBallonIcon::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}

/**
 *  @brief  親にジョイント
 */
void EffectBallonIcon::jointParent( Field::IField3DObjectNode *pParent )
{
  if( m_bPosMode )
  {
    // 座標にジョイント
    updatePosition();
    return;
  }

  // nodeにジョイント
    s32 joint_index = -1;
  poke_3d::model::BaseModel *pBaseModel = NULL;
  if( pParent->SafeCast<Field::TrialModel::FieldTrialModel>() != NULL )
  {
    Field::TrialModel::FieldTrialModel *pTrialModel = pParent->SafeCast<Field::TrialModel::FieldTrialModel>();
    pBaseModel = pTrialModel->GetPokeModel();
    joint_index = pBaseModel->GetJointIndex( "Head" );
  }
  else if( pParent->SafeCast<Field::MoveModel::FieldMoveModel>() != NULL )
  {
    Field::MoveModel::FieldMoveModel *pMoveModel = pParent->SafeCast<Field::MoveModel::FieldMoveModel>();
    pBaseModel = pMoveModel->GetCharaDrawInstance();
    joint_index = pBaseModel->GetJointIndex( "Loc_Head" );
  }
  else if( pParent->SafeCast<Field::StaticModel::StaticModel>() != NULL )
  {
    Field::StaticModel::StaticModel *pStaticModel = pParent->SafeCast<Field::StaticModel::StaticModel>();
    pBaseModel = pStaticModel->GetBaseModel( StaticModel::MODEL_TYPE_HIGH );
    joint_index = pBaseModel->GetJointIndex( "Loc_Head" );
  }

  // ジョイントすべきノードが見つからない
  if( !pBaseModel )
  {
    GFL_ASSERT_MSG( false, "BaseModel is not found\n" );
  }
  else if( joint_index == -1 )
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
    updatePosition();
  }
}

/**
 *  @brief  座標更新
 */
void EffectBallonIcon::updatePosition( void )
{
  if( !m_bPosMode )
  {
    if( m_pJointNode )
    {
      gfl2::math::Vector3 vPos = m_pJointNode->GetWorldMatrix().GetElemPosition();
      m_Model.SetPosition( vPos + m_Offset );

      // Viewがほしい
      if( m_pCameraManager )
      {
        gfl2::math::Matrix34 mtx_view;
        m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
        m_pModelInstanceNode->SetBillboardRotation( mtx_view, gfl2::renderingengine::BillboardType::ScreenXYZ );
      }
    }
  }
  else
  {
    m_Model.SetPosition( m_vPosition );
    // Viewがほしい
    if( m_pCameraManager )
    {
      gfl2::math::Matrix34 mtx_view;
      m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
      m_pModelInstanceNode->SetBillboardRotation( mtx_view, gfl2::renderingengine::BillboardType::ScreenXYZ );
    }
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
