#include <clr/include/Animation/gfl2_JointController_CLR.h>

namespace gfl2 { namespace clr { namespace animation {

JointController_::JointController_()
{
  m_pNative = new gfl2::animation::JointController(); 
  m_Pos = new gfl2::math::Vector3();
}

JointController_::!JointController_()
{
  if (m_pNative)
  {
   this->Terminate();
    delete m_pNative;
    m_pNative = NULL;
  }

  if (m_Pos)
  {
    delete m_Pos;
    m_Pos = NULL;
  }
}

void JointController_::Initialize(
    gfl2::clr::poke_3d_lib::PokemonModel^ pokemonModel,
    gfl2::clr::renderingengine::scenegraph::instance::MotionConfigInstanceNode^ pMotionConfigNode)
{
  m_pNative->Initialize(
    dynamic_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pokemonModel->GetNative()->GetModelInstanceNode()),
      pMotionConfigNode->GetNative());
}

void JointController_::Terminate()
{
  m_pNative->Finalize();
}

void JointController_::Update()
{
  m_pNative->Update();
}

/**
 * @brief 注目位置設定
 *
 * @param position 注目位置
 */
void JointController_::SetInterestPosition(gfl2::clr::math::Vector^ position)
{
  if (position == nullptr)
  {
    m_pNative->GetInterestController()->SetInterestPosition(NULL);
    return;
  }

  *m_Pos = position->GetNativeVector3();
  m_pNative->GetInterestController()->SetInterestPosition(m_Pos);
}

/**
 * @brief 注目タイプ設定
 *
 * @param index インデックス
 * @param type 注目タイプ
 */
void JointController_::SetInterestType(u32 index, gfl2::clr::renderingengine::InterestType::Enum type)
{
  m_pNative->GetInterestController()->SetType(index, static_cast<gfl2::renderingengine::InterestType::Enum>(type));
}

/**
 * @brief 注目タイプ取得
 *
 * @param index インデックス
 *
 * @return 注目タイプを返す
 */
gfl2::clr::renderingengine::InterestType::Enum JointController_::GetInterestType(u32 index)
{
  return static_cast<gfl2::clr::renderingengine::InterestType::Enum>((u32)m_pNative->GetInterestController()->GetType(index));
}

/**
 * @brief 注目速度設定
 *
 * @param index インデックス
 * @param speed 注目速度
 */
void JointController_::SetInterestSpeed(u32 index, f32 speed)
{
  m_pNative->GetInterestController()->SetSpeed(index, speed);
}

/**
 * @brief 注目速度取得
 *
 * @param index インデックス
 *
 * @return 注目速度を返す
 */
f32 JointController_::GetInterestSpeed(u32 index)
{
  return m_pNative->GetInterestController()->GetSpeed(index);
}

/**
 * @brief 角度制限：最小値設定
 *
 * @param index インデックス
 * @param low 角度制限：最小値
 */
void JointController_::SetInterestLow(u32 index, f32 low)
{
  m_pNative->GetInterestController()->SetLow(index, low);
}

/**
 * @brief 角度制限：最小値取得
 *
 * @param index インデックス
 *
 * @return 角度制限：最小値を返す
 */
f32 JointController_::GetInterestLow(u32 index)
{
  return m_pNative->GetInterestController()->GetLow(index);
}

/**
 * @brief 角度制限：最大値設定
 *
 * @param index インデックス
 * @param high 角度制限：最大値
 */
void JointController_::SetInterestHigh(u32 index, f32 high)
{
  m_pNative->GetInterestController()->SetHigh(index, high);
}

/**
 * @brief 角度制限：最大値取得
 *
 * @param index インデックス
 *
 * @return 角度制限：最大値を返す
 */
f32 JointController_::GetInterestHigh(u32 index)
{
  return m_pNative->GetInterestController()->GetHigh(index);
}

/**
 * @brief 角度制限：最小値２設定
 *
 * @param index インデックス
 * @param low 角度制限：最小値
 */
void JointController_::SetInterestLow2(u32 index, f32 low)
{
  m_pNative->GetInterestController()->SetLow2(index, low);
}

/**
 * @brief 角度制限：最小値２取得
 *
 * @param index インデックス
 *
 * @return 角度制限：最小値２を返す
 */
f32 JointController_::GetInterestLow2(u32 index)
{
  return m_pNative->GetInterestController()->GetLow2(index);
}

/**
 * @brief 角度制限：最大値２設定
 *
 * @param index インデックス
 * @param high 角度制限：最大値
 */
void JointController_::SetInterestHigh2(u32 index, f32 high)
{
  m_pNative->GetInterestController()->SetHigh2(index, high);
}

/**
 * @brief 角度制限：最大値２取得
 *
 * @param index インデックス
 *
 * @return 角度制限：最大値２を返す
 */
f32 JointController_::GetInterestHigh2(u32 index)
{
  return m_pNative->GetInterestController()->GetHigh2(index);
}

/**
 * @brief 注目解除にかかるフレーム数を設定
 *
 * @param index インデックス
 * @param resetFrameCount 注目解除にかかるフレーム数
 */
void JointController_::SetInterestResetFrameCount(u32 index, s32 resetFrameCount)
{
  m_pNative->GetInterestController()->SetResetFrameCount(index, resetFrameCount);
}

/**
 * @brief 注目解除にかかるフレーム数を取得
 *
 * @param index インデックス
 *
 * @return 注目解除にかかるフレーム数を返す
 */
s32 JointController_::GetInterestResetFrameCount(u32 index)
{
  return m_pNative->GetInterestController()->GetResetFrameCount(index);
}

}}}
