/**
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
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Update( poke_3d::model::BaseCamera* pCamera ) = 0;

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
  * @brief 現在の設定値から生成されるビュー行列を取得する
  * @note  この関数がfalseを返す場合は、BaseCameraから行列を取得する
  * @note  この関数によりvirtualが毎回呼ばれる+インターフェイスじゃなくなるので削除する可能性がある
  *
  * @param  pOut 行列格納先( 戻り値 true 時のみ値が入る)
  *
  * @return 成否
  */
  virtual b32 GetViewMatrixOverride( gfl2::math::Matrix34* pOut ) const { return false; }


private:


};  // class FieldControllerBase


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__
