/**
* @brief  フィールドカメラの操作、デバッグ用正射影カメラ
* @file   FieldCameraControllerTypeDebugOrthoView.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEORTHOVIEW_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEORTHOVIEW_H_INCLUDED__
#pragma once

#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_Macros.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief フィールドカメラの操作、デバッグ用正射影カメラ
*/
class ControllerTypeDebugOrthoView : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeDebugOrthoView );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @param pTarget  注視対象
  *
  * @return 無し
  */ 
  ControllerTypeDebugOrthoView( const Target::TargetBase* pTarget );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeDebugOrthoView( void );
  
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
  * @brief 現在の設定値から生成されるプロジェクション行列を取得する
  * @note  この関数がfalseを返す場合は、BaseCameraから行列を取得する
  * @note  この関数によりvirtualが毎回呼ばれる+インターフェイスじゃなくなるので削除する可能性がある
  *
  * @param  pOut 行列格納先( 戻り値 true 時のみ値が入る)
  *
  * @return 成否
  */
  b32 GetProjectionMatrixOverride( gfl2::math::Matrix44* pOut ) const;

  /**
  * @brief 現在の設定値から生成されるビュー行列を取得する
  * @note  この関数がfalseを返す場合は、BaseCameraから行列を取得する
  * @note  この関数によりvirtualが毎回呼ばれる+インターフェイスじゃなくなるので削除する可能性がある
  *
  * @param  pOut 行列格納先( 戻り値 true 時のみ値が入る)
  *
  * @return 成否
  */
  b32 GetViewMatrixOverride( gfl2::math::Matrix34* pOut ) const;

private:

  //
  // 内部制御
  //
  u32                       m_DistanceLevel;

  //
  // 外部オブジェクト
  //
  const Target::TargetBase* m_pTarget;  // 注視対象

};  // class FieldControllerTypeDebugOrthoView


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // #if PM_DEBUG
#endif // __FIELDCAMERACONTROLLERERTYPEORTHOVIEW_H_INCLUDED__

  