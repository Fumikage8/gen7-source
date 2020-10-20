/**
* @brief  フィールドカメラの操作、イベントスクリプト用
* @file   FieldCameraControllerTypeEventScript.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>

// field
#include "Field/FieldRo/include/Camera/FieldCameraShakeController.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"


#if PM_DEBUG
#define DEBUG_EVENT_CAMERA_EDITOR 1
#else
#define DEBUG_EVENT_CAMERA_EDITOR 0
#endif // PM_DEBUG


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModel;
GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

#if defined(GF_PLATFORM_WIN32)
class CameraRecorder;
#endif

GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief フィールドカメラの操作、イベントスクリプト用
*/
class ControllerTypeEventScript : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeEventScript );

public:

  // 行動の状態識別子
  struct ActionState
  {
    enum Value
    {
      NONE = 0,     // 何もしていない
      MOVE,         // 移動命令実行中
      ANIMATION,    // アニメーション中

      MAX
    };
  };
 
  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    u32 dummy;
  };




  struct TARGET_PARAM
  {
    gfl2::math::Vector3    cameraPosition;        // 位置
    gfl2::math::Vector3    cameraTargetPosition;  // ターゲット位置
    f32                    bank;                  // Z回転
    f32                    fovY;                  // 画角
  };


  struct ROTATION_PARAM
  {
    gfl2::math::Vector3 targetPosition;
    gfl2::math::Vector3 rotation;
    f32 distance;
  };


  // 遷移型式
  struct Trans
  {
    enum Value
    {
      ROTATION = 0,     // 回転遷移(円形で移動)
      POSITON,          // 座標遷移(直線で移動)
      MAX
    };
  };

  // 引数のデフォルト値
	static const f32 DEF_BANK_DEG;
	static const f32 DEF_FOVY_RAD;
  
    /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  ControllerTypeEventScript( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeEventScript( void );
  

  /**
  * @brief 初期化処理
  * @note  ControllerがCameraUnitの設定された直後に実行される
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void SetUp( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 外したときの処理
  * @note  ControllerがCameraUnitから交換される直前に実行される
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void Remove( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void Update( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 移動命令発行( カメラ座標/ターゲット座標指定版 )
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
  b32 Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Vector3& rTargetPosition, u32 frame, gfl2::math::Easing::EaseFunc easingFunc = gfl2::math::Easing::LINEAR, f32 bankDeg = DEF_BANK_DEG, f32 fovY = DEF_FOVY_RAD, u32 waitFrame = 0 );

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
  b32 Move( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rRotation, f32 distance, u32 frame, gfl2::math::Easing::EaseFunc easingFunc = gfl2::math::Easing::LINEAR, Trans::Value transType = Trans::ROTATION );

  /**
  * @brief 動いているか取得
  * @note  アニメーション中もtrueを返す
  *
  * @return true 動いている  false 動いていない
  */
  b32 IsMoving( void ) const;


  ShakeController* GetShakeController( void )
  {
    return &m_ShakeController;
  }

#if defined(GF_PLATFORM_WIN32)
  CameraRecorder* GetCameraRecorder( void )
  {
    return m_pCameraRecorder;
  }
#endif

  /**
  * @brief アニメーション再生
  *
  * @return 無し
  */
  void SetAnimation( gfl2::renderingengine::scenegraph::resource::ResourceNode* pData );

  /**
  * @brief アニメーションしているか取得
  *
  * @return true 動いている  false 動いていない
  */
  b32 IsAnimation( void ) const;


  /**
  * @brief カメラ情報を取得
  *
  * @return 成否
  */
  b32 GetCameraPosition( gfl2::math::Vector3* pOut, b32 isAnimeBlend = true ) const;
  b32 GetTargetPosition( gfl2::math::Vector3* pOut ) const;

#if PM_DEBUG
  void SetMoveFirstFrame( void );
  void SetMoveEndFrame( void );
  f32 GetNowBank( void ) { return m_Bank; }
#endif

private:

#if 0
  /**
  * @brief 内部制御パラメータをカメラ実体に反映
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void SetParam( poke_3d::model::BaseCamera* pCamera );
#endif

  /**
  * @brief 移動更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void UpdateMove( poke_3d::model::BaseCamera* pCamera );


  //
  // 内部制御
  //
  ActionState::Value              m_ActionState;        // 行動の状態識別子


  // 移動命令制御
  struct MoveType
  {
    enum Value
    {
      TARGET = 0,
      DISTANCE_ROTATION,
#if 0
      POS_ROT
#endif
    };
  };



  f32                             m_Bank;
  MoveType::Value                 m_MoveType;

  // 共通パラメータ
  u32                             m_MoveEndFrame;       // 移動終了フレーム
  u32                             m_MoveNowFrame;       // 現在フレーム
  u32                             m_MoveWaitFrame;      // 0になるまで停止
  gfl2::math::Easing::EaseFunc    m_MoveEasingFunc;     // 移動に使用するイージング関数
  
  // MoveType::TARGET
  TARGET_PARAM                    m_MoveTargetStart;
  TARGET_PARAM                    m_MoveTargetEnd;

  // MoveType::DISTANCE_ROTATION
  Trans::Value                    m_MoveTransType;      // 移動に使用する遷移型式
  ROTATION_PARAM                  m_MoveRotationStart;
  ROTATION_PARAM                  m_MoveRotationEnd;
  ShakeController                 m_ShakeController;

#if defined(GF_PLATFORM_WIN32)
  CameraRecorder*                 m_pCameraRecorder;
#endif

  //
  // 外部オブジェクト
  //
  poke_3d::model::BaseCamera*     m_pBaseCamera;        // カメラ実体



public:

// イベントカメラエディタ用
#if DEBUG_EVENT_CAMERA_EDITOR

  void GetTargetParamStart( TARGET_PARAM* pOut);
  void GetTargetParamEnd( TARGET_PARAM* pOut);
  void GetCommonParameters(
    ActionState::Value* pActionState,
    f32* pOutBank,
    u32* pMoveType,
    u32* pOutMoveEndFrame,
    u32* pOutMoveNowFrame,
    gfl2::math::Easing::EaseFunc* pOutEasingFunc
    );

  void SetTargetParamStart( const TARGET_PARAM& rIn);
  void SetTargetParamEnd( const TARGET_PARAM& rIn);
  void SetCommonParameters(
    ActionState::Value actionState,
    f32 bank,
    u32 moveType,
    u32 moveEndFrame,
    u32 moveNowFrame,
    gfl2::math::Easing::EaseFunc easingFunc
    );

#endif // DEBUG_EVENT_CAMERA_EDITOR


};  // class FieldControllerTypeEventScript


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__

  