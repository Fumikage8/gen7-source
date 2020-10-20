//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeHold.cpp
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( 固定型 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeHold.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"

// camera
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"


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
SubControllerTypeHold::SubControllerTypeHold( 
  const DATA& rData, 
  const Target::TargetBase* pTarget,
  s32 dataNo
  )
{
  m_Data = rData;
  m_DataNo = dataNo;
  m_pTarget = pTarget;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
SubControllerTypeHold::~SubControllerTypeHold( void )
{

}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void SubControllerTypeHold::Update( poke_3d::model::BaseCamera* pCamera )
{
  gfl2::math::Vector3 targetObjectPosition;
  gfl2::math::Vector3 fixTargetPosition;
  gfl2::math::Vector3 fixCameraPosition;
  m_pTarget->GetCameraTargetPosition( &targetObjectPosition );

  // パラメータから最新の座標を計算
  GetTargetPosition( &fixTargetPosition );
  fixCameraPosition = m_Data.position;

  // カメラ実体に設定
  pCamera->SetupCameraLookAt(
    fixCameraPosition,
    fixTargetPosition,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
    );

  pCamera->SetFovy( m_Data.fovY );

  // バンク回転( 補間が必要なケースは運用上ない )
  gfl2::math::Quaternion rot = pCamera->GetRotationQuat();
  gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZ( m_Data.bank ) );
  gfl2::math::Quaternion zRot;
  zRot.MatrixToQuaternion( rotMat );
  rot = rot * zRot;

  pCamera->SetRotationQuat( rot );

}

/**
* @brief　最新のターゲット座標を取得する
*
* @return 無し
*/
void SubControllerTypeHold::GetTargetPosition( gfl2::math::Vector3* pOut ) const
{
  if( m_Data.isTargetFocus )
  {
    m_pTarget->GetCameraTargetPosition( pOut );
    *pOut += m_Data.offset;
  }
  else
  {
    *pOut = m_Data.target;
  }
}


GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );