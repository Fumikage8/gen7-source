//======================================================================
/**
 * @file FieldEffectCharaRipple.cpp
 * @date 2016/01/20 17:54:24
 * @author miyachi_soichi
 * @brief エフェクト：キャラクタ波紋
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaRipple.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
// arc
#include "niji_conv_header/field/effect/chara_ripple.h"
#include "niji_conv_header/field/chara/p2_base_fi.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const f32 THRESHOLD_LENGTH = 1.0f;

/**
 *  @class  EffectCharaRipple
 *  @brief  NPC泳ぎ波紋
 *          EFFECT_TYPE_CHARA_RIPPLE
 */
/**
 *  @brief  コンストラクタ
 */
EffectCharaRipple::EffectCharaRipple( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pJointNode( NULL )
, m_pEffectHandle( NULL )
, m_eMode( MODE_NONE )
, m_bPause( false )
{
}

/**
 *  @brief  デストラクタ
 */
EffectCharaRipple::~EffectCharaRipple( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectCharaRipple::Initialize( SetupData &setupData )
{
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;

  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  m_pModel = GFL_NEW( setupData.pHeap ) poke_3d::model::BaseModel();
  void *pModelData = binLinkerAccessor.GetData( BL_IDX_CHARA_RIPPLE_EF_CHARA_SWIMMER_WAIT_WAVE_CTR_GFBMDLP );
  createModel( pModelData );
  m_pModel->Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( BL_IDX_CHARA_RIPPLE_EF_CHARA_SWIMMER_WAIT_WAVE_GFBMOT );
  createMotion( pMotionData );
  m_pModel->ChangeAnimationByResourceNode( m_pMotionResourceRootNode );
  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  // Joint設定
  setJointNode( setupData.pParentObject );
  // ポジションの更新
  m_vPosition = getJointPosition();
}

/**
 *  @brief  破棄
 */
bool EffectCharaRipple::Terminate( void )
{
  // GFCHECK[8464]:安全担保、視界確保のためエフェクト破棄を追加しておく
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  GFL_SAFE_DELETE( m_pModel );
  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode );
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
void EffectCharaRipple::Delete( void )
{
  changeMode( MODE_NONE );
}

/**
 *  @brief  停止
 */
void EffectCharaRipple::Suspend( void )
{
  // @fix GFNMCat[773]
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->SetStopCalc( true );
  }
  m_bPause = true;
}

/**
 *  @brief  再開
 */
void EffectCharaRipple::Resume( void )
{
  // @fix GFNMCat[773]
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->SetStopCalc( false );
  }
  m_bPause = false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  モデルの生成
 */
void EffectCharaRipple::createModel( void *pModelBinary )
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
void EffectCharaRipple::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}

/**
 *  @brief  ジョイント設定
 */
void EffectCharaRipple::setJointNode( Field::IField3DObjectNode *pParent )
{
  s32 joint_index = -1;
  if( pParent->SafeCast<Field::MoveModel::FieldMoveModel>() != NULL )
  {
    Field::MoveModel::FieldMoveModel *pMoveModel = pParent->SafeCast<Field::MoveModel::FieldMoveModel>();
    m_pParent = pMoveModel->GetCharaDrawInstance();
    joint_index = m_pParent->GetJointIndex( "Head" );
  }
  else
  {
    // MoveModel以外は許容しない
    GFL_ASSERT_MSG( false, "Parent Character is not MoveModel\n" );
  }

  // ジョイントすべきノードが見つからない
  if( joint_index == -1 )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = m_pParent->GetModelInstanceNode();
    u32 jointCount = pNode->GetJointNum();
    for( u32 i = 0; i < jointCount; ++ i )
    {
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode *pJoint = pNode->GetJointInstanceNode( i );

      GFL_PRINT( "joint_name %s\n", pJoint->GetName() );
    }

    GFL_ASSERT_MSG( false, "joint_name Head not found" );
  }
  else
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = m_pParent->GetModelInstanceNode();
    m_pJointNode = pNode->GetJointInstanceNode( joint_index );
  }
}

