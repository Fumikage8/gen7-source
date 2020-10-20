/**
* @brief  フィールドカメラの操作、イベントスクリプト用
* @file   FieldCameraControllerTypeEventScript.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"
#include "debug/include/gfl2_DebugPrint.h"


#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"   // areaを見てしまった

#if defined(GF_PLATFORM_WIN32)
#include "Field/FieldRo/include/Camera/FieldCameraRecorder.h"
#endif
#include "System/include/HeapDefine.h"
#include <system/include/InputDevice/gfl2_KeyBoard.h>

#if PM_DEBUG
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#endif  // #if PM_DEBUG

#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

// 引数のデフォルト値
const f32 ControllerTypeEventScript::DEF_BANK_DEG( 0.0f );
const f32 ControllerTypeEventScript::DEF_FOVY_RAD( -1.0f );


// Vector3の全てのメンバーを度→ラジアン変換
void ConvertDegToRad( gfl2::math::Vector3* pOut )
{
  pOut->x = gfl2::math::ConvDegToRad( pOut->x );
  pOut->y = gfl2::math::ConvDegToRad( pOut->y );
  pOut->z = gfl2::math::ConvDegToRad( pOut->z );
}

void ConvertRadToDeg( gfl2::math::Vector3* pOut )
{
  pOut->x = gfl2::math::ConvRadToDeg( pOut->x );
  pOut->y = gfl2::math::ConvRadToDeg( pOut->y );
  pOut->z = gfl2::math::ConvRadToDeg( pOut->z );
}

// 移動命令情報の補間
void Ipo( ControllerTypeEventScript::TARGET_PARAM* pOut, const ControllerTypeEventScript::TARGET_PARAM& rStart, const ControllerTypeEventScript::TARGET_PARAM& rEnd, f32 t )
{
  pOut->cameraPosition = rStart.cameraPosition;
  pOut->cameraPosition.Lerp( rEnd.cameraPosition, t );
  pOut->cameraTargetPosition = rStart.cameraTargetPosition;
  pOut->cameraTargetPosition.Lerp( rEnd.cameraTargetPosition, t );

  // 画角
  pOut->fovY =gfl2::math::Lerp( rStart.fovY, rEnd.fovY, t );
  
  // 回転パラメータは球面補間
  gfl2::math::EulerRotation rot1;
  gfl2::math::EulerRotation rot2;

  gfl2::math::Quaternion q1;
  gfl2::math::Quaternion q2;

  rot1.Set( gfl2::math::Vector3( 0.0f, 0.0f, rStart.bank ), gfl2::math::EulerRotation::RotationOrderYXZ );
  rot2.Set( gfl2::math::Vector3( 0.0f, 0.0f, rEnd.bank ), gfl2::math::EulerRotation::RotationOrderYXZ );

  q1 = rot1.ToQuaternion();
  q2 = rot2.ToQuaternion();
  f32 dummy;
  q1.Slerp( q2, t );
  q1.QuaternionToRadian( &dummy, &dummy, &pOut->bank );
}

// 移動命令情報の補間
void Ipo( ControllerTypeEventScript::ROTATION_PARAM* pOut, const ControllerTypeEventScript::ROTATION_PARAM& rStart, const ControllerTypeEventScript::ROTATION_PARAM& rEnd, f32 t )
{
  pOut->distance = gfl2::math::Lerp( rStart.distance, rEnd.distance, t );
  pOut->targetPosition = rStart.targetPosition;
  pOut->targetPosition.Lerp( rEnd.targetPosition, t );
  
  // 回転パラメータは球面補間
  gfl2::math::EulerRotation rot1;
  gfl2::math::EulerRotation rot2;

  gfl2::math::Quaternion q1;
  gfl2::math::Quaternion q2;

  rot1.Set( gfl2::math::Vector3( rStart.rotation.x, rStart.rotation.y, 0.0f ), gfl2::math::EulerRotation::RotationOrderYXZ );
  rot2.Set( gfl2::math::Vector3( rEnd.rotation.x, rEnd.rotation.y, 0.0f ), gfl2::math::EulerRotation::RotationOrderYXZ );

  q1 = rot1.ToQuaternion();
  q2 = rot2.ToQuaternion();
  f32 dummy;

  q1.Slerp( q2, t );
  q1.QuaternionToRadian( &pOut->rotation.x, &pOut->rotation.y, &dummy );

 
  rot1.Set( gfl2::math::Vector3( 0.0f, 0.0f, rStart.rotation.z ), gfl2::math::EulerRotation::RotationOrderYXZ );
  rot2.Set( gfl2::math::Vector3( 0.0f, 0.0f, rEnd.rotation.z ), gfl2::math::EulerRotation::RotationOrderYXZ );
  q1 = rot1.ToQuaternion();
  q2 = rot2.ToQuaternion();

  q1.Slerp( q2, t );
  q1.QuaternionToRadian( &dummy, &dummy, &pOut->rotation.z );

}

// 回転指定形パラメーターからターゲット座標とカメラ座標を取得する
void GetPosition( const ControllerTypeEventScript::ROTATION_PARAM& rDara, gfl2::math::Vector3* pOutCameraPostion, gfl2::math::Vector3* pOutTargetPostion )
{
  // パラメータから最新の座標を計算
  f32 distance = rDara.distance;
  gfl2::math::Vector3 rotation = rDara.rotation;

  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
  gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( 0.0f, rotation.y, rotation.x ) );
  //gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZ( rotation.z ) * gfl2::math::Matrix34::GetRotationX( rotation.x )  * gfl2::math::Matrix34::GetRotationY( rotation.y ) );
  cameraOffset = rotMat.TransformCoord( cameraOffset );

  *pOutTargetPostion = rDara.targetPosition;
  *pOutCameraPostion = rDara.targetPosition + cameraOffset;
}

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
  ,m_Bank( 0.0f )
  ,m_MoveType( MoveType::TARGET )
  ,m_MoveEndFrame( 0 )
  ,m_MoveNowFrame( 0 )
  ,m_MoveWaitFrame( 0 )
  ,m_MoveEasingFunc( gfl2::math::Easing::LINEAR )
  ,m_MoveTransType( Trans::ROTATION )
#if defined(GF_PLATFORM_WIN32)
  ,m_pCameraRecorder( NULL )
#endif
  // 外部オブジェクト
  ,m_pBaseCamera( NULL )
{
#if defined(GF_PLATFORM_WIN32)
  // 記録
  {
    gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    CameraRecorder::InitializeDescription initDesc;
    initDesc.m_pSystemHeap = pHeap;
    m_pCameraRecorder = GFL_NEW( pHeap ) CameraRecorder();
    m_pCameraRecorder->Initialize( initDesc );
  }
#endif
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeEventScript::~ControllerTypeEventScript( void )
{
#if defined(GF_PLATFORM_WIN32)
  GFL_SAFE_DELETE( m_pCameraRecorder );
#endif
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
  m_MoveWaitFrame = 0;
  m_MoveEasingFunc = gfl2::math::Easing::LINEAR;

  // シェイク
  m_pBaseCamera = pCamera;
  m_ShakeController.Set( m_pBaseCamera );

#if defined(GF_PLATFORM_WIN32)
  // 記録
  if( m_pCameraRecorder != NULL)
  {
    m_pCameraRecorder->SetTarget( pCamera );
  }
#endif
}

/**
* @brief 外したときの処理
* @note  ControllerがCameraUnitから交換される直前に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeEventScript::Remove( poke_3d::model::BaseCamera* pCamera )
{
#if defined(GF_PLATFORM_WIN32)
  // 記録
  if( m_pCameraRecorder != NULL)
  {
    m_pCameraRecorder->ClearTarget();
  }
#endif

  m_ShakeController.Clear();
  m_pBaseCamera = NULL;
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
  // シェイク更新
  m_ShakeController.Update();

  // アニメーション更新 @note シェイク内部でも行っている
  if( m_ShakeController.IsShake() == false )
  {
    // アニメ更新
    m_pBaseCamera->UpdateAnimation();
  }


  switch ( m_ActionState )
  {
  case ActionState::MOVE:
    {
      UpdateMove( pCamera );
      break;
    } // case ActionState::MOVE:
  } // switch ( m_ActionState )

#if defined(GF_PLATFORM_WIN32)
  // 記録処理
  if( m_pCameraRecorder->IsRecording() )
  {
#if PM_DEBUG
    // 記録中を表示
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 1.0f, 1.0f );
    ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
    gfl2::str::MsgData* pMsgData = devWork->GetMessageData();
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    pMsgData->GetString(msg_dmenu_camera_ctrl_rec, *workStrBuf );
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 350.0f, 0.0f, workStrBuf->GetPtr() );
#endif

    // 記録
    m_pCameraRecorder->Update( m_Bank );
  }
#endif // defined(GF_PLATFORM_WIN32)
}


/**
* @brief 移動命令発行
*
* @param  rCameraPosition カメラ位置、移動目標
* @param  rTargetPosition カメラターゲット位置、移動目標
* @param  frame           移動にかけるフレーム
* @param  easingFunc      移動に使用するイージング関数
* @param  bankDeg         バンク回転(度)
* @param  fovY            画角指定(度)、-1.0で無し
* @param  waitFrame       移動開始待ち時間
*
* @return 成否
*/
b32 ControllerTypeEventScript::Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Vector3& rTargetPosition, u32 frame , gfl2::math::Easing::EaseFunc easingFunc, f32 bankDeg, f32 fovY, u32 waitFrame )
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  if( frame < 1 )
  {
    GFL_ASSERT( 0 ); // 開発中に気づくためのアサート
    return false;
  }

  // 移動設定記憶
  m_MoveEasingFunc = easingFunc;
  m_MoveEndFrame = frame;

  // Start 
  m_MoveTargetStart.cameraPosition = m_pBaseCamera->GetPosition( false );       // アニメブレンドは考慮しない
  m_MoveTargetStart.cameraTargetPosition = m_pBaseCamera->GetTargetPosition();
  m_MoveTargetStart.bank = m_Bank;
  m_MoveTargetStart.fovY = m_pBaseCamera->GetFovy( false );

  // End
  m_MoveTargetEnd.cameraPosition = rCameraPosition;
  m_MoveTargetEnd.cameraTargetPosition = rTargetPosition;
  m_MoveTargetEnd.bank = gfl2::math::ConvDegToRad( bankDeg );
  if( fovY != -1.0f )
  {
    m_MoveTargetEnd.fovY = gfl2::math::ConvDegToRad( fovY );
  }
  else
  {
    m_MoveTargetEnd.fovY = m_MoveTargetStart.fovY;
  }

  // 状態変更
  m_MoveNowFrame = 0;
  m_MoveWaitFrame = waitFrame;
  m_ActionState = ActionState::MOVE;
  m_MoveType = MoveType::TARGET;

  return true;
}

