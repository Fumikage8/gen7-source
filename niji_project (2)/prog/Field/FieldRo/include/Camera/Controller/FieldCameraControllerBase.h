﻿/**
* @brief  フィールドカメラの操作インターフェイス
* @file   FieldCameraControllerBase.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__ )
#define __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief フィールドカメラの操作インターフェイス
*/
class ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerBase );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  ControllerBase( void ) {}

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerBase( void ) {}
  

  /**
  * @brief 初期化処理
  * @note  ControllerがCameraUnitの設定された直後に実行される
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void SetUp( poke_3d::model::BaseCamera* pCamera ) = 0;

  /**
  * @brief 外したときの処理
  * @note  ControllerがCameraUnitから交換される直前に実行される
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Remove( poke_3d::model::BaseCamera* pCamera ) {};

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Update( poke_3d::model::BaseCamera* pCamera ) = 0;

  /**
  * @brief 中断処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Pause( poke_3d::model::BaseCamera* pCamera ) {};

  /**
  * @brief 再開処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Resume( poke_3d::model::BaseCamera* pCamera ) {};
  
  /**
  * @brief 補間を打ち切り、最新の状態に更新する
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Fix( poke_3d::model::BaseCamera* pCamera ) {};

  /**
  * @brief 現在の設定値から生成されるプロジェクション行列を取得する
  * @note  この関数がfalseを返す場合は、BaseCameraから行列を取得する
  * @note  この関数によりvirtualが毎回呼ばれる+インターフェイスじゃなくなるので削除する可能性がある
  *
  * @param  pOut 行列格納先( 戻り値 true 時のみ値が入る)
  *
  * @return 成否
  */
  virtual b32 GetProjectionMatrixOverride( gfl2::math::Matrix44* pOut ) const { return false; }

  /**
  * @brief 正射影か？
  *
  * @return true 正射影 false 射影
  */
  virtual b32 IsOrtho( void ) const { return false; }

  /**
  * @brief 現在の設定値から生成されるビュー行列を取得する
  * @note  この関数がfalseを返す場合は、BaseCameraから行列を取得する
  * @note  この関数によりvirtualが毎回呼ばれる+インターフェイスじゃなくなるので削除する可能性がある
  *
  * @param  pOut 行列格納先( 戻り値 true 時のみ値が入る)
  *
  * @return 成否
  */
  virtual b32 GetViewMatrixOverride( gfl2::math::Matrix34* pOut ) const { return false; }

  virtual b32 SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc ){ return false ;}

  /**
  * @brief カメラシェイクリクエスト(水平方向)
  *
  * @param  mass          質量
  * @param  k             バネ定数
  * @param  firstVelocity 初速度
  * @param  damping       減衰率
  *
  * @return リクエスト受理の成否
  */
  virtual b32 ShakeHorizonRequest(f32 mass,f32 k,f32 firstVelocity,f32 damping){ return false; }

  /**
  * @brief カメラシェイクリクエスト(垂直方向)
  *
  * @param  mass          質量
  * @param  k             バネ定数
  * @param  firstVelocity 初速度
  * @param  damping       減衰率
  *
  * @return リクエスト受理の成否
  */
  virtual b32 ShakeVerticalRequest(f32 mass,f32 k,f32 firstVelocity,f32 damping){ return false; }
  
private:


};  // class FieldControllerBase


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__
