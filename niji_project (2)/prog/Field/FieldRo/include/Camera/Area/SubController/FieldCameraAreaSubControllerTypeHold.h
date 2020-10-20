//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeHold.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( 固定型 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREASUBCONTROLLERERTYPEHOLD_H_INCLUDED__
#define __FIELDCAMERAAREASUBCONTROLLERERTYPEHOLD_H_INCLUDED__
#pragma once

// gfl2
#include <math/include/gfl2_SpringSimulation.h>
#include <str/include/gfl2_MsgData.h>

// Field
#include "FieldCameraAreaSubControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
GFL_NAMESPACE_BEGIN( SubController );


/**
* @brief フィールドカメラエリアの子操作( 単純追従型 )
*/
class SubControllerTypeHold : public SubControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( SubControllerTypeHold );

public:

  struct DATA
  {
    b32                 isTargetFocus;   // ターゲット
    gfl2::math::Vector3 offset;          // ( isTargetFocus == true のときのみ有効 )

    gfl2::math::Vector3 position;
    gfl2::math::Vector3 target;          // ( isTargetFocus == false のときのみ有効 )
    f32                 bank;
    f32                 fovY;
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  SubControllerTypeHold( 
    const DATA& rData,
    const Target::TargetBase* pTarget,
    s32 dataNo
    );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~SubControllerTypeHold( void );

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Update( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 子操作のタイプを取得
  *
  * @return 子操作のタイプ定義
  */
  virtual Type::Value GetType( void ) const
  {
    return Type::HOLD;
  }

  /**
  * @brief データを取得
  *
  * @return なし
  */
  void GetData( DATA* pOut ) const
  {
    *pOut = m_Data;
  }

private:

  /**
  * @brief　最新のターゲット座標を取得する
  *
  * @return 無し
  */
  void GetTargetPosition( gfl2::math::Vector3* pOut ) const;

  
  DATA m_Data;
  s32  m_DataNo;

  // 外部オブジェクト
  const Target::TargetBase*             m_pTarget;                // 注視対象
};

GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREASUBCONTROLLERERTYPEHOLD_H_INCLUDED__