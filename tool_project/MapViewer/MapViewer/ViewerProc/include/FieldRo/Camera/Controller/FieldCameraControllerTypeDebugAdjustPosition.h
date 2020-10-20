/**
* @brief  フィールドカメラの操作、デバッグ用カメラ位置調整機能
* @file   FieldCameraControllerTypeDebugAdjustPosition.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEADUSTPOSITION_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEADUSTPOSITION_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief フィールドカメラの操作、デバッグ用カメラ位置調整機能
*/
class ControllerTypeAdjustPosition : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeAdjustPosition );

public:

  struct ControlType
  {
    enum Value
    {
      TARGET_POSITION = 0,    // ターゲット
    };
  };

 
  /**
  * @brief コンストラクタ
  *
  * @param pTarget  注視対象
  *
  * @return 無し
  */ 
  ControllerTypeAdjustPosition( const Target::TargetBase* pTarget );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeAdjustPosition( void );
  
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
  * @brief スクリプター向け情報をDump
  *
  * @return 無し
  */
  void DumpForScript( void );

  /**
  * @brief カメラの状態を操作開始時点にリセットする
  *
  * @return 無し
  */
  void Reset( void );

private:

  //
  // 内部制御
  //
  ControlType::Value        m_ControlType;    // 制御型式
  gfl2::math::Vector3       m_TargetPositon;  // ターゲット座標
  gfl2::math::Vector3       m_CameraPositon;  // カメラ座標
  f32                       m_Distance;   
  gfl2::math::Vector3       m_Rot;


  // 開始時点情報
  gfl2::math::Vector3       m_StartTargetPositon;
  f32                       m_StartDistance;   
  gfl2::math::Vector3       m_StartRot;


  //
  // 外部オブジェクト
  //
  const Target::TargetBase* m_pTarget;  // 注視対象

};  // class FieldControllerTypeAdjustPosition


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEADUSTPOSITION_H_INCLUDED__

  