/**
* @brief 移動命令発行( ターゲット座標/距離/回転指定版 )
*
* @param  rTargetPosition カメラターゲット位置、移動目標
* @param  rRotation       回転
* @param  distance        距離
* @param  frame           移動にかけるフレーム
* @param  easingFunc      移動に使用するイージング関数
* @param  transType       移動時の遷移型式
*
* @return 成否
*/
b32 ControllerTypeEventScript::Move( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rRotation, f32 distance, u32 frame, gfl2::math::Easing::EaseFunc easingFunc, Trans::Value transType )
{
  // 直線移動なら最終座標を求めて通常移動命令を発行
  if( transType == Trans::POSITON )
  {
    ROTATION_PARAM end;
    end.rotation = rRotation;
    end.targetPosition = rTargetPosition;
    end.distance = distance;
    ConvertDegToRad( &end.rotation );

    gfl2::math::Vector3 fixTargetPosition;
    gfl2::math::Vector3 fixCameraPosition;
    GetPosition( end, &fixCameraPosition, &fixTargetPosition );

    return Move( fixCameraPosition, fixTargetPosition, frame, easingFunc, end.rotation.z );
  }

  // 移動設定記憶
  m_MoveEasingFunc = easingFunc;
  m_MoveEndFrame = frame;
  m_MoveTransType = transType;

  // 開始情報
  m_MoveRotationStart.targetPosition = m_pBaseCamera->GetTargetPosition();
  m_MoveRotationStart.distance = ( m_pBaseCamera->GetPosition( false ) - m_pBaseCamera->GetTargetPosition() ).Length();
  Field::Camera::Area::Utility::ConvertQuatToEuler( m_pBaseCamera->GetRotationQuat( false ), &m_MoveRotationStart.rotation );

  m_MoveRotationStart.rotation.z = m_Bank;

  // 終了情報
  m_MoveRotationEnd.rotation = rRotation;
  m_MoveRotationEnd.targetPosition = rTargetPosition;
  m_MoveRotationEnd.distance = distance;
  ConvertDegToRad( &m_MoveRotationEnd.rotation );

#if 0
  gfl2::math::Vector3 work;
  work = m_MoveRotationStart.rotation;
  ConvertRadToDeg( &work );
  GFL_PRINT( "start %f %f %f \n", work.x, work.y, work.z );
  work = m_MoveRotationEnd.rotation;
  ConvertRadToDeg( &work );
  GFL_PRINT( "end %f %f %f ", work.x, work.y, work.z );
#endif

  // 状態変更
  m_MoveNowFrame = 0;
  m_MoveWaitFrame = 0;
  m_ActionState = ActionState::MOVE;
  m_MoveType = MoveType::DISTANCE_ROTATION;
  
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

/**
* @brief アニメーション再生
*
* @return 無し
*/
void ControllerTypeEventScript::SetAnimation( gfl2::renderingengine::scenegraph::resource::ResourceNode* pData )
{
  m_pBaseCamera->ChangeAnimationGlobalByResourceNode( pData );
  m_pBaseCamera->SetAnimationLoop( false );
  m_pBaseCamera->SetAnimationStepFrame( 1.0f );
  m_pBaseCamera->SetLocalAnimationScale( 1.0f );
}

/**
* @brief アニメーションしているか取得
*
* @return true 動いている  false 動いていない
*/
b32 ControllerTypeEventScript::IsAnimation( void ) const
{
  return m_pBaseCamera->IsAnimation();
}


/**
* @brief カメラ情報を取得
*
* @return 成否
*/
b32 ControllerTypeEventScript::GetCameraPosition( gfl2::math::Vector3* pOut, b32 isAnimeBlend ) const
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  *pOut = m_pBaseCamera->GetPosition( isAnimeBlend );
  return true;
}

