//======================================================================
/**
 * @file EventEntranceCameraParameter.h
 * @date 2015/11/08 15:41:12
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EVENT_ENTRANCE_CAMERA_PARAMETER_H_INCLUDED__
#define __EVENT_ENTRANCE_CAMERA_PARAMETER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Math.h>

#include "niji_conv_header/field/mapjump/FieldMapJumpCameraParameter.h"

GFL_NAMESPACE_BEGIN(Field)


  /**
   * @class EventEntranceCameraParameter
   * @brief カメラパラメーター 
   */
  class EventEntranceCameraParameter
  {
      GFL_FORBID_COPY_AND_ASSIGN(EventEntranceCameraParameter);

  public:
    enum
    {
      TYPE_TARGET,
      TYPE_ROTATE,
    };

    enum
    {
      // type[TARGET]
      P_TARGET_CAMERA_POS_X = 0,
      P_TARGET_CAMERA_POS_Y,
      P_TARGET_CAMERA_POS_Z,
      P_TARGET_TARGET_POS_X,
      P_TARGET_TARGET_POS_Y,
      P_TARGET_TARGET_POS_Z,


      // type[ROTATE]
      P_ROTATE_TARGET_POS_X = 0,
      P_ROTATE_TARGET_POS_Y,
      P_ROTATE_TARGET_POS_Z,
      P_ROTATE_ROTATE_X,
      P_ROTATE_ROTATE_Y,
      P_ROTATE_ROTATE_Z,
      P_ROTATE_DISTANCE,

      // Float配列parameterの配列要素数
      P_FLOAT_ARRAY_MAX = P_ROTATE_DISTANCE + 1
    };

    struct Param
    {
      u32 type;
      u32 easing;
      f32 bank;
      u32 frame;
      u32 play_start_frame;
      f32 float_array[P_FLOAT_ARRAY_MAX];
    };

  public:
    /**
     *  @brief コンストラクタ
     */
    EventEntranceCameraParameter();
    /**
     *  @brief デストラクタ 
     */
    ~EventEntranceCameraParameter();

    /**
     *  @brief 初期化
     */
    void Initialize( const void* cpbuff, int size );

    /**
     *  @brief データ数の取得
     */
    u32 GetDataMax() const;

    /**
     *  @brief parameter取得
     */
    const Param& GetParam( u32 idx ) const;

    static void GetTypeTarget_CameraPos( const Param& data, gfl2::math::Vector3* p_out );
    static void GetTypeTarget_TargetPos( const Param& data, gfl2::math::Vector3* p_out );

    static void GetTypeRotate_TargetPos( const Param& data, gfl2::math::Vector3* p_out );
    static void GetTypeRotate_Rotate( const Param& data, gfl2::math::Vector3* p_out );
    static f32 GetTypeRotate_Distance( const Param& data );


  private:
    const void *  m_cpBuff;
    const Param*  m_cpParams;
    u32           m_Size;
  };

GFL_NAMESPACE_END(Field)

#endif // __EVENT_ENTRANCE_CAMERA_PARAMETER_H_INCLUDED__