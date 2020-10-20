//======================================================================
/**
 * @file FieldEffectTrial3.cpp
 * @date 2016/03/16 16:30:28
 * @author miyachi_soichi
 * @brief 試練3専用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial3.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
// ref
#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectTrial3
 *  @brief  エフェクト：試練3専用
 */
/**
 *  @brief  コンストラクタ
 */
EffectTrial3::EffectTrial3( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pCameraManager( NULL )
{
  for( u32 idx = 0; idx < EFF_HDL_MAX; ++ idx )
  {
    m_pEffectHandle[idx] = NULL;
  }
  m_nIndex = 0;
  m_nCoolTime = 0;
}

/**
 *  @brief  デストラクタ
 */
EffectTrial3::~EffectTrial3( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectTrial3::Initialize( SetupData &setupData )
{
  m_vPosition = setupData.position;
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  for( u32 idx = 0; idx < EFF_HDL_MAX; ++ idx )
  {
    m_pEffectHandle[idx] = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
  }
  m_pCameraManager = setupData.pCameraManager;
}

/**
 *  @brief  破棄
 */
bool EffectTrial3::Terminate( void )
{
  for( u32 idx = 0; idx < EFF_HDL_MAX; ++ idx )
  {
    if( m_pEffectHandle[idx]->GetEmitterSet() )
    {
      m_pEffectHandle[idx]->GetEmitterSet()->Kill();
    }
    GFL_SAFE_DELETE( m_pEffectHandle[idx] );
  }
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
bool EffectTrial3::IsAnimationLastFrame( void )
{
  // まだ再生中
  return false;
}

/**
 *  @brief  エフェクトを再生する
 */
void EffectTrial3::CreateEffect( s32 idx )
{
  if( m_nCoolTime > 0 ){ return; }

  gfl2::Effect::Handle *pHandle = m_pEffectHandle[m_nIndex];
  if( pHandle && pHandle->GetEmitterSet() && pHandle->GetEmitterSet()->IsAlive() )
  {
    pHandle->GetEmitterSet()->Kill();
    pHandle->Invalidate();
  }

  switch( idx )
  {
  case Field::FieldScript::EFF_TRIAL3_KNOCK:
    m_pEffectSystem->CreateEmitterSetID( pHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL3_1 );
    break;
  case Field::FieldScript::EFF_TRIAL3_CRUSH:
    m_pEffectSystem->CreateEmitterSetID( pHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL3_2 );
    break;
  case Field::FieldScript::EFF_TRIAL3_MASH:
    m_pEffectSystem->CreateEmitterSetID( pHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL3_3 );
    break;
  case Field::FieldScript::EFF_TRIAL3_MELT:
    m_pEffectSystem->CreateEmitterSetID( pHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL3_4 );
    break;
  }

  ++ m_nIndex;
  if( m_nIndex >= EFF_HDL_MAX )
  {
    m_nIndex = 0;
  }
  m_nCoolTime = COOL_TIME;

  updatePosition();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectTrial3::updateNormal( void )
{
  if( m_nCoolTime > 0 ){ -- m_nCoolTime; }
  updatePosition();
}

/**
 *  @brief  座標更新
 */
void EffectTrial3::updatePosition( void )
{
  for( u32 idx = 0; idx < EFF_HDL_MAX; ++ idx )
  {
    if( !m_pEffectHandle[idx] ){ continue; }
    if( !m_pEffectHandle[idx]->GetEmitterSet() ){ continue; }
    if( !m_pEffectHandle[idx]->GetEmitterSet()->IsAlive() ){ continue; }
    if( !m_pCameraManager ){ continue; }

    Field::Camera::CameraUnit *pCameraUnit = m_pCameraManager->GetMainViewCamera();
    if( !pCameraUnit ){ continue; }

    // カメラの座標系を取得する
    gfl2::math::Vector3 vTarget;
    gfl2::math::Vector3 vPosition;
    gfl2::math::Vector3 vUpY( 0.0f, 1.0f, 0.0f );
    pCameraUnit->GetCameraAndTargetPosition( &vTarget, &vPosition );
    const poke_3d::model::BaseCamera *pBaseCamera = pCameraUnit->GetBaseCamera();
    if( !pBaseCamera ){ continue; }

    // 座標
    gfl2::math::Vector3 rot = pBaseCamera->GetRotation();
    gfl2::math::Vector3 offset = vTarget - vPosition;
    m_vPosition = vPosition + offset.Normalize() * 200.0f;
    gfl2::math::Matrix34 mtx_rot = gfl2::math::Matrix34::GetRotationZYX( rot.x, rot.y, rot.z );
    gfl2::math::Vector3 dstY( 0.0f, -100.0f, 0.0f );
    dstY = mtx_rot * dstY;
    m_vPosition += dstY;
    nw::math::VEC3 nwPos( m_vPosition.x, m_vPosition.y, m_vPosition.z );
    // 回転
    gfl2::math::Vector3 unitZ = gfl2::math::Vector3::GetZUnit();
    gfl2::math::Vector3 dir   = vPosition - m_vPosition;
    dir = dir.Normalize();
    gfl2::math::Quaternion quat( 0.0f, 0.0f, 0.0f, 1.0f );
    gfl2::math::Vector3 axis  = unitZ.Cross( dir );
    if( !gfl2::math::IsAlmostZero( axis.LengthSq() ) )
    {
      axis.Normalize();
      f32                 dot   = unitZ.Dot(   dir );
      f32                 angle = gfl2::math::AcosRad( dot );
      quat = gfl2::math::Quaternion( axis, angle );
    }
    f32 rotx,roty,rotz;
    quat.QuaternionToRadian( &rotx, &roty, & rotz );
    nw::math::VEC3 nwRot( rotx, roty, rotz );
    // 拡縮
    nw::math::VEC3 nwScl( 1.0f, 1.0f, 1.0f );
    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScl,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScl,nwRot,nwPos);
#endif
    m_pEffectHandle[idx]->GetEmitterSet()->SetMtx( srt );
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
