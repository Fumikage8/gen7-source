//==============================================================================
/**
 * @file	FieldCameraControllerTypeJoinFesta.h
 * @brief	フィールドカメラの操作　ジョインフェスタ用
 * @author	ichiraku_katsuhiko
 * @date	2015/05/26 12:45:12
 */
// =============================================================================

#if !defined( __FIELDCAMERACONTROLLERERTYPEJOINFESTA_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEJOINFESTA_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field )
  class Fieldmap;
GFL_NAMESPACE_END( Field )

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Camera )
GFL_NAMESPACE_BEGIN( Controller )

/**
* @brief フィールドカメラの操作、テスト用
*/
class ControllerTypeJoinFesta : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeJoinFesta );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @param pTarget  注視対象
  *
  * @return 無し
  */ 
  ControllerTypeJoinFesta( Field::Fieldmap* pFieldmap );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeJoinFesta( void );
  
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


  //------------------------------------------------------------------
  /**
    *  @brief    補間
    *  @note     override
    */
  //------------------------------------------------------------------
  virtual b32 SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc );

private:
  /**
  * @brief 内部制御パラメータをカメラ実体に反映
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void SetParam( poke_3d::model::BaseCamera* pCamera );

  //
  // 内部制御
  //
  f32                       m_CameraDistance;   
  f32                       m_CameraHeight;   
  f32                       m_TargetDistance;   
  f32                       m_TargetHeight;   
  gfl2::math::Vector3       m_Rot;
  Area::Interpolation       m_Interpolation;

  //
  // 外部オブジェクト
  //
  Field::Fieldmap* m_pFieldmap;

};  // class FieldControllerTypeJoinFesta


GFL_NAMESPACE_END( Controller )
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_END( Field )

#endif // __FIELDCAMERACONTROLLERERTYPEJOINFESTA_H_INCLUDED__

 