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
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModel;
GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
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
    // MoveModel::FieldMoveModel* m_pPlayerModel;
    // const MoveModel::FieldMoveModel* m_pPlayerModel; @todo constで座標を取る統べがない！

    u32 dummy;
    // @note 回転制限などが追加される
  };

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
  *
  * @return 成否
  */
  b32 Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Vector3& rTargetPosition, u32 frame , gfl2::math::Easing::EaseFunc easingFunc = gfl2::math::Easing::LINEAR );

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
  b32 Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Quaternion& rRotation, u32 frame, gfl2::math::Easing::EaseFunc easingFunc = gfl2::math::Easing::LINEAR );
  
  /**
  * @brief 動いているか取得
  * @note  アニメーション中もtrueを返す
  *
  * @return true 動いている  false 動いていない
  */
  b32 IsMoving( void ) const;

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

  //
  // 内部制御
  //
  ActionState::Value              m_ActionState;        // 行動の状態識別子


  // 移動命令制御
  u32                             m_MoveEndFrame;       // 移動終了フレーム
  u32                             m_MoveNowFrame;       // 現在フレーム
  gfl2::math::Quaternion          m_MoveStartRotation;  // 移動開始(回転)
  gfl2::math::Vector3             m_MoveStartPosition;  // 移動開始(位置)
  gfl2::math::Quaternion          m_MoveTargetRotation; // 移動目標(回転)
  gfl2::math::Vector3             m_MoveTargetPosition; // 移動目標(位置)
  gfl2::math::Easing::EaseFunc    m_MoveEasingFunc;     // 移動に使用するイージング関数


  //
  // 外部オブジェクト
  //
  poke_3d::model::BaseCamera*     m_pBaseCamera;        // カメラ実体 @todo システムで吸収？
  // MoveModel::FieldMoveModel* m_pPlayerModel;    // 自機
  // const MoveModel::FieldMoveModel* m_pPlayerModel; @todo constで座標を取る統べがない！

};  // class FieldControllerTypeEventScript


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__

  