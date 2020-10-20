/**
* @brief  フィールドカメラの操作、イベントスクリプト用
* @file   FieldCameraControllerTypeEventScript.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeEventScript.h"



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
ControllerTypeEventScript::ControllerTypeEventScript( const InitializeDescription& rInitDesc ) :

  // 内部制御
  m_ActionState( ActionState::NONE )
  ,m_MoveEndFrame( 0 )
  ,m_MoveNowFrame( 0 )
  // ,m_MoveStartRotation
  ,m_MoveStartPosition( 0.0f ,0.0f, 0.0f )
  // ,m_MoveTargetRotation
  ,m_MoveTargetPosition( 0.0f ,0.0f, 0.0f )
  ,m_MoveEasingFunc( gfl2::math::Easing::LINEAR )
  // 外部オブジェクト
  ,m_pBaseCamera( NULL )
{

}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeEventScript::~ControllerTypeEventScript( void )
{

}

/**
* @brief 初期化処理
* @note  ControllerがCameraUnitの設定された直後に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeEventScript::SetUp( poke_3d::model::BaseCamera* pCamera )
{
  // パラメータ初期化
  m_ActionState = ActionState::NONE;
  m_MoveEndFrame = 0;
  m_MoveNowFrame = 0;
  // ,m_MoveStartRotation
  m_MoveStartPosition = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
  // ,m_MoveTargetRotation
  m_MoveTargetPosition = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
  m_MoveEasingFunc = gfl2::math::Easing::LINEAR;

  // 記憶
  m_pBaseCamera = pCamera;
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeEventScript::Update( poke_3d::model::BaseCamera* pCamera )
{
  switch ( m_ActionState )
  {
  case ActionState::MOVE:
    {
      // 進行
      m_MoveNowFrame ++;
      
      // 進行度を計算
      f32 t = static_cast<f32>( m_MoveNowFrame ) / m_MoveEndFrame;
      t = gfl2::math::Easing::GetValue( m_MoveEasingFunc, t );

      // 補間
      gfl2::math::Quaternion rot = m_MoveStartRotation;
      gfl2::math::Vector3 pos = m_MoveStartPosition;
      rot.Slerp( m_MoveTargetRotation, t );
      pos.Lerp( m_MoveTargetPosition, t );

      // カメラ実体に設定
      pCamera->SetPosition( pos );
      pCamera->SetRotationQuat( rot );

      // 終了
      if( m_MoveNowFrame >= m_MoveEndFrame )
      {
        m_ActionState = ActionState::NONE;
        break;
      }

      break;
    } // case ActionState::MOVE:
  } // switch ( m_ActionState )
}

/**
* @brief 移動命令発行
*
* @param  rCameraPosition カメラ位置、移動目標
* @param  rTargetPosition カメラターゲット位置、移動目標
* @param  frame           移動にかけるフレーム
* @param  easingFunc      移動に使用するイージング関数
*
* @return 成否
*/
b32 ControllerTypeEventScript::Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Vector3& rTargetPosition, u32 frame , gfl2::math::Easing::EaseFunc easingFunc )
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  if( frame < 1 )
  {
    // @todo
    return false;
  }

  // 座標情報->回転
  {
    gfl2::math::Quaternion rotation;
    gfl2::math::Vector3 cameraUpVector( 0.0f, 1.0f, 0.0f );
    gfl2::math::Vector3 v = rCameraPosition - rTargetPosition;

    gfl2::math::Vector3 zAxis = v.Normalize();
    gfl2::math::Vector3 xAxis = cameraUpVector.Cross(zAxis).Normalize();
    gfl2::math::Vector3 yAxis = zAxis.Cross(xAxis).Normalize();

    gfl2::math::Matrix34 rotMatrix(
      gfl2::math::Vector4(xAxis.x, yAxis.x, zAxis.x, 0.0f),
      gfl2::math::Vector4(xAxis.y, yAxis.y, zAxis.y, 0.0f),
      gfl2::math::Vector4(xAxis.z, yAxis.z, zAxis.z, 0.0f)
      );
    rotation.MatrixToQuaternion( rotMatrix );

    return Move(
      rCameraPosition,
      rotation,
      frame,
      easingFunc
      );
  }
}

/**
* @brief 移動命令発行( カメラ座標/回転指定版 )
*
* @param  rCameraPosition カメラ位置、移動目標
* @param  rRotation       カメラ回転
* @param  frame           移動にかけるフレーム
* @param  easingFunc      移動に使用するイージング関数
*
* @return 成否
*/
b32 ControllerTypeEventScript::Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Quaternion& rRotation, u32 frame, gfl2::math::Easing::EaseFunc easingFunc )
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  if( frame < 1 )
  {
    // @todo
    return false;
  }

  //
  // 移動設定記憶
  //
  m_MoveEasingFunc = easingFunc;
  m_MoveEndFrame = frame;
  m_MoveTargetPosition = rCameraPosition;
  m_MoveTargetRotation = rRotation;

  //
  // 現在の状態を記憶
  //
  m_MoveStartPosition = m_pBaseCamera->GetPosition();
  m_MoveStartRotation = m_pBaseCamera->GetRotationQuat();

  //
  // 状態変更
  //
  m_MoveNowFrame = 0;
  m_ActionState = ActionState::MOVE;


  return true;
}

/**
* @brief 動いているか取得
* @note  アニメーション中もtrueを返す
*
* @return true 動いている  false 動いていない
*/
b32 ControllerTypeEventScript::IsMoving( void ) const
{
  return ( m_ActionState != ActionState::NONE );
}

#if 0
/**
* @brief 内部制御パラメータをカメラ実体に反映
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeEventScript::SetParam( poke_3d::model::BaseCamera* pCamera )
{

}
#endif

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );