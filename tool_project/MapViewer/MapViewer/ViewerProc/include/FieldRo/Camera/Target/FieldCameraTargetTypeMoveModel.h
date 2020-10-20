/**
* @brief  フィールドカメラのターゲット、動作モデル
* @file   FieldCameraTargetTypeMoveModel.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERATARGETTYPEMOVEMODELER_H_INCLUDED__ )
#define __FIELDCAMERATARGETTYPEMOVEMODELER_H_INCLUDED__
#pragma once

// field
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModel;
GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );

/**
* @brief フィールドカメラのターゲット、動作モデル
*/
class TargetTypeMoveModel : public TargetBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TargetTypeMoveModel );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @param  pMoveModel 中身、寿命は管理しない
  *
  * @return 無し
  */ 
  TargetTypeMoveModel( MoveModel::FieldMoveModel* pMoveModel = 0 );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TargetTypeMoveModel( void );
  
  /**
  * @brief カメラがみる座標を取得する
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const;

  /**
  * @brief 中身を設定
  *
  * @param  pMoveModel 中身、寿命は管理しない
  *
  * @return 無し
  */ 
  void SetContent( MoveModel::FieldMoveModel* pMoveModel );

private:


  //
  // 外部オブジェクト
  //
  MoveModel::FieldMoveModel* m_pMoveModel;


};  // class FieldTargetTypeMoveModel


GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETTYPEMOVEMODELER_H_INCLUDED__