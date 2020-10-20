//======================================================================
/**
 * @file FieldEffectTrial5.cpp
 * @date 2016/03/17 15:52:41
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial5.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
// ref
#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectTrial5
 *  @brief  エフェクト：試練3専用
 */
/**
 *  @brief  コンストラクタ
 */
EffectTrial5::EffectTrial5( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pEffectHandle( NULL )
, m_pCameraManager( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectTrial5::~EffectTrial5( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectTrial5::Initialize( SetupData &setupData )
{
  m_vPosition = setupData.position;
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
  m_pCameraManager = setupData.pCameraManager;
}

/**
 *  @brief  破棄
 */
bool EffectTrial5::Terminate( void )
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
bool EffectTrial5::IsAnimationLastFrame( void )
{
  // まだ再生中
  return false;
}

/**
 *  @brief  エフェクトを再生する
 */
void EffectTrial5::CreateEffect( s32 idx )
{
  // 再生済みを破棄する
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
    m_pEffectHandle->Invalidate();
  }

  switch( idx )
  {
  case Field::FieldScript::EFF_TRIAL5_STAR:
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL5_3 );
    break;
  case Field::FieldScript::EFF_TRIAL5_THUNDER_ENEMY:
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL5_4 );
    break;
  case Field::FieldScript::EFF_TRIAL5_THUNDER_BOSS:
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL5_5 );
    break;
  case Field::FieldScript::EFF_TRIAL5_THUNDER_CAPTAIN:
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DEMO_TRIAL5_6 );
    break;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectTrial5::updateNormal( void )
{
  updatePosition();
}

/**
 *  @brief  座標更新
 */
void EffectTrial5::updatePosition( void )
{
  if( !m_pEffectHandle ){ return; }
  if( !m_pEffectHandle->GetEmitterSet() ){ return; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() ){ return; }
  if( !m_pCameraManager ){ return; }

  Field::Camera::CameraUnit *pCameraUnit = m_pCameraManager->GetMainViewCamera();
  if( !pCameraUnit ){ return; }

  // カメラの座標系を取得する
  gfl2::math::Vector3 vTarget;
  gfl2::math::Vector3 vPosition;
  gfl2::math::Vector3 vUpY( 0.0f, 1.0f, 0.0f );
  pCameraUnit->GetCameraAndTargetPosition( &vTarget, &vPosition );

  // 座標
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
  m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
