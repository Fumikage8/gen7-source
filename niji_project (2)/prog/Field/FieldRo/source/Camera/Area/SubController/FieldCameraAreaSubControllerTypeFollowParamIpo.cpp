//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeFollowParamIpo.cpp
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( 現在位置から2つのパラメータが補間する追従型 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeFollowParamIpo.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// camera
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"

// debug
#include <fs/include/gfl2_Fs.h>
#include <fs/include/gfl2_FsPcUtil.h>
#endif // #if PM_DEBUG


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
GFL_NAMESPACE_BEGIN( SubController );

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
SubControllerTypeFollowParamIpo::SubControllerTypeFollowParamIpo( 
  const DATA& rData,
  const f32*  pParamRatio,              // パラメータ補間、開始時点の位置(0.0 - 1.0)
  const Target::TargetBase* pTarget,
  Area::Support*                        pSupport,
  Area::Focus*                          pFocus,
  s32 dataNo
#if PM_DEBUG
  ,b32 isDebugEffect
#endif
  ) :
  m_pParamRatio( NULL )
#if PM_DEBUG
  ,m_IsDebugEffect( isDebugEffect )
  ,m_IsPrevDebugUpdate( false )
  ,m_IsPutsInfoFile( false )
#endif
{
  m_Data = rData;
  m_DataNo = dataNo;
  m_pTarget = pTarget;
  m_pSupport = pSupport;
  m_pParamRatio = pParamRatio;
  m_pFocus = pFocus;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
SubControllerTypeFollowParamIpo::~SubControllerTypeFollowParamIpo( void )
{

}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void SubControllerTypeFollowParamIpo::Update( poke_3d::model::BaseCamera* pCamera )
{
  gfl2::math::Vector3 targetObjectPosition;
  gfl2::math::Vector3 fixTargetPosition;
  gfl2::math::Vector3 fixCameraPosition;
  m_pTarget->GetCameraTargetPosition( &targetObjectPosition );

  // カメラパラメータの補間を実行
  Area::MAIN_PARAM param;
  f32 t;
  t = gfl2::math::Easing::GetValue( m_Data.paramEaseFunc, *m_pParamRatio );
  Area::Utility::Lerp( &param, m_Data.paramA, m_Data.paramB, t );

  // パラメータから最新の座標を計算
  f32 distance = param.distance;
  gfl2::math::Vector3 rotation = param.rotation;

  // 距離サポートによる距離の加工
  distance = param.distance;
  if( m_pSupport->IsValid() )
  {
    distance = m_pSupport->ConvertDistance( distance );
  }

  // カメラに設定
  rotation = param.rotation;
  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
  gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( rotation.z, rotation.y, rotation.x ) );
  cameraOffset = rotMat.TransformCoord( cameraOffset );
  fixTargetPosition = targetObjectPosition + param.offset;
  fixCameraPosition = fixTargetPosition + cameraOffset;

  f32 fovy = param.fovY;
  pCamera->SetFovy( fovy );
  pCamera->SetupCameraLookAt(
    fixCameraPosition,
    fixTargetPosition,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
    );

  // フォーカス処理
  m_pFocus->Update( pCamera, param.distance );
  m_pFocus->Execute( pCamera );
}


GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );