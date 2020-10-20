//======================================================================
/**
 * @file FieldCameraControllerTypeDebugTrainer.h
 * @date 2015/10/05 12:33:20
 * @author miyachi_soichi
 * @brief デバッグカメラ：トレーナー用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_CAMERACONTROLLER_TYPE_DEBUGTRAINER_H_INCLUDED__
#define __FIELD_CAMERACONTROLLER_TYPE_DEBUGTRAINER_H_INCLUDED__
#pragma once

#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_Macros.h>
// Camera
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Camera)
GFL_NAMESPACE_BEGIN(Target)
class TargetBase;
GFL_NAMESPACE_END(Target)
GFL_NAMESPACE_BEGIN(Controller)

/**
 *  @class  ControllerTypeDebugTrainer
 *  @brief  トレーナーデバッグ用カメラ
 */
class ControllerTypeDebugTrainer : public ControllerBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ControllerTypeDebugTrainer);

public:
  /**
   *  @brief  コンストラクタ
   */
  ControllerTypeDebugTrainer( const Target::TargetBase *pTarget );

  /**
   *  @brief  デストラクタ
   */
  virtual ~ControllerTypeDebugTrainer( void );

  /**
   *  @brief  初期化処理
   */
  virtual void SetUp( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  更新処理
   */
  virtual void Update( poke_3d::model::BaseCamera *pCamera );

private:
  /**
   *  @brief  操作による更新
   */
  void updatePad( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  対象座標の更新
   */
  void updateTarget( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  カメラ更新
   */
  void updateCamera( poke_3d::model::BaseCamera *pCamera );

private:
  const Target::TargetBase    *m_pTarget;
  gfl2::math::Vector3         m_vCameraPosition;
  gfl2::math::Vector3         m_vTargetPosition;
  gfl2::math::Vector3         m_vRotation;
  f32                         m_fDistance;
};

GFL_NAMESPACE_END(Controller)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
#endif // __FIELD_CAMERACONTROLLER_TYPE_DEBUGTRAINER_H_INCLUDED__
