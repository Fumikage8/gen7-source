//======================================================================
/**
 * @file FieldEffectDemoRide.cpp
 * @date 2016/02/03 14:55:51
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectDemoRide.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// subobject
#include "Field/FieldStatic/include/MoveModel/FieldSubobject.h"
// sound
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  コンストラクタ
 */
EffectDemoRide::EffectDemoRide( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pEffectHandle( NULL )
, m_pParent( NULL )
, m_pJointNode( NULL )
{
  m_vPosition = gfl2::math::Vector3::GetZero();
}

/**
 *  @brief  デストラクタ
 */
EffectDemoRide::~EffectDemoRide( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectDemoRide::Initialize( SetupData &setupData )
{
  m_pEffectHeap   = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
  setJointNode( setupData.pParentObject );
  m_vPosition = getJointPosition();

  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_RIDE );

  if( IsPlaySe() )
  {
    Sound::PlaySE( SEQ_SE_SD9080_BUNGLE );
  }
}

/**
 *  @brief  破棄
 */
bool EffectDemoRide::Terminate( void )
{
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  GFL_SAFE_DELETE( m_pEffectHandle );
  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  終了しているか.
 */
bool EffectDemoRide::IsAnimationLastFrame( void )
{
  if( !m_pEffectHandle ){ return true; }
  if( !m_pEffectHandle->GetEmitterSet() ){ return true; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() ){ return true; }

  // まだ再生中
  return false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectDemoRide::updateNormal( void )
{
  updatePosition();
}

/**
 *  @brief  ジョイント設定
 */
void EffectDemoRide::setJointNode( Field::IField3DObjectNode *pParent )
{
  s32 joint_index = -1;
  if( pParent->SafeCast<Field::MoveModel::FieldMoveModel>() != NULL )
  {
    Field::MoveModel::FieldMoveModel *pMoveModel = pParent->SafeCast<Field::MoveModel::FieldMoveModel>();
    Field::MoveModel::FieldSubobject *pSubObject = pMoveModel->GetSubobject( Field::MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_DYNAMIC );
    if( pSubObject ){ m_pParent = pSubObject->GetCharaDrawInstance(); }
    if( m_pParent ) { joint_index = m_pParent->GetJointIndex( "Parts" ); }
  }
  else
  {
    // MoveModel以外は許容しない
    GFL_ASSERT_MSG( false, "Parent Character is not MoveModel\n" );
  }

  // ジョイントすべきノードが見つからない
  if( !m_pParent )
  {
    GFL_ASSERT_MSG( false, "Subobject Character is not found\n" );
  }
  else if( joint_index == -1 )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = m_pParent->GetModelInstanceNode();
    u32 jointCount = pNode->GetJointNum();
    for( u32 i = 0; i < jointCount; ++ i )
    {
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode *pJoint = pNode->GetJointInstanceNode( i );

      GFL_PRINT( "joint_name %s\n", pJoint->GetName() );
    }

    GFL_ASSERT_MSG( false, "joint_name Parts not found" );
  }
  else
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = m_pParent->GetModelInstanceNode();
    m_pJointNode = pNode->GetJointInstanceNode( joint_index );
  }
}

/**
 *  @brief  座標更新
 */
void EffectDemoRide::updatePosition( void )
{
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_vPosition = getJointPosition();

    // trans
    nw::math::VEC3      nwPos( m_vPosition.x, m_vPosition.y, m_vPosition.z );

    // rotate
    gfl2::math::Quaternion quat = m_pParent->GetRotationQuat();
    f32 x, y, z;
    quat.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

/**
 *  @brief  ジョイント設定
 */
gfl2::math::Vector3 EffectDemoRide::getJointPosition( void )
{
  gfl2::math::Vector3 pos = gfl2::math::Vector3::GetZero();

  if( m_pJointNode )
  {
    pos = m_pJointNode->GetWorldMatrix().GetElemPosition();
  }

  return pos;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
