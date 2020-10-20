/**
* @brief  フィールドカメラの操作、テスト用
* @file   FieldCameraControllerTypeTest.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPETEST_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPETEST_H_INCLUDED__
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
* @brief フィールドカメラの操作、テスト用
*/
class ControllerTypeTest : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeTest );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @param pTarget  注視対象
  *
  * @return 無し
  */ 
  ControllerTypeTest( const Target::TargetBase* pTarget );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeTest( void );
  
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

private:

  //
  // 内部制御
  //
  f32                       m_Distance;   
  gfl2::math::Vector3       m_Rot;

  //
  // 外部オブジェクト
  //
  const Target::TargetBase* m_pTarget;  // 注視対象

};  // class FieldControllerTypeTest


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPETEST_H_INCLUDED__

  