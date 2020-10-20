/**
* @brief  フィールドカメラのターゲットインターフェイス
* @file   FieldCameraTargetBase.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERATARGETBASEER_H_INCLUDED__ )
#define __FIELDCAMERATARGETBASEER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );

/**
* @brief フィールドカメラのターゲットインターフェイス
*/
class TargetBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TargetBase );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TargetBase( void ) {}

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TargetBase( void ) {}
  
  /**
  * @brief カメラがみる座標を取得する
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const = 0;


private:


};  // class FieldTargetBase


GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETBASEER_H_INCLUDED__