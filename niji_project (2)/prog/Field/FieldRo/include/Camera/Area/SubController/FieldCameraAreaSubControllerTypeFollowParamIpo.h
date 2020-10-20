﻿//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeFollowParamIpo.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( 現在位置から2つのパラメータが補間する追従型 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREASUBCONTROLLERERTYPEFOLLOWPARAMIPO_H_INCLUDED__
#define __FIELDCAMERAAREASUBCONTROLLERERTYPEFOLLOWPARAMIPO_H_INCLUDED__
#pragma once

// gfl2
#include <math/include/gfl2_SpringSimulation.h>

// Field
#include "FieldCameraAreaSubControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class Support;
class Focus;

GFL_NAMESPACE_BEGIN( SubController );

/**
* @brief フィールドカメラエリアの子操作( 現在位置から2つのパラメータが補間する追従型 )
*/
class SubControllerTypeFollowParamIpo : public SubControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( SubControllerTypeFollowParamIpo );

public:

  struct DATA
  {
    MAIN_PARAM                    paramA;         // A地点(0.0)のパラメータ
    MAIN_PARAM                    paramB;         // B地点(1.0)のパラメータ
    gfl2::math::Easing::EaseFunc  paramEaseFunc;  // パラメータ補間時に使用するイージング関数
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  SubControllerTypeFollowParamIpo( 
    const DATA& rData,
    const f32*  pParamRatio,              // パラメータ補間、開始時点の位置(0.0 - 1.0)
    const Target::TargetBase* pTarget,
    Area::Support*                  pSupport,
    Area::Focus*                    pFocus,
    s32 dataNo
#if PM_DEBUG
    ,b32 isDebugEffect = false
#endif

    );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~SubControllerTypeFollowParamIpo( void );

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
    return Type::FOLLOW_IPO;
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

#if PM_DEBUG

  /**
  * @brief　デバッグ操作の影響があるか取得する
  *
  * @return true ある false ない
  */
  virtual b32 IsDebugEffect( void ) const { return m_IsDebugEffect; }
  
    /**
  * @brief　カメラ情報を出力する必要があるか取得する
  *
  * @return true ある false ない
  */
  virtual b32 IsPutsInfoFile( void ) const { return m_IsPutsInfoFile; };


#endif // #if PM_DEBUG

private:

  DATA m_Data;
  s32  m_DataNo;

#if PM_DEBUG
  b32  m_IsDebugEffect;
  b32  m_IsPrevDebugUpdate;
  b32  m_IsPutsInfoFile;
#endif

  // 外部オブジェクト
  const Target::TargetBase* m_pTarget;                            // 注視対象
  const f32*                            m_pParamRatio;
  Area::Support*                        m_pSupport;
  Area::Focus*                          m_pFocus;
};

GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREASUBCONTROLLERERTYPEFOLLOWPARAMIPO_H_INCLUDED__