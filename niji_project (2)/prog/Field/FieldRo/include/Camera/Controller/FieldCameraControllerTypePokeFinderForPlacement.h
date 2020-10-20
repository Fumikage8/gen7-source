//======================================================================
/**
 * @file FieldCameraControllerTypePokeFinderForPlacement.h
 * @date 2015/10/09 12:49:36
 * @author miyachi_soichi
 * @brief カメラ：ポケファインダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_CAMERACONTROLLER_TYPE_POKEFINDER_FOR_PLACEMENT_H_INCLUDED__
#define __FIELD_CAMERACONTROLLER_TYPE_POKEFINDER_FOR_PLACEMENT_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
// Camera
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
// finder
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Camera)
GFL_NAMESPACE_BEGIN(Controller)

/**
 *  @class  ControllerTypePokeFinderForPlacement
 *  @brief  ポケファインダーカメラ
 */
class ControllerTypePokeFinderForPlacement : public ControllerBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ControllerTypePokeFinderForPlacement);

public:
  struct SetupParam
  {
    gfl2::math::Vector3 position;
    gfl2::math::Vector3 rotate;
    gfl2::math::Vector3 limit_min;
    gfl2::math::Vector3 limit_max;
    f32                 zoom_maxi;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  ControllerTypePokeFinderForPlacement( const SetupParam &data );

  /**
   *  @brief  デストラクタ
   */
  virtual ~ControllerTypePokeFinderForPlacement( void );

  /**
   *  @brief  初期化処理
   */
  virtual void SetUp( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  更新処理
   */
  virtual void Update( poke_3d::model::BaseCamera *pCamera );

  //! ズームの取得
  inline f32 GetZoomMini( void ){ return m_Zoom_Mini; }
  inline f32 GetZoomMaxi( void ){ return m_Zoom_Maxi; }
  inline f32 GetZoom( void ){ return m_Zoom; }
  inline bool GetZoomInput( void ){ return m_IsZoomInput; }
  //! 入力
  inline void SetInputEnable( bool flag ){ m_IsInputEnable = flag; }
  inline bool GetInputEnable( void ){ return m_IsInputEnable; }
  //! ジャイロ
  inline void SetGyroEnable( bool flag ){ m_IsGyroEnable = flag; }
  inline bool GetGyroEnable( void ){ return m_IsGyroEnable; }

  /**
   *  @brief  座標
   */
  inline gfl2::math::Vector3 GetPosition( void ){ return m_Position; }

private:
  /**
   *  @brief  パッドの更新
   */
  void updatePad( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  ジャイロの更新
   */
  void updateGyro( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  カメラの更新
   */
  void updateCamera( poke_3d::model::BaseCamera *pCamera );

  /**
   *  @brief  スティック入力値を加工する
   */
  f32 processStickValue( f32 value );

  /**
   *  @brief  カウンターズーム
   */
  f32 calcCounterZoom( void );

private:
  gfl2::math::Vector3 m_Position;
  gfl2::math::Vector3 m_Rotate_Base;
  gfl2::math::Vector3 m_Rotate_Add;
  gfl2::math::Vector3 m_Limit_min;
  gfl2::math::Vector3 m_Limit_max;
  gfl2::math::Matrix34 m_mtx;

  f32 m_Zoom;
  f32 m_Zoom_Mini;
  f32 m_Zoom_Maxi;

  f32 m_Stick_Mini;
  f32 m_Stick_Maxi;
  f32 m_Stick_Ratio;
  f32 m_Stick_Counter;

  f32 m_Gyro_Ratio;

  bool m_IsInputEnable;
  bool m_IsGyroEnable;
  bool m_IsZoomInput;

#if PM_DEBUG
  //! 以下デバッグ用設定
public:
  f32 debug_getCamPosX( void ){ return m_Position.x; }
  f32 debug_getCamPosY( void ){ return m_Position.y; }
  f32 debug_getCamPosZ( void ){ return m_Position.z; }
  void debug_setCamPosX( f32 dst ){ m_Position.x = dst; }
  void debug_setCamPosY( f32 dst ){ m_Position.y = dst; }
  void debug_setCamPosZ( f32 dst ){ m_Position.z = dst; }
  f32 debug_getRotX( void ){ return gfl2::math::ConvRadToDeg( m_Rotate_Base.x ); }
  f32 debug_getRotY( void ){ return gfl2::math::ConvRadToDeg( m_Rotate_Base.y ); }
  void debug_setRotX( f32 dst ){ m_Rotate_Base.x = gfl2::math::ConvDegToRad(dst); }
  void debug_setRotY( f32 dst ){ m_Rotate_Base.y = gfl2::math::ConvDegToRad(dst); }
  f32 debug_getLimMinX( void ){ return gfl2::math::ConvRadToDeg(m_Limit_min.x); }
  f32 debug_getLimMinY( void ){ return gfl2::math::ConvRadToDeg(m_Limit_min.y); }
  void debug_setLimMinX( f32 dst ){ m_Limit_min.x = gfl2::math::ConvDegToRad(dst); }
  void debug_setLimMinY( f32 dst ){ m_Limit_min.y = gfl2::math::ConvDegToRad(dst); }
  f32 debug_getLimMaxX( void ){ return gfl2::math::ConvRadToDeg(m_Limit_max.x); }
  f32 debug_getLimMaxY( void ){ return gfl2::math::ConvRadToDeg(m_Limit_max.y); }
  void debug_setLimMaxX( f32 dst ){ m_Limit_max.x = gfl2::math::ConvDegToRad(dst); }
  void debug_setLimMaxY( f32 dst ){ m_Limit_max.y = gfl2::math::ConvDegToRad(dst); }
  f32 debug_getZoomMini( void ){ return m_Zoom_Mini; }
  void debug_setZoomMini( f32 dst ){ m_Zoom_Mini = dst; }
  f32 debug_getZoomMaxi( void ){ return m_Zoom_Maxi; }
  void debug_setZoomMaxi( f32 dst ){ m_Zoom_Maxi = dst; }
  f32 debug_getStickMini( void ){ return m_Stick_Mini; }
  void debug_setStickMini( f32 dst ){ m_Stick_Mini = dst; }
  f32 debug_getStickMaxi( void ){ return m_Stick_Maxi; }
  void debug_setStickMaxi( f32 dst ){ m_Stick_Maxi = dst; }
  f32 debug_getStickRatio( void ){ return m_Stick_Ratio; }
  void debug_setStickRatio( f32 dst ){ m_Stick_Ratio = dst; }
  f32 debug_getStickCounter( void ){ return m_Stick_Counter; }
  void debug_setStickCounter( f32 dst ){ m_Stick_Counter = dst; }
  f32 debug_getGyroRatio( void ){ return m_Gyro_Ratio; }
  void debug_setGyroRatio( f32 dst ){ m_Gyro_Ratio = dst; }

  bool debug_LockZoom;
#endif
};

GFL_NAMESPACE_END(Controller)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_CAMERACONTROLLER_TYPE_POKEFINDER_FOR_PLACEMENT_H_INCLUDED__
