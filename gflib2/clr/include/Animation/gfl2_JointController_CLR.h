#if !defined(GFLIB2_CLR_ANIMATION_JOINTCONTROLER_CLR_H_INCLUDED)
#define GFLIB2_CLR_ANIMATION_JOINTCONTROLER_CLR_H_INCLUDED
#pragma once

#include <Animation/include/gfl2_JointController.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ModelInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_MotionConfigInstanceNode_CLR.h>

#include "clr/include/poke_3d_lib/gfl2_PokemonModel_CLR.h"

namespace gfl2 { namespace clr { namespace animation { 

/**
 * @brief アニメーションコントローラー
 */
public ref class JointController_
{
public:

  JointController_();
	~JointController_() { this->!JointController_(); }
  !JointController_();

	void Initialize(
    gfl2::clr::poke_3d_lib::PokemonModel^ pokemonModel,
    gfl2::clr::renderingengine::scenegraph::instance::MotionConfigInstanceNode^ pMotionConfigNode);
  void Terminate();
  void Update();

  /**
   * @brief 注目位置設定
   *
   * @param position 注目位置
   */
  void SetInterestPosition(gfl2::clr::math::Vector^ position);

  /**
   * @brief 注目タイプ設定
   *
   * @param index インデックス
   * @param type 注目タイプ
   */
  void SetInterestType(u32 index, gfl2::clr::renderingengine::InterestType::Enum type);

  /**
   * @brief 注目タイプ取得
   *
   * @param index インデックス
   *
   * @return 注目タイプを返す
   */
  gfl2::clr::renderingengine::InterestType::Enum GetInterestType(u32 index);

  /**
   * @brief 注目速度設定
   *
   * @param index インデックス
   * @param speed 注目速度
   */
  void SetInterestSpeed(u32 index, f32 speed);

  /**
   * @brief 注目速度取得
   *
   * @param index インデックス
   *
   * @return 注目速度を返す
   */
  f32 GetInterestSpeed(u32 index);

  /**
   * @brief 角度制限：最小値設定
   *
   * @param index インデックス
   * @param low 角度制限：最小値
   */
  void SetInterestLow(u32 index, f32 low);

  /**
   * @brief 角度制限：最小値取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最小値を返す
   */
  f32 GetInterestLow(u32 index);

  /**
   * @brief 角度制限：最大値設定
   *
   * @param index インデックス
   * @param high 角度制限：最大値
   */
  void SetInterestHigh(u32 index, f32 high);

  /**
   * @brief 角度制限：最大値取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最大値を返す
   */
  f32 GetInterestHigh(u32 index);

  /**
   * @brief 角度制限：最小値２設定
   *
   * @param index インデックス
   * @param low 角度制限：最小値
   */
  void SetInterestLow2(u32 index, f32 low);

  /**
   * @brief 角度制限：最小値２取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最小値２を返す
   */
  f32 GetInterestLow2(u32 index);

  /**
   * @brief 角度制限：最大値２設定
   *
   * @param index インデックス
   * @param high 角度制限：最大値
   */
  void SetInterestHigh2(u32 index, f32 high);

  /**
   * @brief 角度制限：最大値２取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最大値２を返す
   */
  f32 GetInterestHigh2(u32 index);

  /**
   * @brief 注目解除にかかるフレーム数を設定
   *
   * @param index インデックス
   * @param resetFrameCount 注目解除にかかるフレーム数
   */
  void SetInterestResetFrameCount(u32 index, s32 resetFrameCount);

  /**
   * @brief 注目解除にかかるフレーム数を取得
   *
   * @param index インデックス
   *
   * @return 注目解除にかかるフレーム数を返す
   */
  s32 GetInterestResetFrameCount(u32 index);

private:
	gfl2::animation::JointController*	m_pNative;
  gfl2::math::Vector3* m_Pos;
};
}}}

#endif
