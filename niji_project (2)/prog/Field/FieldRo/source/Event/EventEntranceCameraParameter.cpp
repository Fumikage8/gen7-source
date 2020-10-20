//======================================================================
/**
 * @file EventEntranceCameraParameter.cpp
 * @date 2015/11/08 15:41:48
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <Debug/include/gfl2_Assert.h>


#include "Field/FieldRo/include/Event/EventEntranceCameraParameter.h"
GFL_NAMESPACE_BEGIN(Field)


/**
 *  @brief コンストラクタ
 */
EventEntranceCameraParameter::EventEntranceCameraParameter() : 
   m_cpBuff(NULL)
  ,m_cpParams(NULL)
  ,m_Size(0)
{
}



/**
 *  @brief デストラクタ 
 */
EventEntranceCameraParameter::~EventEntranceCameraParameter()
{
}

/**
 *  @brief 初期化
 */
void EventEntranceCameraParameter::Initialize( const void* cpbuff, int size )
{
  m_cpBuff    = cpbuff;
  m_cpParams  = reinterpret_cast<const Param*>(cpbuff);
  m_Size = size / sizeof(Param);
}

/**
 *  @brief データ数の取得
 */
u32 EventEntranceCameraParameter::GetDataMax() const
{
  return m_Size;
}

/**
 *  @brief parameter取得
 */
const EventEntranceCameraParameter::Param& EventEntranceCameraParameter::GetParam( u32 idx ) const
{
  if( idx < m_Size )
  {
    return m_cpParams[idx];
  }
  else
  {
    GFL_ASSERT(0);

    return m_cpParams[0];
  }
}



void EventEntranceCameraParameter::GetTypeTarget_CameraPos( const Param& data, gfl2::math::Vector3* p_out )
{
  p_out->SetX( data.float_array[P_TARGET_CAMERA_POS_X] );
  p_out->SetY( data.float_array[P_TARGET_CAMERA_POS_Y] );
  p_out->SetZ( data.float_array[P_TARGET_CAMERA_POS_Z] );
}

void EventEntranceCameraParameter::GetTypeTarget_TargetPos( const Param& data, gfl2::math::Vector3* p_out )
{
  p_out->SetX( data.float_array[P_TARGET_TARGET_POS_X] );
  p_out->SetY( data.float_array[P_TARGET_TARGET_POS_Y] );
  p_out->SetZ( data.float_array[P_TARGET_TARGET_POS_Z] );
}


void EventEntranceCameraParameter::GetTypeRotate_TargetPos( const Param& data, gfl2::math::Vector3* p_out )
{
  p_out->SetX( data.float_array[P_ROTATE_TARGET_POS_X] );
  p_out->SetY( data.float_array[P_ROTATE_TARGET_POS_Y] );
  p_out->SetZ( data.float_array[P_ROTATE_TARGET_POS_Z] );
}

void EventEntranceCameraParameter::GetTypeRotate_Rotate( const Param& data, gfl2::math::Vector3* p_out )
{
  p_out->SetX( data.float_array[P_ROTATE_ROTATE_X] );
  p_out->SetY( data.float_array[P_ROTATE_ROTATE_Y] );
  p_out->SetZ( data.float_array[P_ROTATE_ROTATE_Z] );
}

f32 EventEntranceCameraParameter::GetTypeRotate_Distance( const Param& data )
{
  return data.float_array[P_ROTATE_DISTANCE];
}

GFL_NAMESPACE_END(Field)