/**
 *  @brief  更新処理
 */
void EffectCharaRipple::updateNormal( void )
{
  // @fix GFNMCat[773]
  if( m_bPause ){ return; }

  // 座標更新
  updatePosition();
  m_pModel->UpdateAnimation();
}

/**
 *  @brief  座標更新
 */
void EffectCharaRipple::updatePosition( void )
{
  gfl2::math::Vector3 vPos = getJointPosition();
  gfl2::math::Vector3 diff = vPos - m_vPosition;
  m_vPosition = vPos;

  s32 motID = m_pParent->GetAnimationId();
  switch( motID )
  {
  case P2_BASE_FI001_WAIT01:        // no break;
  case P2_BASE_FI016_TURN01_RIGHT:  // no break;
  case P2_BASE_FI017_TURN01_LEFT:   // no break;
  case P2_BASE_FI135_SEARCH03:      // no break;
  case P2_BASE_FI132_POSE04:        // no break;
    {
      // 待機
      changeMode( MODE_WAIT );
    }
    break;

  case P2_BASE_FI010_WALK01:        // no break;
    {
      // 移動
      changeMode( MODE_MOVE );
    }
    break;

  default:
    {
      // デフォルトは移動
      GFL_ASSERT( 0 );
      changeMode( MODE_MOVE );
    }
    break;
  }

  // モデル
  m_pModel->SetPosition( vPos );
  gfl2::math::Quaternion quat = m_pParent->GetRotationQuat();
  m_pModel->SetRotationQuat( quat );

  // エミッター
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    // trans
    nw::math::VEC3      nwPos( vPos.x, vPos.y, vPos.z );

    // rotate
    f32 x, y, z;
    quat.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot  ( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

/**
 *  @brief  ジョイント設定
 */
gfl2::math::Vector3 EffectCharaRipple::getJointPosition( void )
{
  gfl2::math::Vector3 pos = gfl2::math::Vector3::GetZero();

  if( m_pJointNode )
  {
    pos = m_pJointNode->GetWorldMatrix().GetElemPosition();
    // YはOriginから
    pos.y = m_pParent->GetPosition().y;
  }

  return pos;
}

/**
 *  @brief  描画モードの切り替え
 */
void EffectCharaRipple::changeMode( u32 mode )
{
  switch( mode )
  {
  case MODE_NONE:
    {
      if( m_eMode == MODE_WAIT )
      {
        m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
      }
      if( m_eMode == MODE_MOVE )
      {
        if( m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
        {
          m_pEffectHandle->GetEmitterSet()->Kill();
          m_pEffectHandle->Invalidate(); // @fix GFNMCat[1254]ハンドルの不正アクセス防止のため
        }
      }
      m_eMode = mode;
    }
    break;
  case MODE_WAIT:
    {
      if( m_eMode == MODE_MOVE )
      {
        if( m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
        {
          m_pEffectHandle->GetEmitterSet()->Fade();
          m_pEffectHandle->Invalidate(); // @fix GFNMCat[1254]ハンドルの不正アクセス防止のため
        }
      }
      if( m_eMode != MODE_WAIT )
      {
        m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
        m_pModel->SetAnimationFrame( 0.0f );
      }
      m_eMode = mode;
    }
    break;
  case MODE_MOVE:
    {
      if( m_eMode == MODE_WAIT )
      {
        m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
      }
      bool bAlive = true;
      if( !m_pEffectHandle->GetEmitterSet() ){ bAlive = false; }
      else if( !m_pEffectHandle->GetEmitterSet()->IsAlive() ){ bAlive = false; }
      if( !bAlive )
      {
        m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_CHARA_RIPPLE );
      }
      m_eMode = mode;
    }
    break;
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

