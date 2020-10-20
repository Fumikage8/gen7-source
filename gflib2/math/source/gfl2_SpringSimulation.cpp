#pragma once
//=================================================================================================
/**
 *  @file   SpringSimulation.cpp
 *  @brief  バネシミュレーション
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================

#include <math/include/gfl2_math_control.h>
#include <math/include/gfl2_SpringSimulation.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(math)

// デフォルトの自然長
f32 SpringSimulation::NATURAL_LENGTH = 0.0f;
// デフォルトの重力
f32 SpringSimulation::GRAVITY        = 9.80665f;
// デフォルトの微小時間
f32 SpringSimulation::DELTA_TIME     = 0.03333333f;

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
SpringSimulation::SpringSimulation()
{
  Init();
}

//-----------------------------------------------------------------------------
/**
*  @brief  更新
*/
//-----------------------------------------------------------------------------
void SpringSimulation::Update(void)
{
  if( !m_isUpdate )
  {
    return;
  }

  // 力を求める
  f32 force = -m_k*(m_position - m_naturalLength) + m_mass * m_gravity;
  // 加速度を求める
  f32 accel = force / m_mass;
  // 速度を求める
  m_velocity += accel * m_dt;
  m_velocity *= m_damping;
  // 位置を求める
  m_position += m_velocity;
  // 速度が収まると止める
  const f32 THRESHOLD = 0.001f;
  if( gfl2::math::Abs(m_velocity) < THRESHOLD )
  {
    Init();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief 跳ねるリクエスト
*
*  @param  mass          質量
*  @param  k             バネ定数
*  @param  firstVelocity 初速度
*  @param  damping       減衰率
*
*  @return 無し
*/
//-----------------------------------------------------------------------------
void SpringSimulation::Request(f32 mass,f32 k,f32 firstVelocity,f32 damping)
{
  m_isUpdate        = true;

  // 設定を簡略化するため一部のパラメータをデフォルト値を使う
  m_naturalLength   = NATURAL_LENGTH;
  m_dt              = DELTA_TIME;
  m_gravity         = GRAVITY;
  m_position        = 0.0f;
  // 可変値
  m_k               = k;
  m_mass            = mass;
  m_damping         = damping;
  m_velocity        = firstVelocity;
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化
*/
//-----------------------------------------------------------------------------
void SpringSimulation::Init(void)
{
  m_isUpdate      = false;
  m_position      = 0.0f;
  m_naturalLength = 0.0f;
  m_dt            = 0.0f;
  m_gravity       = 0.0f;
  m_k             = 0.0f;
  m_mass          = 0.0f;
  m_damping       = 0.0f;
  m_velocity      = 0.0f;
}

//-----------------------------------------------------------------------------
/**
*  @brief  バネの先端についている重りの座標を取得
*
*  @return 座標
*/
//-----------------------------------------------------------------------------
f32 SpringSimulation::GetPostion(void)
{
  return m_position;
}

GFL_NAMESPACE_END(math)
GFL_NAMESPACE_END(gfl2)
