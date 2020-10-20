//======================================================================
/**
 * @file FieldGimmickEncountWork.h
 * @date 2015/09/04 16:59:09
 * @author saita_kazuki
 * @brief ギミックエンカウント共通アクセスワーク
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_WORK_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_WORK_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector3.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @class ギミックエンカウント共通アクセスワーク
 */
class Work
{
  GFL_FORBID_COPY_AND_ASSIGN( Work );

public:

  /**
   * @brief 更新情報
   */
  struct UpdateDescription
  {
    gfl2::math::Vector3 playerPosition;
  };

public:

  /**
   * @brief コンストラクタ
   */
  Work();

  /**
   * @brief デストラクタ
   */
  virtual ~Work();

  /**
   * @brief 初期化
   */
  void Initialize();

  /**
   * @brief 破棄
   */
  void Terminate();

  /**
   * @brief 更新
   * @param desc 更新情報
   */
  void Update( const UpdateDescription& desc);

  /**
   * @brief getter
   */
  f32 GetPlayerWalkSpeed() const { return PLAYER_MOVE_SPEED[PLAYER_MOVE_SPEED_WALK]; }
  f32 GetPlayerRunSpeed() const { return PLAYER_MOVE_SPEED[PLAYER_MOVE_SPEED_RUN]; }
  const gfl2::math::Vector3& GetPlayerPosition() const { return m_playerPosition; }

private:

  enum PlayerMoveSpeed
  {
    PLAYER_MOVE_SPEED_WALK,
    PLAYER_MOVE_SPEED_RUN,
    PLAYER_MOVE_SPEED_MAX,
  };

  static const f32 PLAYER_MOVE_SPEED[PLAYER_MOVE_SPEED_MAX];    ///< プレイヤー移動速度

  gfl2::math::Vector3     m_playerPosition;     ///< 自機の位置

};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_WORK_H_INCLUDED__