b32 ControllerTypeEventScript::GetTargetPosition( gfl2::math::Vector3* pOut ) const
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  *pOut = m_pBaseCamera->GetTargetPosition();
  return true;
}

#if PM_DEBUG
void ControllerTypeEventScript::SetMoveFirstFrame( void )
{
  // デバッグ用途なので強引
  if( m_MoveEndFrame == 0 )
  {
    return;
  }

  m_MoveNowFrame = 0;
  m_MoveWaitFrame = 1;

  UpdateMove( m_pBaseCamera );
}

void ControllerTypeEventScript::SetMoveEndFrame( void )
{
  // デバッグ用途なので強引
  if( m_MoveEndFrame == 0 )
  {
    return;
  }
  
  m_MoveNowFrame = m_MoveEndFrame;
  m_MoveWaitFrame = 1;

  UpdateMove( m_pBaseCamera );

  // @note m_ActionState = ActionState::NONEになる
}
#endif

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

/**
* @brief 移動更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeEventScript::UpdateMove( poke_3d::model::BaseCamera* pCamera )
{
  // 進行
  if( m_MoveWaitFrame == 0 )
  {
    m_MoveNowFrame ++;
  }
  else
  {
    m_MoveWaitFrame --;
  }

  // 進行度を計算
  f32 t = static_cast<f32>( m_MoveNowFrame ) / m_MoveEndFrame;
  t = gfl2::math::Easing::GetValue( m_MoveEasingFunc, t );

  // カメラ更新
  switch( m_MoveType )
  {
  case MoveType::TARGET:
    {
      // 補間
      TARGET_PARAM now;
      Ipo( &now, m_MoveTargetStart, m_MoveTargetEnd, t );

      // カメラ実体に設定
      gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZ( now.bank ) );
      gfl2::math::Vector3 cameraUp( 0.0f, 1.0f, 0.0f );
      pCamera->SetupCameraLookAt( now.cameraPosition, now.cameraTargetPosition, cameraUp );
      pCamera->SetFovy( now.fovY );
      
      // 強引にBank回転
      gfl2::math::Quaternion rot = pCamera->GetRotationQuat();
      cameraUp = rotMat.TransformCoord( cameraUp );
      cameraUp = rot.Transform( cameraUp );
      pCamera->SetupCameraLookAt( now.cameraPosition, now.cameraTargetPosition, cameraUp );

      // 記憶
      m_Bank = now.bank;
      break;
    }
  case MoveType::DISTANCE_ROTATION:
    {
      // 補間
      ROTATION_PARAM now;
      Ipo( &now, m_MoveRotationStart, m_MoveRotationEnd, t );

      // カメラ実体に設定
      gfl2::math::Vector3 targetObjectPosition;
      gfl2::math::Vector3 fixTargetPosition;
      gfl2::math::Vector3 fixCameraPosition;

      GetPosition( now, &fixCameraPosition, &fixTargetPosition );

      pCamera->SetTargetPosition( fixTargetPosition );
      pCamera->SetPosition( fixCameraPosition );
      pCamera->SetRotation( now.rotation.x, now.rotation.y, now.rotation.z, gfl2::math::EulerRotation::RotationOrderYXZ) ;
      
      // 記憶
      m_Bank = now.rotation.z;
      break;
    }

#if 0
  case MoveType::POS_ROT:
    {
      // 補間
      gfl2::math::Quaternion rot = m_MoveStartRotation;
      gfl2::math::Vector3 pos = m_MoveStartPosition;
      rot.Slerp( m_MoveTargetRotation, t );
      pos.Lerp( m_MoveTargetPosition, t );

      // カメラ実体に設定
      pCamera->SetPosition( pos );
      pCamera->SetRotationQuat( rot );
    }
#endif
  }

  gfl2::math::Vector3 rot;

  Field::Camera::Area::Utility::ConvertQuatToEuler( pCamera->GetRotationQuat(), &rot );
  rot.x = gfl2::math::ConvRadToDeg( rot.x );
  rot.y = gfl2::math::ConvRadToDeg( rot.y );
  rot.z = gfl2::math::ConvRadToDeg( rot.z );


  // 終了
  if( m_MoveNowFrame >= m_MoveEndFrame )
  {
    m_ActionState = ActionState::NONE;
  }
}



//
// イベントカメラエディタ用ここから
//
#if DEBUG_EVENT_CAMERA_EDITOR

void ControllerTypeEventScript::GetTargetParamStart( TARGET_PARAM* pOut)
{
  *pOut = m_MoveTargetStart;
}

void ControllerTypeEventScript::GetTargetParamEnd( TARGET_PARAM* pOut)
{
  *pOut = m_MoveTargetEnd;
}

void ControllerTypeEventScript::GetCommonParameters(
  ActionState::Value* pActionState,
  f32* pOutBank,
  u32* pMoveType,
  u32* pOutMoveEndFrame,
  u32* pOutMoveNowFrame,
  gfl2::math::Easing::EaseFunc* pOutEasingFunc
  )
{
  *pActionState = m_ActionState;
  *pOutBank = m_Bank;
  *pMoveType = m_MoveType;
  *pOutMoveEndFrame = m_MoveEndFrame;
  *pOutMoveNowFrame = m_MoveNowFrame;
  *pOutEasingFunc = m_MoveEasingFunc;
}

void ControllerTypeEventScript::SetTargetParamStart( const TARGET_PARAM& rIn)
{
  m_MoveTargetStart = rIn;
}

void ControllerTypeEventScript::SetTargetParamEnd( const TARGET_PARAM& rIn)
{
  m_MoveTargetEnd = rIn;
}

void ControllerTypeEventScript::SetCommonParameters(
  ActionState::Value actionState,
  f32 bank,
  u32 moveType,
  u32 moveEndFrame,
  u32 moveNowFrame,
  gfl2::math::Easing::EaseFunc easingFunc
  )
{
  m_ActionState = actionState;
  m_Bank = bank;
  m_MoveType = static_cast<MoveType::Value>( moveType);
  m_MoveEndFrame = moveEndFrame;
  m_MoveNowFrame = moveNowFrame;
  m_MoveEasingFunc = easingFunc;
}

#endif // DEBUG_EVENT_CAMERA_EDITOR
//
// イベントエディタ用ここまで
//


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );