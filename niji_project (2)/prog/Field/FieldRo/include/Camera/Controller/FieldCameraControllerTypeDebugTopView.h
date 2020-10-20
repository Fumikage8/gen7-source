//==============================================================================
/**
 * @file FieldCameraControllerTypeDebugTopView.h
 * @brief デバッグ用トップビューカメラ
 * @author saita_kazuki
 * @date 2015/11/23
 */
// =============================================================================

#if PM_DEBUG

#if !defined( __FIELDCAMERACONTROLLERERTYPEDEBUGTOPVIEW_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEDEBUGTOPVIEW_H_INCLUDED__
#pragma once

#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"

// 前方参照
namespace Field {
  class IField3DObjectNode;
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Camera )
GFL_NAMESPACE_BEGIN( Controller )

/**
* @class デバッグ用トップビューカメラ
*/
class ControllerTypeDebugTopView : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeDebugTopView );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @param 無し
  *
  * @return 無し
  */ 
  ControllerTypeDebugTopView();

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeDebugTopView( void );
  
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



  // ここから先はこのクラスでの独自定義

  /**
  * @brief ターゲットオブジェクトを設定
  *
  * @param  注視点を取得するフィールド3Dオブジェクト
  *
  * @return 無し
  */
  void SetTargetObject( Field::IField3DObjectNode* pObject);

  /**
  * @brief ターゲット位置を設定
  *
  * @param  注視点とする位置
  *
  * @return 無し
  */
  void SetTargetPosition( gfl2::math::Vector3* pPosition);

  /**
  * @brief 注視点とカメラ位置の距離を設定
  *
  * @param 距離
  *
  * @return 無し
  */
  void SetDistance( f32 distance);

private:

  gfl2::math::Vector3 GetTargetPosition();

private:

  f32                                 m_distance;
  Field::IField3DObjectNode*          m_pTargetObject;
  gfl2::math::Vector3*                m_pTargetPosition;


};  // class FieldControllerTypeDebugTopView


GFL_NAMESPACE_END( Controller )
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_END( Field )

#endif // #if defined(GF_ENABLE_DEBUG_EDITOR)
#endif // __FIELDCAMERACONTROLLERERTYPEDEBUGTOPVIEW_H_INCLUDED__
#endif // PM_DEBUG

